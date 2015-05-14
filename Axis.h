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
	String name;

public:
	//THE BIG QUESTION IS: Does it pass classes into
	//constructor by reference? or do I have to use
	//pointers?
	Axis();
	Axis(Motor myMotor, MagneticEncoder myEncoder);
	~Axis();

	Motor getMotor();
	MagneticEncoder getEncoder();

	void motorGO(int inputMECount);
	void motorGOCW(int inputMECount);
	void motorGOCCW(int inputMECount);
};
#endif