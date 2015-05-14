#ifndef Motor_H
#define Motor_H
 
#include "Arduino.h"

#define mopbufferlen 256

struct mop {
		int  pwm;
		long  position;
};


class Motor {

private:
		int pinPWM;
		int pinInputA;
		int pinInputB;
		bool clockwise;
		int currentPWM;
		
public:

		Motor();
        Motor(int ppinPWM, int ppinInputA, int ppinInputB);
		~Motor();
		
		long pinOut();	//verified, change to String later
		
		void setPWM(int intPWM);	//verified
		void setClockwise(bool clockwise);	//verified
		bool isClockwise(); 
		
        void motorGo(int intPWM);	//verified
};
 
#endif