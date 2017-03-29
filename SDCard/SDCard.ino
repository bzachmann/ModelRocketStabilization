#include <SD.h>
#include <SPI.h>
#include "avr/interrupt.h"
#include <MsTimer2.h>
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
volatile int writeToSD =1;
  float number1 = 111.123;
  double number2 = 222.213;
  double number3 = 333.321;
  char buffer[30];

void setup()
{
  char buffer[30];
  Serial.begin(9600);
  startSDCard();
  createFile("OutFile.txt");
  writeToFile("1000-test");
  writeNumbersToFile(buffer, number1, number2, number3);  
  closeFile();
  MsTimer2::set(1000, timerHandler); // 500ms period
  MsTimer2::start();
//  for(int i = 0; i < 50; i++)
//  {
//    delay(20);
//    writeNumbersToFile(array, number1, number2, number3);
//  }  
}
void loop()
{
    if(writeToSD == 1)
  {
    createFile("OutFile.txt");  
    writeNumbersToFile(buffer, number1, number2, number3);  
    closeFile();
    writeToSD = 0;
  }
}
 
void timerHandler()
{
    writeToSD = 1;             // Increment volatile variable
}

void startSDCard()
{
  //Serial.println("Start SD card");
  pinMode(ChipSelect, OUTPUT);
  if (SD.begin())
  {
    //Serial.println("SD card is ready to use.");
  } else
  {
    //Serial.println("SD card initialization failed");
    return;
  }
}

int createFile(char filename[])
{
  SDfile = SD.open(filename, FILE_WRITE);

  if (SDfile)
  {
    //Serial.println("File created successfully.");
    return 1;
  } else
  {
    //Serial.println("Error while creating file.");
    return 0;
  }
}

int writeNumbersToFile(char text[], float number1, float number2, float number3)
{
  int  num1 = number1*100;
  int  num2 = number2*100;
  int  num3 = number3*100;
  
  sprintf(text, "%i , %i, %i", num1, num2, num3);
  
  if (SDfile)
  {
    SDfile.println(text);
    //Serial.println("Writing: ");
    //Serial.println(text);
    return 1;
  } else
  {
    //Serial.println("Couldn't write");
    return 0;
  }
}

int writeToFile(char text[])
{
  if (SDfile)
  {
    SDfile.println(text);
    //Serial.println("Writing: ");
    //Serial.println(text);
    return 1;
  } else
  {
    //Serial.println("Couldn't write");
    return 0;
  }
}

void closeFile()
{
  if (SDfile)
  {
    SDfile.close();
    //Serial.println("File closed");
  }
}

int openFile(char filename[])
{
  SDfile = SD.open(filename);
  if (SDfile)
  {
    //Serial.println("File opened");
    return 1;
  } else
  {
    //Serial.println("Error opening file");
    return 0;
  }
}

