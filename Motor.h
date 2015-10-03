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
    int quadraturePin;

    void rawPWM(int intPWM); //don't use

  public:
    Motor();
    Motor(int ppinPWM, int ppinInputA, int ppinInputB);
    Motor(int ppinPWM, int ppinInputA, int ppinInputB, int quadPin);
    ~Motor();

    long pinOut();	//verified, change to String later
    int getQuadPin();

    void motorGo(int intPWM);	//verified
    void setPWM(int intPWM);	//verified
    int getPWM();

    void setClockwise(bool clockwise);	//verified
    bool isClockwise();

};

#endif
