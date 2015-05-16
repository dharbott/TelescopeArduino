#include "Axis.h"

Axis::Axis()
{
	//empty constructors??
	//not the best idea
	motor = Motor();
	encoder = MagneticEncoder();
	target = 0;
}

Axis::Axis(Motor myMotor, MagneticEncoder myEncoder)
{
	motor = myMotor;
	encoder = myEncoder;
	target = 0;
}

Axis::~Axis()
{
}


Motor Axis::getMotor()
{
	return motor;
}

MagneticEncoder Axis::getEncoder()
{
	return encoder;
}


//Is there a better design?
void Axis::motorSetup(int inputMECount) {

	int temp0 = abs(inputMECount);

	target = temp0;

	int temp1 = encoder.getCWDistance(encoder.getMECount(), temp0);
	int temp2 = encoder.getCCWDistance(encoder.getMECount(), temp0);

	motor.setClockwise(temp1 <= temp2);
}


bool Axis::processME() {
	int temppwm = 0;
	int distance = 0;

	if (distance >= 100) {
		temppwm = -255;
	}
	else if (distance >= 40) {
		temppwm = -100;
	}
	else if (distance >= 1) {
		temppwm = -20;
	}
	else {
	}

	motor.setPWM(temppwm);
	
	return true;
}
void Axis::motorGOCW(int inputMECount) {
	
	int distance;
	int tempspeed;

	while (true) {
		distance = encoder.getCWDistance(encoder.getMECount(), inputMECount);
		if (distance >= 100)
			tempspeed = 255;
		else if (distance >= 40)
			tempspeed = 100;
		else if (distance >= 1)
			tempspeed = 20;
		else
			break;
		motor.motorGo(tempspeed);
		delay(3);
	}
	motor.motorGo(0);
}

//SAVE MEMORY - MERGE SIMILAR FUNCTIONS
void Axis::motorGOCCW(int inputMECount) {
	
	int distance;
	int tempspeed;

	while (true) {
		distance = encoder.getCCWDistance(encoder.getMECount(), inputMECount);
		if (distance >= 100)
			tempspeed = -255;
		else if (distance >= 40)
			tempspeed = -100;
		else if (distance >= 1)
			tempspeed = -20;
		else
			break;
		motor.motorGo(tempspeed);
		delay(3);
	}
	motor.motorGo(0);
}
