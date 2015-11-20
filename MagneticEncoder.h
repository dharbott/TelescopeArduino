#ifndef MagneticEncoder_H
#define MagneticEncoder_H

#include "Arduino.h"

class MagneticEncoder {

private:
	int pinSelect;
	int pinClock;
	int pinData;
	int maxCount;
	int offset;

	byte shiftIn();

public:
	MagneticEncoder();
	MagneticEncoder(int ppinSelect, int ppinClock, int ppData);
	MagneticEncoder(int ppinSelect, int ppinClock, int ppData, int pmaxCount);
	~MagneticEncoder();

	unsigned int readPosition();
	//WRAPPER, not necessary??
	int getMECount();
	int getMaxCount();
//        int getMERate();
//        int getMERateArcminutes();
//        float getMERateAngleFloat();

	void setOffset(int offset);
	int getOffset();

	float countToAngleFloat(int count);
	int countToMinutes(int count);
	int minutesToCount(int minutes);
	int angleFloatToCount(float angle);

	int getCWDistance(int current, int target);
	int getCCWDistance(int current, int target);
};

#endif
