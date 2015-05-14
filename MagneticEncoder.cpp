#include "MagneticEncoder.h"

MagneticEncoder::MagneticEncoder()
{
	pinSelect = -1;
	pinClock = -1;
	pinData = -1;
	maxCount = 4096;
	offset = -1;
}

MagneticEncoder::MagneticEncoder(int ppinSelect, int ppinClock, int ppinData)
{
	pinSelect = ppinSelect;
	pinClock = ppinClock;
	pinData = ppinData;
	maxCount = 4096;
	offset = 0;
}

MagneticEncoder::MagneticEncoder(int ppinSelect, int ppinClock, int ppinData, int pmaxCount)
{
	pinSelect = ppinSelect;
	pinClock = ppinClock;
	pinData = ppinData;
	maxCount = pmaxCount;
	offset = 0;
}

MagneticEncoder::~MagneticEncoder() {
}

//read the current angular position, binary, 16 bits
unsigned int MagneticEncoder::readPosition()
{
	unsigned int posbyte = 0;

	//There is a function in the Arduino Lib that does 
	//the following bit manipulation

	//shift in our data  
	digitalWrite(pinSelect, LOW);
	delayMicroseconds(1);
	byte d1 = shiftIn(); //read in 8 bits
	byte d2 = shiftIn(); //read in 8 bits
	digitalWrite(pinSelect, HIGH);

	//Bit Shifting, 16 bit unsigned int
	posbyte = d1;
	posbyte = posbyte << 8;
	posbyte |= d2;

	//the encoder only gives 12 bits for position
	//shift back 4 bits
	posbyte = posbyte >> 4;

	return posbyte;
}


//read in a byte (8bits) of data from the digital input of the board.
byte MagneticEncoder::shiftIn()
{
	byte data = 0;

	for (int i = 7; i >= 0; i--)
	{
		digitalWrite(pinClock, LOW);
		delayMicroseconds(1);
		digitalWrite(pinClock, HIGH);
		delayMicroseconds(1);

		//read byte when clock signal goes from LOW to HIGH
		byte dbit = digitalRead(pinData);

		data |= (dbit << i);
	}

	return data;
}


//WRAPPER, not necessary??
int MagneticEncoder::getMECount() {
	return readPosition();
}


// assume valid input
// input is the 12-bit mag encoder's output (0-4095)
// -I think using a long, multiplying by an int, then
// dividing by a float will yield the best accuracy
// but then again this is just a guess
float MagneticEncoder::countToAngleFloat(int input){
	long temp = input;
	return ((temp * 45) / 512.0);
	//return ((input / 4096.0) * 360.0);
}

// I think using a long, multiplying by an int, then
// dividing by a float will yield the best accuracy
// but then again this is just a guess
int MagneticEncoder::countToMinutes(int input)
{
	long temp = input;
	return ((temp * 675) / 128.0);
	//return (input / 4096.0) * 21600.0;
}

int MagneticEncoder::mintesToCount(int input)
{
	long temp = input;
	return ((temp * 128) / 675.0);
	//return ((input / 21600.0) * 4096.0);
}

int MagneticEncoder::angleFloatToCount(float input)
{
	long temp = ((input * 512) / 45.0);
	return temp;
}


int MagneticEncoder::getCWDistance(int current, int target) {
	return (target - current + ((target < current) * maxCount));
}

//MERGE SIMILAR FUNCTIONS
int MagneticEncoder::getCCWDistance(int current, int target) {
	return (current - target + ((target > current) * maxCount));
}


//WE"RE NOT TOUCHING OFFSETS YET
//TODO LATER
void MagneticEncoder::setOffset(int input) {
	offset = input;
}

int MagneticEncoder::getOffset() {
	return offset;
}