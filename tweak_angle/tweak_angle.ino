#include <Servo.h>

Servo myservo;  // create servo object to control a servo
int servoPin = 16; // Change this to the pin your servo is connected to
int pos = -1;    // variable to store the servo position, start with an invalid value

void setup() {
  myservo.attach(servoPin);  // attaches the servo on a pin to the servo object
  Serial.begin(115200);      // initialize serial communication at 115200 bits per second
  Serial.println("Enter servo position (0 to 180):");
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming bytes:
    int incomingValue = Serial.parseInt();

    // Check if data was actually received
    if (Serial.read() != -1) {
      if (incomingValue >= 0 && incomingValue <= 180 && incomingValue != pos) {
        pos = incomingValue;
        myservo.write(pos); // tell servo to go to position in variable 'pos'
        Serial.print("Moving servo to: ");
        Serial.println(pos);
      } else if (incomingValue != pos) {
        Serial.println("Invalid input. Please enter a value between 0 and 180.");
      }
    }
  }
}