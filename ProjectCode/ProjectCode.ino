#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BetterServo.h"

///////////////defines/////////////////////
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

#define MIN_FIN_LIMIT     -50
#define MAX_FIN_LIMIT      50

///////////////variables///////////////////
Adafruit_BNO055 bno = Adafruit_BNO055(55);
BetterServo servo[4];
sensors_event_t event;
double zeroPoint[2] = {0.0, 0.0};
double deviation[2] = {0.0, 0.0};
uint32_t buttonCount = 0;
bool zeroPointSetFlag = false;

///////////////prototypes//////////////////
void setupServos();
void setServosTilt(double y, double z);
void setZeroPoint();


//////////////program///////////////////
void setup() {
  
  setupServos();

  if(!bno.begin())
  {
    while(1);
  }

  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop()
{
  bno.getEvent(&event);
  deviation[0] = event.orientation.y - zeroPoint[0];
  deviation[1] = event.orientation.z - zeroPoint[1];
  setServosTilt(deviation[0], deviation[1]);
  

  if(digitalRead(PIN_BUTTON))
  {
    buttonCount++;
    if(buttonCount == 150) //3 sec b/c of 10ms delay at end of loop.. change this when timer interrupt implemented
    {
      setZeroPoint();
      buttonCount = 0;
    }
  }
  else
  {
    buttonCount = 0;
  }
  
  delay(10);

}

void setupServos()
{
  servo[0] = BetterServo();
  servo[1] = BetterServo();
  servo[2] = BetterServo();
  servo[3] = BetterServo();
   
  servo[0].attach(PIN_SERVO_0, MIN_FIN_LIMIT, MAX_FIN_LIMIT, OFFSET_ALL + OFFSET_SERVO_0);
  servo[1].attach(PIN_SERVO_1, MIN_FIN_LIMIT, MAX_FIN_LIMIT, OFFSET_ALL + OFFSET_SERVO_1);
  servo[2].attach(PIN_SERVO_2, MIN_FIN_LIMIT, MAX_FIN_LIMIT, OFFSET_ALL + OFFSET_SERVO_2);
  servo[3].attach(PIN_SERVO_3, MIN_FIN_LIMIT, MAX_FIN_LIMIT, OFFSET_ALL + OFFSET_SERVO_3);

  for(int i = 0; i < 4; i++)
  {
    servo[i].write(0.0);
  }
}

void setServosTilt(double y, double z)
{
  servo[1].write(y);
  servo[3].write(-y);
  
  servo[0].write(z);
  servo[2].write(-z);
}

void setZeroPoint()
{
  zeroPoint[0] = event.orientation.y;
  zeroPoint[1] = event.orientation.z;
}


