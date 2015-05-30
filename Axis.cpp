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



// Determines Clockwise or Counter-
// Sets Target MCODER VALUE
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

	//negative overflow?
	if (temp0 < 0) temp0 += maxcount;

	//positive overflow?
	if (temp0 >= maxcount) temp0 -= maxcount;

	int temp1 = encoder.getMECount();
	int temp2 = encoder.getCWDistance(temp1, temp0);
	int temp3 = encoder.getCCWDistance(temp1, temp0);
	
	Serial.print(temp1);
	Serial.print(" - ");
	Serial.println(temp0);

	target = temp0;
	clockwise = (temp2 <= temp3);
	motor.setClockwise(clockwise);
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


// CHANGING
bool Axis::processME()
{

	int distance = 0;

	// There's a Delay between 
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
		slewing = false;
		updatePWM(0);
		return false;
	}
	return true;
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

// Sets the 
void Axis::abort()
{
	updatePWM(0);
	delay(100);
	motorSetup(encoder.getMECount());
	return;
}