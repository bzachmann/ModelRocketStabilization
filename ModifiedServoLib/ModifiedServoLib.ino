#include <Arduino.h>
#include "BetterServo.h"

BetterServo servo = BetterServo();

void setup() {
  pinMode(13, OUTPUT);
}

void loop() {
  servo.test();
  delay(200);
  servo.test();
  delay(200);

}
