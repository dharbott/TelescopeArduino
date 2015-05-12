#include "MagneticEncoder.h"

MagneticEncoder::MagneticEncoder(int ppinSelect, int ppinClock, int ppinData)
{
	pinSelect = ppinSelect;
	pinClock = ppinClock;
	pinData = ppinData;
	maxCount = 4096;
}

MagneticEncoder::MagneticEncoder(int ppinSelect, int ppinClock, int ppinData, int pmaxCount)
{
	pinSelect = ppinSelect;
	pinClock = ppinClock;
	pinData = ppinData;
	maxCount = pmaxCount;
}

MagneticEncoder::~MagneticEncoder() {
}

//read the current angular position, binary, 16 bits
//CURRENTLY HARDCODED TO PINS 11,12,13 for MAGNETIC ENCODER AZM
//EVENTUALLY NEED A 2ND ReadPosition for MAGNETIC ENCODER ALT
unsigned int MagneticEncoder::readPosition()
{
	unsigned int posbyte = 0;

	//There is a function in the Arduino Lib that does the following


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

