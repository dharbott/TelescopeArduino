#include "Axis.h"

Axis::Axis()
{
	//empty constructors??
	//not the best idea
	motor = Motor();
	encoder = MagneticEncoder();
	target = 0;
	currentPWM = 0;
}

Axis::Axis(Motor myMotor, MagneticEncoder myEncoder)
{
	motor = myMotor;
	encoder = myEncoder;
	target = 0;
	currentPWM = 0;
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

int Axis::getPWM()
{
	return currentPWM;
}

//Is there a better design?
//determine if it's shorter to go
//clockwise or counterclockwise
//then sets the motor direction
//-executed only once per move command
void Axis::motorSetup(int inputMECount)
{

	int temp0 = abs(inputMECount);
	int temp1 = encoder.getMECount();

	int temp2 = encoder.getCWDistance(temp1, temp0);
	int temp3 = encoder.getCCWDistance(temp1, temp0);
	
	target = temp0;
	clockwise = (temp2 <= temp3);
	motor.setClockwise(clockwise);
}


//looking for better ways to do this
bool Axis::processME()
{

	int distance = 0;
	int current = encoder.getMECount();
	
	if (clockwise)
		distance = encoder.getCWDistance(current, target);
	else
		distance = encoder.getCCWDistance(current, target);

	if (distance >= 100)
	{
		updatePWM(255);
	}
	//small exception where this can fail
	else if (distance >= 1)
	{
		//value jumps to 100,
		//then jumps to 80,
		//then jumps to 60, 40, 20
		updatePWM((distance / 20) * 20 + 20);
	}
	else
	{
		updatePWM(0);
		return false;
	}
	return true;
}


void Axis::updatePWM(int intPWM)
{
	if (intPWM != currentPWM)
	{
		currentPWM = intPWM;
		motor.setPWM(currentPWM);
	}
}