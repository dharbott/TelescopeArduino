#ifndef Motor_H
#define Motor_H
 
#include "Arduino.h"

#define mopbufferlen 256

class Motor {

private:
		int pinPWM;
		int pinInputA;
		int pinInputB;
		bool clockwise;
		int currentPWM;
		bool moving;
		
public:
		Motor();
        Motor(int ppinPWM, int ppinInputA, int ppinInputB);
		~Motor();
		
		long pinOut();	//verified, change to String later
		
		void setPWM(int intPWM);	//verified

		void setClockwise(bool clockwise);	//verified
		bool isClockwise(); 
		
        void motorGo(int intPWM);	//verified
		void motorGo2(int intPWM);

		// another way of handling motor commands
		void motorSetup(int intPWM);
		void motorStart();
		void motorBrake();
		void changePWM(int intPWM);
};
 
#endif