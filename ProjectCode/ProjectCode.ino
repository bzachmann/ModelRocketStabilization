#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BetterServo.h"

//test servo addition offset 
#define PIN_SERVO_0       5
#define PIN_SERVO_1       6
#define PIN_SERVO_2       9
#define PIN_SERVO_3       3
#define PIN_BUTTON        4

#define OFFSET_ALL        -90
#define OFFSET_SERVO_0     16
#define OFFSET_SERVO_1    7.5
#define OFFSET_SERVO_2    6.5
#define OFFSET_SERVO_3    4.5

Adafruit_BNO055 bno = Adafruit_BNO055(55);

BetterServo servo = BetterServo();

double val = 0;

void setup() {
  servo.attach(PIN_SERVO_1, -50.0, 50.0, OFFSET_ALL + OFFSET_SERVO_1);

  if(!bno.begin())
  {
    while(1);
  }

  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop() {

  servo.write(val);

  if(digitalRead(PIN_BUTTON))
  {
    delay(100);
    val -= 0.5;
    while(digitalRead(PIN_BUTTON)){}
  }

  
//  sensors_event_t event; 
//  bno.getEvent(&event);
//  servo.write(event.orientation.z);
//  delay(10);

}
