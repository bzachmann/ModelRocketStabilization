#include "BetterServo.h"
#include <Arduino.h>

BetterServo::BetterServo() : Servo() 
{

}

void BetterServo::test()
{
	digitalWrite(13, !digitalRead(13));
}

