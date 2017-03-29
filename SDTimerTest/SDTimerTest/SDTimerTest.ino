#include <SD.h>
#include <SPI.h>
#include "avr/interrupt.h"
/*
 * Advanced Embedded
 * Matt Ruhland, Boston Zachman, Andrew Krecklau
 * PIN placement set from arduino SPI library
 * MISO:PIN 12 --Master In Slave Out
 * MOSI:PIN 11 --Master Out Slave In
 * SCk :PIN 13 --Clock
 */
  int ChipSelect = 10;//Chip Select
  File SDfile;// used for storing the file object
  char buffer[36];
  unsigned int count = 0;   //used to keep count of how many interrupts were fired
  uint32_t totalCount = 0;   //used to keep count of how many interrupts were fired

void setup()
{
  Serial.begin(9600);
  startSDCard();
  createFile("OutFile.txt");
  writeToFile("test123");
  closeFile();

  TCCR2B = 0x00;        //Disbale Timer2 while we set it up
  TCNT2  = 130;         //Reset Timer Count to 130 out of 255(1 ms)
  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
  TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 0x05;        //Timer2 Control Reg B: Timer Prescaler set to 128
}

//Timer2 Overflow Interrupt Vector, called every 1ms
ISR(TIMER2_OVF_vect)        // interrupt service routine 
{ 
  TCNT2 = 130;           //Reset Timer to 130 out of 255
  TIFR2 = 0x00;          //Timer2 INT Flag Reg: Clear Timer Overflow Flag
  totalCount++;  
};

void loop()
{
    startSDCard();
    uint32_t value =0;
        
    uint32_t aStart = totalCount;
    delay(500);
    uint32_t aEnd = totalCount;
    value = aEnd-aStart;
    
    createFile("OutFile.txt");
    sprintf(buffer, "difference: %i", value);
    writeToFile(buffer);  
    closeFile();
    while(true)
    {
      
    }      

}
 

void startSDCard()
{
  pinMode(ChipSelect, OUTPUT);
  if (SD.begin())
  {
  } else
  {
    return;
  }
}

int createFile(char filename[])
{
  SDfile = SD.open(filename, FILE_WRITE);

  if (SDfile)
  {
    return 1;
  } else
  {
    return 0;
  }
}

int writeNumbersToFile(char text[], float number1, float number2, float number3)
{
  int  num1 = number1;
  int  num1Dec = (number1-num1) * 1000;
  int  num2 = number2;
  int  num2Dec = (number2-num2) * 1000;
  int  num3 = number3;
  int  num3Dec = (number3-num3) * 1000;
  
  sprintf(text, "%i.%i , %i.%i, %i.%i", num1, num1Dec, num2, num2Dec, num3, num3Dec);
  
  if (SDfile)
  {
    SDfile.println(text);
    return 1;
  } else
  {
    return 0;
  }
}

int writeToFile(char text[])
{
  if (SDfile)
  {
    SDfile.println(text);
    return 1;
  } else
  {
    return 0;
  }
}

void closeFile()
{
  if (SDfile)
  {
    SDfile.close();
  }
}


