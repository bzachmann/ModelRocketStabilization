#include <Arduino.h>
#include "BetterServo.h"

BetterServo servo = BetterServo();

void setup() {
  pinMode(13, OUTPUT);
  servo.attach(11, -30.0, 30.0, -90.0);
}

void loop() {
  
  servo.write(-22.5);
  delay(500);
  servo.write(15.3);
  delay(500);

}
