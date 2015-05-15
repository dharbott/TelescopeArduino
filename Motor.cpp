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

Motor::Motor(int ppinPWM, int ppinInputA, int ppinInputB){
		pinPWM = ppinPWM;
		pinInputA = ppinInputA;
		pinInputB = ppinInputB;
		setPWM(0);
		setClockwise(true);
		currentPWM = 0;
}

Motor::~Motor(){
		setPWM(0);
}

//change it to STRING LATER
long Motor::pinOut(){
	long temp = pinPWM * 10000L + pinInputA * 100L + pinInputB;
	return temp;
}

//unregulated PWM input
void Motor::setPWM(int intPWM){
	analogWrite(pinPWM, intPWM);
}


//set direction
//set initial speed
//all using the pwm number
void Motor::motorSetup(int intPWM){
	if ((intPWM < 256) && (intPWM >= 0)) {
		setClockwise(true);
		currentPWM = intPWM;
	}
	else if ((intPWM > -256) && (intPWM <= 0)) {
		setClockwise(false);
		currentPWM = -intPWM;
	}
	else if (intPWM == 0){
		digitalWrite(pinInputA, HIGH);
		digitalWrite(pinInputB, HIGH);
		currentPWM = intPWM;
	}
}


//start motor
void Motor::motorStart(){
	analogWrite(pinPWM, currentPWM);
}


//stop motor
//set/check flag

//change speed
void Motor::changePWM(int intPWM){

}

//not sure how to brake a dc motor
void Motor::brake(){
	digitalWrite(pinInputA, HIGH);
	digitalWrite(pinInputB, HIGH);
	setPWM(0);
}

//sets motor direction using booleans
void Motor::setClockwise(bool cclockwise){
	clockwise = cclockwise;
	digitalWrite(pinInputA, clockwise);
	digitalWrite(pinInputB, !clockwise);
}

//returns the state of member variable
bool Motor::isClockwise(){
	return clockwise;
}

//============================================

//regulated PWM input
//accepts a number between 0 to 255 for clockwise rotation
//also accepts number between (-255)to 0 for counterclockwise
//******we'll need faster more specialized versions of this******
void Motor::motorGo(int intPWM){
	if ((intPWM < 256) && (intPWM >= 0)) {
		setClockwise(true);
		setPWM(intPWM);
	}
	else if ((intPWM > -256) && (intPWM <= 0)) {
		setClockwise(false);		
		setPWM(-intPWM);
	}
	else if (intPWM == 0){
		digitalWrite(pinInputA, HIGH);
		digitalWrite(pinInputB, HIGH);
		setPWM(0);
	}
}

//Let's REDO the motorGO function, and split it into two
//independent functions motorGOCW, motorGOCCW



void Motor::motorGo(int intPWM){
	if ((intPWM < 256) && (intPWM >= 0)) {
		setPWM(intPWM);
	}
	else if (intPWM == 0){
		//apply motor brakes?
		digitalWrite(pinInputA, HIGH);
		digitalWrite(pinInputB, HIGH);
		setPWM(0);
	}
}


//fewer checks, does it really help? not really
//void Motor::motorGo2(int intPWM){
//	if (intPWM >= 0) {
//		setClockwise(true);
//		setPWM(intPWM);
//	}
//	else {
//		setClockwise(false);		
//		setPWM(-intPWM);
//	}
//}