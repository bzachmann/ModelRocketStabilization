#include <Arduino.h>
#include "BetterServo.h"

BetterServo servo = BetterServo();
double val = 0.0;
//test servo addition offset 
#define OFFSET_ALL -90
#define OFFSET_SERVO 15

void setup() {
  pinMode(13, OUTPUT);
  servo.attach(11, -30.0, 30.0, OFFSET_ALL + OFFSET_SERVO); //7.5 offset
  Serial.begin(9600);
}

void loop() {
  
  servo.write(val);
  while(Serial.available())
  {
    char myChar = Serial.read();
    if(myChar == '-')
    {
      val = val - 0.5;
      Serial.println(val);
    }
    else if(myChar == '+')
    {
      val = val + 0.5;
      Serial.println(val);
    }
  }
}
