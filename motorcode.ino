// ESP32 28BYJ-48 Stepper Motor with AccelStepper Library
// Install AccelStepper library: Tools -> Manage Libraries -> Search "AccelStepper"
//
// Wiring (ULN2003 to ESP32):
// IN1 -> GPIO 19
// IN2 -> GPIO 18  
// IN3 -> GPIO 5
// IN4 -> GPIO 17
// VCC -> 5V (external power recommended)
// GND -> GND

#include <AccelStepper.h>

// Define pin connections
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

// Create stepper object
// AccelStepper::HALF4WIRE = 8 (half-step, 4-wire)
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);
  
  // Set maximum speed (steps per second)
  stepper.setMaxSpeed(1000);
  
  // Set acceleration (steps per second per second)
  stepper.setAcceleration(500);
  
  // Set initial speed (steps per second)
  stepper.setSpeed(300);
  
  Serial.println("28BYJ-48 Stepper Motor - AccelStepper Library");
  Serial.println("Motor will start spinning continuously...");
}

void loop() {
  // Method 1: Continuous rotation using runSpeed()
  stepper.runSpeed();
  
  // Optional: Change direction every 2048 steps (1 full rotation)
  // Uncomment the lines below if you want it to reverse periodically
  /*
  static unsigned long lastChange = 0;
  static int direction = 1;
  
  if (millis() - lastChange > 3000) { // Change every 3 seconds
    direction *= -1;
    stepper.setSpeed(200 * direction);
    lastChange = millis();
    Serial.println(direction > 0 ? "Clockwise" : "Counter-clockwise");
  }
  */
}

// Alternative continuous rotation methods:

/*
// Method 2: Using moveTo() with large target
void loop() {
  if (stepper.distanceToGo() == 0) {
    // Move a large number of steps
    stepper.moveTo(stepper.currentPosition() + 10000);
  }
  stepper.run();
}
*/

/*
// Method 3: Precise speed control with acceleration
void loop() {
  static unsigned long lastTime = 0;
  
  if (millis() - lastTime > 50) { // Update every 50ms
    stepper.move(10); // Move 10 steps
    lastTime = millis();
  }
  stepper.run();
}
*/
