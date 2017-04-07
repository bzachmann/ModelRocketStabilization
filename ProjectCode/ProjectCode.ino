/*
 * TODO:
 * 
 * 2. figure out way to cancel rocket spin using angular velocity about the x axis
 * 3. figure out shaking issue that is caused by imu i2c line inducing noise on PWM signals
 * 4. add sd card writing into code
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <SD.h>
#include <SPI.h>

#include "BetterServo.h"
#include "SDQueue.h"

///////////////defines/////////////////////
#define PIN_SERVO_0       5
#define PIN_SERVO_1       6
#define PIN_SERVO_2       9
#define PIN_SERVO_3       3
#define PIN_BUTTON        4
#define PIN_HALL          8
#define PIN_CHIP_SELECT   10

#define OFFSET_ALL        -90
#define OFFSET_SERVO_0     16
#define OFFSET_SERVO_1    7.5
#define OFFSET_SERVO_2    6.5
#define OFFSET_SERVO_3    4.5

#define MIN_FIN_LIMIT     -50
#define MAX_FIN_LIMIT      50

#define ENABLE_HALL_SHUTDOWN    0

///////////////variables///////////////////

BetterServo servo[4];
sensors_event_t event;
double zeroPoint[2] = {0.0, 0.0};
volatile double deviation[2] = {0.0, 0.0};
volatile uint32_t buttonCount = 0;
volatile bool setZeroPointFlag = false;
volatile uint32_t hallCount = 0;
volatile bool shutdownFlag = false;

volatile SDQueue SDLog;
volatile SDQueue busyQueue;
volatile bool queueBusy = false;
volatile uint8_t timeToQueueCounter = 0;

char filename[15] = "DATA.TXT";
int ChipSelect = PIN_CHIP_SELECT;
char buff[36] = "helloWorld";

///////////////prototypes//////////////////
void setupServos();
void setServosTilt(double y, double z);
void setZeroPoint();
void setupTimer2();
void checkButton();
void checkHall();
void checkSetZeroPoint();
void checkWriteSD();
void checkBusyQueue();
void checkDoShutdown();



//////////////program///////////////////
void setup() {}
void loop()
{
  Adafruit_BNO055 bno = Adafruit_BNO055(55);
  SDLog.init();
  busyQueue.init();
  
  if(!SD.begin(PIN_CHIP_SELECT)) //if the sd card is not inserted, the device will fault out here
  {
    while(1);
  }
  
  if(!bno.begin())
  {
    while(1);
  }

  
  
  delay(1000);
  bno.setExtCrystalUse(true);

  setupTimer2();
  setupServos();

  File SDfile = SD.open(filename, FILE_WRITE);
  if(!SDfile)
  {
    while(1);
  }

  while(1)
  {
    checkDoShutdown();
  
    bno.getEvent(&event);// this causes servo jitter...might be caused by noise induced in pmw line when i2c bus active
                         //this could be fixed by a filter on the hardware line.  as a software fix we 
                         // just might need to limit how often we use the i2c line
                         
    deviation[0] = event.orientation.y - zeroPoint[0];
    deviation[1] = event.orientation.z - zeroPoint[1];
  
    setServosTilt(deviation[0], deviation[1]);
  
    checkSetZeroPoint();
    checkWriteSD();
  }
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
    if(queueBusy)
    {
      if(!busyQueue.enqueue(newLine))
      {
        //TODO - remove. This was in place to test if QUEUE FILLS UP
        for(int i = 0; i < 4; i++)
        {
          servo[i].write(20.0);
        }
      } 
    }
    else
    {
      if(!SDLog.enqueue(newLine))
      {
        //TODO - remove. This was in place to test if QUEUE FILLS UP
        for(int i = 0; i < 4; i++)
        {
          servo[i].write(20.0);
        }
      }
    }
  }
}

void checkWriteSD()
{
  if(!SDLog.isEmpty())
  {
    bool ok;
    queueBusy = true;
    sd_line newLine = SDLog.dequeue(ok);
    queueBusy = false;
    if(ok)
    {
      //TODO write to SD Card here
    }
  }
}

void checkBusyQueue()
{
  if(!queueBusy && !busyQueue.isEmpty())
  {
    while(!busyQueue.isEmpty())
    {
      bool ok;
      if(!SDLog.enqueue(busyQueue.dequeue(ok)))
      {
        for(int i = 0; i < 4; i++)
        {
          servo[i].write(20.0);
        }
      }
    }
  }
}

void checkHall()
{
  if(ENABLE_HALL_SHUTDOWN != 0)
  {
    if(digitalRead(PIN_HALL))
    {
      hallCount++;
      if(hallCount == 500)
      {
        shutdownFlag = true;
        hallCount = 0;
      }
    }
    else
    {
      hallCount = 0;
    }
  }
}

void checkDoShutdown()
{
  if(shutdownFlag)
  {
    //Disable Interrupt
    TIMSK2 = 0;
    
    //Detach all servos
    for(int i = 0; i < 4; i++)
    {
      servo[i].write(0.0);
    }

    //TODO write the rest of the contents in Queues to sd card
    bool ok;
    sd_line newLine;
    while(!SDLog.isEmpty())
    {
      newLine = SDLog.dequeue(ok);
      //TODO write to SD
    }
    while(!busyQueue.isEmpty())
    {
      newLine = busyQueue.dequeue(ok);
      //TODO write to SD
    }

    //TODO close SD card file here

    delay(1000);//wait till servos are straight and then shutdown
    for(int i = 0; i < 4; i++)
    {
      servo[i].detach();
    }
    
    while(1)
    {
      //do nothing until reset;
    }
  }
}

//Timer2 Overflow Interrupt Vector, called every 1ms
ISR(TIMER2_OVF_vect)        // interrupt service routine 
{ 
  TCNT2 = 130;           //Reset Timer to 130 out of 255
  
  checkHall();
  checkBusyQueue();
  checkTimeToQueue();
  checkButton();
    
  TIFR2 = 0x00;          //Timer2 INT Flag Reg: Clear Timer Overflow Flag
};


