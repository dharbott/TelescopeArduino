#ifndef MagneticEncoder_H
#define MagneticEncoder_H

#include "Arduino.h"

class MagneticEncoder {

private:
	int pinSelect;
	int pinClock;
	int pinData;
	int maxCount;

	byte shiftIn();

public:
	MagneticEncoder(int ppinSelect, int ppinClock, int ppData);
	MagneticEncoder(int ppinSelect, int ppinClock, int ppData, int pmaxCount);
	~MagneticEncoder();

	unsigned int readPosition();
};

#endif