#include <Stepper.h>

#define TOUCH_PIN 4
//pin that sends 5v
#define TOUCH_5V_PIN 2
#define TOUCH_COOLDOWN 40

struct CooldownState{
  bool isRunning;
  int cooldownCounter;
  unsigned long timer;
  int motorDirection;
  
  void tickCooldown(){
    if(cooldownCounter > -1){
      cooldownCounter--;

      Serial.print("COOLDOWN: ");
      Serial.println(cooldownCounter);
    }
  }

  void handleButtonPress(){
    int buttonPressed = digitalRead(TOUCH_PIN);
    if(buttonPressed && cooldownCounter <= -1){
      isRunning = !isRunning;
      cooldownCounter = TOUCH_COOLDOWN;

      Serial.print("Button pressed, toggling isRunning to ");
      Serial.println(isRunning);
    }
  }

  void switchMotorDirection(){
    if(millis() - timer >= 5000){
      timer = millis();
      motorDirection *= -1;
    }
  }
};

CooldownState runState = {false, -1, 0L, 1};
const int stepsPerRevolution = 2000; // change this to fit the number of steps per revolution
Stepper myStepper(stepsPerRevolution, 8,10, 9, 11);

void setup() {

  myStepper.setSpeed(15); // set the speed in RP<
  pinMode(TOUCH_PIN, INPUT);

  //sends 5v through pin
  //please don't unplug
  pinMode(TOUCH_5V_PIN, OUTPUT);
  digitalWrite(TOUCH_5V_PIN, HIGH);
  Serial.begin(9600);
}

void loop()
{
  runState.tickCooldown();
  runState.handleButtonPress();

  if(!runState.isRunning) return;

  runState.switchMotorDirection();

  //from here run the motor

  myStepper.step(runState.motorDirection * stepsPerRevolution/100); // step one revolution
  //Serial.println("finished step");

  delay(1);
}