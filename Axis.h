#ifndef Axis_H
#define Axis_H

#include "Arduino.h"

class Axis {

private:
	int pinSelect;


public:
	Axis(int ppinSelect, int ppinClock, int ppData);
	Axis(int ppinSelect, int ppinClock, int ppData, int pmaxCount);
	~Axis();



#endif