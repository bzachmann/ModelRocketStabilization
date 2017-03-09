#include <SD.h>
#include <SPI.h>
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

void setup()
{
  Serial.begin(9600);
  startSDCard();
  createFile("OutFile.txt");
  writeToFile("Sample text");
  writeToFile("Sample text");
  writeToFile("Sample text");
  writeToFile("Sample text4");
  writeToFile("Sample text5");
  writeToFile("Sample text6");
  writeToFile("Sample text");
  writeToFile("Sample text");
  closeFile();
}
void loop()
{
  
}

void startSDCard()
{
  Serial.println("Start SD card");
  pinMode(ChipSelect, OUTPUT);
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
}

int createFile(char filename[])
{
  SDfile = SD.open(filename, FILE_WRITE);

  if (SDfile)
  {
    Serial.println("File created successfully.");
    return 1;
  } else
  {
    Serial.println("Error while creating file.");
    return 0;
  }
}

int writeToFile(char text[])
{
  if (SDfile)
  {
    SDfile.println(text);
    Serial.println("Writing: ");
    Serial.println(text);
    return 1;
  } else
  {
    Serial.println("Couldn't write");
    return 0;
  }
}

void closeFile()
{
  if (SDfile)
  {
    SDfile.close();
    Serial.println("File closed");
  }
}

int openFile(char filename[])
{
  SDfile = SD.open(filename);
  if (SDfile)
  {
    Serial.println("File opened");
    return 1;
  } else
  {
    Serial.println("Error opening file");
    return 0;
  }
}

