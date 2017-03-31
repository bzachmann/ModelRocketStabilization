/*
 * TODO:
 * 
 * 1. check hall effect sensor, and if not present trigger shutdown
 * 2. figure out way to cancel rocket spin using angular velocity about the x axis
 * 3. figure out shaking issue that is caused if removing the delay() in the main loop
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BetterServo.h"
#include "SDQueue.h"

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
volatile double deviation[2] = {0.0, 0.0};
volatile uint32_t buttonCount = 0;
volatile bool setZeroPointFlag = false;

volatile SDQueue SDLog = SDQueue();
volatile uint8_t timeToQueueCounter = 0;

///////////////prototypes//////////////////
void setupServos();
void setServosTilt(double y, double z);
void setZeroPoint();
void setupTimer2();
void checkButton();
void checkSetZeroPoint();
void checkWriteSD();


//////////////program///////////////////
void setup() {
  //TODO REMOVE THIS LINE WHEN SETTING UP SD CARD
  pinMode(13, OUTPUT);
  
  setupTimer2();
  setupServos();

  if(!bno.begin())
  {
    while(1);
  }
  delay(200);
  bno.setExtCrystalUse(true);
}

void loop()
{
  bno.getEvent(&event);
  deviation[0] = event.orientation.y - zeroPoint[0];
  deviation[1] = event.orientation.z - zeroPoint[1];
  
  setServosTilt(deviation[0], deviation[1]);
  checkSetZeroPoint();
  checkWriteSD();
  delay(5);//without this delay the servos jitter.  I'm guessing that without this delay, the servo values being changed
            // too often and it is messing up the PWM in the middle of the cycle.  I also tried a interrup driven "wait to update"
            // but that also caused shaking and also the update time was terrible.  Like 500 ms instead of 5ms.  Not sure if the
            //timer interrupt is being handled as often as we think due to the other interrupts on the system.  This would be
            //something to look into.
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

void setupTimer2()
{
  TCCR2B = 0x00;        //Disbale Timer2 while we set it up
  TCNT2  = 130;         //Reset Timer Count to 130 out of 255(1 ms)
  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
  TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 0x05;        //Timer2 Control Reg B: Timer Prescaler set to 128
}

void checkButton()
{
  if(digitalRead(PIN_BUTTON))
  {
    buttonCount++;
    if(buttonCount == 1500)
    {
      setZeroPointFlag = true;
      buttonCount = 0;
    }
  }
  else
  {
    buttonCount = 0;
  }
}

void checkSetZeroPoint()
{
  if(setZeroPointFlag)
  {
    setZeroPoint();
    setZeroPointFlag = false;
  }
}

void checkTimeToQueue()
{
  timeToQueueCounter++;
  if(timeToQueueCounter == 10)
  {
    timeToQueueCounter = 0;
    sd_line newLine = {0.0 , deviation[0], deviation [1]};
    SDLog.enqueue(newLine);
  }
}

void checkWriteSD()
{
  if(!SDLog.isEmpty())
  {
    bool ok;
    sd_line newLine = SDLog.dequeue(ok);
    if(ok)
    {
      digitalWrite(13, !digitalRead(13));
    }
  }
}

//Timer2 Overflow Interrupt Vector, called every 1ms
ISR(TIMER2_OVF_vect)        // interrupt service routine 
{ 
  TCNT2 = 130;           //Reset Timer to 130 out of 255
  
  //we want to do a couple things with the interrupt
  //2. every 10 interrupts, log the deviation values to a queue that will be then sent to the sd card whenever possible
  //3. check if the magnet is no longer present after a debounce, similar to checkButton().  If not, trigger shutdown flag.
  checkTimeToQueue();
  checkButton();
    
  TIFR2 = 0x00;          //Timer2 INT Flag Reg: Clear Timer Overflow Flag
};


