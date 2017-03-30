#include "BetterServo.h"
#include <Arduino.h>

BetterServo::BetterServo() : Servo() 
{

}

uint8_t BetterServo::attach(int pin, double min, double max, double offset)
{
  this->minD = min - offset;
  this->maxD = max - offset;
  this->m_offset = offset;

  Servo::attach(pin);
}

uint8_t BetterServo::attach(int pin)
{
  Servo::attach(pin);
}

uint8_t BetterServo::write(double value)
{
  double r_value = value - m_offset;
  
  if(r_value > maxD)
  {
    r_value = maxD;
  }
  else if(r_value < minD)
  {
    r_value = minD;
  }

  long r_value_scaled = r_value * 100; 
  int us_value = map(r_value_scaled, 0, 18000, 0, 2400);

  Servo::writeMicroseconds(us_value);
  
}

