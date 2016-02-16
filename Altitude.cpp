#include "Altitude.h"


Altitude::Altitude(Motor myMotor, MagneticEncoder myEncoder) : Axis(myMotor, myEncoder)
{
  //calls parent constructor using passed variables
}

void Altitude::findLimits()
{
  /****
  int target = getEncoder().getMECount();
  motorSetup();

  while (getSlewing())
  {
    if (getSlewing())
      processPosition();

    if (digitalRead(A3) == LOW)
    {

      //abort motion on Altitude motor
      reverse();
      updatePWM(50);

      delay(50);

      digitalWrite(A0, HIGH);
      delay(50);

      while (analogRead(A3) < 600)
      {
        delay(50);
      }

      digitalWrite(A0, LOW);

      abort();
      delay(50);
    }
  }
  **/
}

void Altitude::findSpeeds()
{
    //Hardest part is overcoming static friction to start moving
    //but once moving, it may be capable of slower speeds

    //TEST part 1, minimal voltage for movement from a standstill

    //TEST part 2, minimal voltage for movement while already moving

    //PART 1. Direction 1

    //SET HIGH,LOW //clockwise??
    delay(1000);
    
    bool cwDir = 1;

    int i = 128;
    int upperbound = i;
    int lowerbound = 0;
    int getARate = 0;
    
    while (true)
    {
      setClockwise(cwDir);
      updatePWM(i);
      delay(1000);
      getARate = getRate(); //sample time 1 second I think

      updatePWM(0);
      delay(500); //slow down and stop

      if (getARate > 1)        
        upperbound = i;  //upperbound becomes old i value
      else        
        lowerbound = i;  //lowerbound becomes old i value

      if ((upperbound - lowerbound) <= 2)
        break;

      i = (upperbound + lowerbound) / 2;
    }

    return;
  }
