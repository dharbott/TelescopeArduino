#include "Axis.h"

Axis::Axis()
{
	//empty constructors??
	//not the best idea
	motor = Motor();
	encoder = MagneticEncoder();
	target = 0;
	countOffset = 0;
	clockwise = true;
	currentPWM = 0;
	slewing = false;
}


Axis::Axis(Motor myMotor, MagneticEncoder myEncoder)
{
	motor = myMotor;
	encoder = myEncoder;
	target = 0;
	countOffset = 0;
	clockwise = true;
	currentPWM = 0;
	slewing = false;
}


Axis::~Axis()
{
	motor.~Motor();
	encoder.~MagneticEncoder();
}


Motor Axis::getMotor()
{
	return motor;
}


MagneticEncoder Axis::getEncoder()
{
	return encoder;
}



// Sets Target MCODER VALUE
// includes offset value
void Axis::motorSetup(int input)
{
	int maxcount = encoder.getMaxCount();

	if ((input < 0) || (input >= maxcount))
	{
		return; //unhandled error state
	}
	
	//translate from user coordinates
	//to machine coordinates
	int temp0 = input - countOffset;

	//negative overflow? OR positive overflow?
	if (temp0 < 0) temp0 += maxcount;
	if (temp0 >= maxcount) temp0 -= maxcount;

	target = temp0;
	slewing = true;
}


// CHANGING
void Axis::processME()
{
	int distance = 0;
	int maxCount = encoder.getMaxCount();

	distance = encoder.getCWDistance(encoder.getMECount(), target);

	// if the clockwise distance is greater than half
	// the degrees (i.e. Encoder Count) then we don't
	// want to traverse clockwise, but counterclockwise
	// so set clockwise to false, and make distance
	// the complement
	if (distance >= (maxCount / 2))
	{
		distance = 4095 - distance;
		motor.setClockwise(false);
	}
	else
	{
		motor.setClockwise(true);
	}

	if (distance >= (maxCount / 36))
	{
		updatePWM(255);
	}
	else if (distance >= 1)
	{
		//value jumps from 255 to 100,
		//then jumps to 80,
		//then jumps to 60, 40, 20
		updatePWM((distance / 30) * 30 + 30);
	}
	else
	{
		slewing = false;
		updatePWM(0);
	}
}


int Axis::getPWM()
{
	return currentPWM;
}


bool Axis::getSlewing()
{
	return slewing;
}


void Axis::updatePWM(int intPWM)
{
	if (intPWM != currentPWM)
	{
		currentPWM = intPWM;
		motor.setPWM(currentPWM);
	}
}

// Sets the pwm to 0, target to match current
void Axis::abort()
{
	slewing = false;
	updatePWM(0);

	// Delay may be necessary as motor
	// stops and settles on a position
	delay(50);
	target = encoder.getMECount();
	return;
}


int Axis::getUserSyncCount()
{
	int maxcount = encoder.getMaxCount();

	//if my current is 180d, userSync is 270d
	//userSyncCount equals  sync = current + offset
	int temp = encoder.getMECount() + countOffset;

	//negative overflow?
	if (temp < 0) temp += maxcount;

	//positive overflow?
	if (temp >= maxcount) temp -= maxcount;

	return temp;
}


void Axis::setUserSyncCount(int input)
{
	if ((input < 0) || (input > encoder.getMaxCount()))
	{
		return; //unhandled error state
	}

	//if my current is 180d, set to 270d
	//offset equals  (new - current) = 90d
	countOffset = input - encoder.getMECount();

}
