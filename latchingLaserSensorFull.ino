/*
  Latching Laser Sensor

  This will grow to accomodate the scenarios for the laser maze.

  Latches an output in until reset when a signal is detected/missing.

  created 2025
  by Brad Swearingen
*/

// constants won't change. Used here to set a pin number:
// separate your inputs and output to make this cleaner, I'd love to array each set, but the reads/writes may be too low level to maximize this
const int ledPin = LED_BUILTIN;  // the number of the LED pin

// const int sensor1StatePin = 24;
// const int led1StatePin = 25;
const int resetButtonStatePin = 17;

// Rotating sensor setup
const int led2StatePin = 18;
const int degradeStatePin = 19;

// Rotating sensor arrays
const int fullRotatingLedOutputStatePins[]={23,24,25,26,27,28,29,30,31,32,
                                            33,34,35,36,37,38,39,40,41,42,43,
                                            44,45,46,47,48,49,50,51,52,53};
const int fullRotatingSensorInputStatePins[]={ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
                                              11,12,13,14,15,16,54,55,56,57,58,
                                              59,60,61,62,63,64,65,66,67,68,69};
                                              // still have 17,18,19,20,21,22
bool rotatingSensorMonitorStatus[] = {false, false, false, false, false, false, false, false, false, false, false,
                                      false, false, false, false, false, false, false, false, false, false, false,
                                      false, false, false, false, false, false, false, false, false, false, false
                                      };
//do I need this, or do I just use math? I probably don't even need the previous structure, just a timer
// if each light is "off" for 1.5 seconds, with a .5 second overlap, and there are 33 lights, with a 50ms gap on each side for monitoring
// then the formula for if an LED is on is just: 
// currentms = (current - start) % (arraylen * 500)
// and state is on if (current ms < (loc * 500) && current_ms > (loc * 500)) << tweak for edge cases, OR run the pattern off the end
// monitory is just -50 and +50 respectively
// actually, even simpler. increment the number every rotatingCycleTime
// everything is ON except that number plus 2 (wrap array, make 2 longer if you want to fade off)
// monitoring just looks at the clock - at 0to50ms, minus 1 isn't monitored either, at 450to500, +3 is no longer monitored
// will also need to NOT monitor runoff locations
// unsigned long rotatingSensorMonitorMillis[] = {0, 0};
unsigned long previousMillis = 0;
int ledPinsLength = 0;  // Will be set in setup() to ensure all arrays match
int currentLed = 0;

// Variables will change:
int resetButtonStatus = LOW;
int degradeInputStatus = LOW;


// Rotating sensor variables
int rotatingSensorStatus = LOW;
int led2Status = LOW;
int previousDegradedMode = false; 
//static int currentPair = 0;  // 0 = pair 1 (LED 30, Sensor 31), 1 = pair 2 (LED 32, Sensor 33)
// static int state = 0;  // 0 = LED on, 1 = listening, 2 = LED off, 3 = listeningunsigned long currentMillis = millis();
// Rotating sensor timing constants
const long rotatingStartDelay = 50;  // 1/20 second
const long rotatingCycleTime = 750;   // 1/2 second
const int rotatingNumberOff = 3;

//method declarations
// void flashErrorCode(int);
// int rotatingArray(const int[], const int[], bool[], unsigned long[], bool);
int ison(int, int, int);

// 
void setup() {
  // Validate and set rotating array lengths
  // int ledPinsLength = sizeof(rotatingLedStatePins) / sizeof(rotatingLedStatePins[0]);
  // int sensorPinsLength = sizeof(rotatingSensorStatePins) / sizeof(rotatingSensorStatePins[0]);
  // int monitorStatusLength = sizeof(rotatingSensorMonitorStatus) / sizeof(rotatingSensorMonitorStatus[0]);
  // int monitorMillisLength = sizeof(rotatingSensorMonitorMillis) / sizeof(rotatingSensorMonitorMillis[0]);
  // rotatingArrayLength = ledPinsLength;
  
  // if(ledPinsLength != sensorPinsLength || ledPinsLength != monitorStatusLength || ledPinsLength != monitorMillisLength) {
  //   // Arrays must be equal length - halt execution
  //   while(1) {
  //     flashErrorCode(5);  // Error code 5: Array length mismatch
  //   }
    ledPinsLength = 31;
    for(int i = 0; i < ledPinsLength; i++){
      pinMode(fullRotatingLedOutputStatePins[i], OUTPUT);
      digitalWrite(fullRotatingLedOutputStatePins[i], LOW);
    }

    // set the digital pins as output:
    pinMode(ledPin, OUTPUT);
    // pinMode(led1StatePin, OUTPUT);
    pinMode(led2StatePin, OUTPUT);
    previousMillis = millis();

    // // set up rotating LED pins as output and initialize low and sensor pins as input
    // for(int i = 0; i < rotatingArrayLength; i++) {
    //   pinMode(rotatingLedStatePins[i], OUTPUT);
    //   digitalWrite(rotatingLedStatePins[i], LOW);
    //   pinMode(rotatingSensorStatePins[i], INPUT);
    // }

    // // set up input pins
    // // pinMode(sensor1StatePin, INPUT);
    // pinMode(resetButtonStatePin, INPUT);
    // pinMode(degradeStatePin, INPUT);

    // // set everything output to off
    // digitalWrite(ledPin, LOW);
    // // digitalWrite(led1StatePin, LOW);
    // digitalWrite(led2StatePin, LOW);

}

