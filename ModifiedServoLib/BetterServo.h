#ifndef BETTERSERVO_H_
#define BETTERSERVO_H_

#include <Servo.h>

class BetterServo: public Servo 
{
public:
  BetterServo();

	uint8_t attach(int pin, double min, double max, double offset);
  uint8_t attach(int pin);

  uint8_t write(double value);

private:
  double minD;
  double maxD;
  double m_offset;
};

#endif /* BETTERSERVO_H_ */
