#ifndef SDQUEUE_H_
#define SDQUEUE_H_

#include <stdint.h>

#define QUEUELENGTH 10

typedef struct triple_double{
  double xSpin;
  double yDev;
  double zDev;
}sd_line;

class SDQueue {
public:
	SDQueue();
  
  void init();
	bool enqueue(sd_line data);
	sd_line dequeue(bool &ok);

	bool isEmpty();

private:
	sd_line buff[QUEUELENGTH];
	int8_t iFront;
	int8_t iBack;

};

#endif /* SDQUEUE_H_ */