void loop() {
  unsigned long currentMillis = millis();

  unsigned long since = currentMillis - previousMillis;

  if(since > rotatingCycleTime){
    currentLed++;
    if(currentLed > ledPinsLength){
      currentLed = 0;
    }
    previousMillis = currentMillis;
  }

  for(int i = 0; i < ledPinsLength; i++){
    // if(i == currentLed || currentLed == 0){
    //   digitalWrite(fullRotatingLedOutputStatePins[i],HIGH);
    // }
    // else{
    //   digitalWrite(fullRotatingLedOutputStatePins[i],LOW);
    // }
    digitalWrite(fullRotatingLedOutputStatePins[i],ison(i,ledPinsLength,currentLed));
  }

  // // latching circuit
  // // sensor1Status = digitalRead(sensor1StatePin);
  // resetButtonStatus = digitalRead(resetButtonStatePin);
  // // the status is degraded if the input is missing
  // degradeInputStatus = !digitalRead(degradeStatePin);
  // // if(resetButtonStatus){
  // //   led1Status = LOW;
  // // }else if(sensor1Status){
  // //   led1Status = HIGH;
  // // }


  // // rotating sensor test
  // if(resetButtonStatus){
  //   led2Status = LOW;
  // }else if(rotatingArray(rotatingLedStatePins, rotatingSensorStatePins, 
  //          rotatingSensorMonitorStatus, rotatingSensorMonitorMillis, 
  //          degradeInputStatus)) {
  //   // testRotatingSensor()){
  //   led2Status = HIGH;
  // }

  // digitalWrite(led2StatePin, led2Status);
  // // digitalWrite(led1StatePin, led1Status);
  // digitalWrite(ledPin, resetButtonStatus);
  // //digitalWrite(ledPin, led1Status);
  // //digitalWrite(ledPin, led2Status);

}

int ison(int pin, int pinlen, int leadpin){
  if(pin <= leadpin && pin >= leadpin-rotatingNumberOff){
    return LOW;
  }else if(pin < (leadpin + pinlen) && pin >= (leadpin + pinlen - rotatingNumberOff)){
    return LOW;
  }else { return HIGH;}
}

// // Rotating sensor test function
// // Returns HIGH if test fails, LOW if test passes
// // I need to refactor this - I think something is failing, but I hate how it's written
// int testRotatingSensor() {
//   unsigned long currentMillis = millis();
  
//   // get state of currently monitored input and set up other variables - for more, maybe just monitor the "on" pin.
//   // I'd like to have a little better watch here
//   // maybe two arrays, one with the desired state of the LED and one with how long it's been in that state
//   // then we can just and the state with the time > delay to see if we should monitor that pin
//   // mock it up with two, then test. Being able to do this let's us get really fancy on the rotation
//   // The other thing moving these to arrays does is let me control more than one with a single controller, just pass the arrays to this function
//   int onLaser = 0;
//   int offLaser = 0;
//   switch (currentPair) {
//     case 0:
//       rotatingSensorStatus = digitalRead(rotatingSensor1StatePin);
//       onLaser = rotatingLed1StatePin;
//       offLaser = rotatingLed2StatePin;
//       break;
//     case 1:
//       rotatingSensorStatus = digitalRead(rotatingSensor2StatePin);
//       onLaser = rotatingLed2StatePin;
//       offLaser = rotatingLed1StatePin;
//       break;
//   }

//   // determine state of monitoring
//   long sinceUpdateMillis = currentMillis - previousMillis;
//   if(sinceUpdateMillis < rotatingStartDelay){
//     //just make sure the laser is on
//     digitalWrite(onLaser, HIGH);
//     digitalWrite(offLaser, LOW);

//   }else if (sinceUpdateMillis < rotatingCycleTime){
//     // here test the ping
//     return rotatingSensorStatus;
//   }else{
//     // move to the next LED
//     currentPair += 1;
//     // again, a better structure will allow me to set this more effectively too
//     // we could do all kinds of really cool things
//     // we can control the on site order by placement
//     // so we really just need to tell it either how many to have on, or how long after the last one to turn on the next one
//     currentPair = currentPair %2;
//     previousMillis = currentMillis;
//   }

