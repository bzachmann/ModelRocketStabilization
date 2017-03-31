#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BetterServo.h"

//test servo addition offset 
#define OFFSET_ALL -90
#define OFFSET_SERVO 15

Adafruit_BNO055 bno = Adafruit_BNO055(55);

BetterServo servo = BetterServo();
double val = 0.0;

void setup() {
  pinMode(13, OUTPUT);
  servo.attach(11, -50.0, 50.0, OFFSET_ALL + OFFSET_SERVO); //7.5 offset
  Serial.begin(9600);

  if(!bno.begin())
  {
    Serial.print("bno not connected");
    while(1);
  }

  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop() {
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  servo.write(event.orientation.z);
  
  delay(10);
}
