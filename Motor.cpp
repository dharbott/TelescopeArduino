#include "Motor.h"

Motor::Motor()
{
	pinPWM = -1;
	pinInputA = -1;
	pinInputB = -1;
	//setPWM(0);
	setClockwise(true);
	currentPWM = -1;
}

Motor::Motor(int ppinPWM, int ppinInputA, int ppinInputB)
{
		pinPWM = ppinPWM;
		pinInputA = ppinInputA;
		pinInputB = ppinInputB;
		setPWM(0);
		setClockwise(true);
		currentPWM = 0;
}

Motor::~Motor()
{
		setPWM(0);
}

//change it to STRING LATER
long Motor::pinOut()
{
	long temp = pinPWM * 10000L + pinInputA * 100L + pinInputB;
	return temp;
}


void Motor::rawPWM(int intPWM)
{
	analogWrite(pinPWM, intPWM);
}


//regulated PWM input
void Motor::setPWM(int intPWM)
{
	if ((intPWM < 256) && (intPWM > 0)) 
	{
		currentPWM = intPWM;
		analogWrite(pinPWM, intPWM);
	}
	else if (intPWM == 0)
	{
		currentPWM = 0;
		digitalWrite(pinInputA, HIGH);
		digitalWrite(pinInputB, HIGH);
		analogWrite(pinPWM, 0);
	}
}


int Motor::getPWM()
{
	return currentPWM;
}


//sets motor direction using booleans
void Motor::setClockwise(bool cclockwise)
{
	clockwise = cclockwise;
	digitalWrite(pinInputA, clockwise);
	digitalWrite(pinInputB, !clockwise);
}


//returns the state of member variable
bool Motor::isClockwise()
{
	return clockwise;
}


//regulated PWM input
//accepts a number between 0 to 255 for clockwise rotation
//also accepts number between (-255)to 0 for counterclockwise
//******we'll need faster more specialized versions of this******
void Motor::motorGo(int intPWM)
{
	setClockwise(intPWM >= 0);
	setPWM(abs(intPWM));
}