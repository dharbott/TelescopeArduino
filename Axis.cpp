#include "Axis.h"

Axis::Axis()
{
	//empty constructors??
	//not the best idea
	motor = Motor();
	encoder = MagneticEncoder();
}

Axis::Axis(Motor myMotor, MagneticEncoder myEncoder)
{
	motor = myMotor;
	encoder = myEncoder;
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
void Axis::motorGO(int inputMECount) {

	int temp1 = encoder.getCWDistance(encoder.getMECount(), inputMECount);
	int temp2 = encoder.getCCWDistance(encoder.getMECount(), inputMECount);

	if (temp1 < temp2)
	{
		motorGOCW(inputMECount);
	}
	else
	{
		motorGOCCW(inputMECount);
	}
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
		distance = encoder.getCWDistance(encoder.getMECount(), inputMECount);
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