//   // note the only test scenario is when we're in the test "window", which will actually be most of the time.
//   return LOW;
// }

// int rotatingArray(const int rotatingLedStatePins[], const int rotatingSensorStatePins[], bool rotatingSensorMonitorStatus[], unsigned long rotatingSensorMonitorMillis[], bool degradedMode = true) {
//   if(!degradedMode) {
//     previousDegradedMode = degradedMode;
//     //turn each laser on
//     for(int i = 0; i < rotatingArrayLength; i++) {
//       digitalWrite(rotatingLedStatePins[i], HIGH);
//       if(!rotatingSensorMonitorStatus[i]) {
//         //if the monitor is currently off, set it on and reset the timer
//         rotatingSensorMonitorStatus[i] = true;
//         rotatingSensorMonitorMillis[i] = millis();
//       }
    
//       //make sure the delay ms has passed
//       if(millis() - rotatingSensorMonitorMillis[i] > rotatingStartDelay && rotatingSensorMonitorStatus[i]) {
//         //check the input
//         if(digitalRead(rotatingSensorStatePins[i]) == LOW) {
//           //if the input is LOW, return HIGH
//           return HIGH;
//         }
//       }
//     }
//     return LOW;
//   }

//   //so, we want an initial state when degraded mode activates, I want them all to "fritz"
//   //then start rotating 3 being off, and the rest on and monitored
//   if(degradedMode != previousDegradedMode) {
//     //we've changed status
//     for(int i = 0; i < rotatingArrayLength; i++) {
//       digitalWrite(rotatingLedStatePins[i], LOW);
//     }
//     delay(50);
//     for(int i = 0; i < rotatingArrayLength; i++) {
//       digitalWrite(rotatingLedStatePins[i], HIGH);
//     }
//     delay(50);
//     //to-do check each state here to make sure no one is quick running through during the "failure"
//     for(int i = 0; i < rotatingArrayLength; i++) {
//       //turn off each in turn, then turn on
//       digitalWrite(rotatingLedStatePins[i], LOW);
//       delay(50);
//       digitalWrite(rotatingLedStatePins[i], HIGH);
//     }
//   }
//   previousDegradedMode = degradedMode;

//   //I need to figure out the formula for the rotating array
//   //each one will be current millis - start millis % full ct of array (locations * unit) to determine the state of that unit. 
//   //monitor the unit if it's delaytime more
//   // I'm really doing some of this for clarity, but it's not necessary, I could calculate straight off millis, but readability would suffer
//   //unitTime is how long to delay between each lazer state
//   int unitTime = rotatingCycleTime / rotatingNumberOff;
//   int fullCycleTime = rotatingArrayLength * unitTime;
//   unsigned long currentStateTime = millis() % fullCycleTime;
//   for(int i = 0; i < rotatingArrayLength; i++) {
//     //figure out when to turn this pair on and off
//     int onAt = i * unitTime;
//     int offAt = onAt + rotatingCycleTime;
//     //set whether this pair should be on or off and monitored
//     if(currentStateTime > onAt && currentStateTime < offAt) {
//       rotatingSensorMonitorStatus[i] = true;
//       rotatingSensorMonitorMillis[i] = currentStateTime - onAt;
//     }else{
//       rotatingSensorMonitorStatus[i] = false;
//       rotatingSensorMonitorMillis[i] = 0;
//     }

//     // if the lazer is on and we're in the monitoring range, check the input
//     if(rotatingSensorMonitorMillis[i] > rotatingStartDelay) {
//         if(digitalRead(rotatingSensorStatePins[i]) == LOW) {
//             return HIGH;
//         }
//     }
//   }
  
//   return LOW;
// }

// void flashErrorCode(int errorCode) {
//   const int flashOnTime = 500;  // half second on
//   const int flashOffTime = 500;  // half second off
//   const int pauseTime = 2000;    // 2 second pause
  
//   // Flash errorCode times
//   for(int i = 0; i < errorCode; i++) {
//     digitalWrite(ledPin, HIGH);
//     delay(flashOnTime);
//     digitalWrite(ledPin, LOW);
//     delay(flashOffTime);
//   }
  
//   // Pause 2 seconds
//   delay(pauseTime);
  
//   // // Flash errorCode times again
//   // for(int i = 0; i < errorCode; i++) {
//   //   digitalWrite(ledPin, HIGH);
//   //   delay(flashOnTime);
//   //   digitalWrite(ledPin, LOW);
//   //   delay(flashOffTime);
//   // }
// }
