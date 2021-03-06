#ifndef Axis_H
#define Axis_H

#include "Arduino.h"
#include "Motor.h"
#include "MagneticEncoder.h"

//axis class will have to 
//include motor object and magnetic encoder object
//One day, it will be modified to include quad encoder?

class Axis {

private:
	Motor motor;
	MagneticEncoder encoder;
	int target;
	bool clockwise;

public:
	Axis();
	Axis(Motor myMotor, MagneticEncoder myEncoder);
	~Axis();

	Motor getMotor();
	MagneticEncoder getEncoder();

	void motorSetup(int inputMECount);

	void motorGOCW(int inputMECount);
	void motorGOCCW(int inputMECount);

	bool processME();
};
#endif