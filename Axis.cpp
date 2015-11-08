#include "Axis.h"

Axis::Axis()
{
  //empty constructors??
  motor = Motor();
  encoder = MagneticEncoder();
  target = 0;
  countOffset = 0;
  currentPWM = 0;
  slewing = false;

}


Axis::Axis(Motor myMotor, MagneticEncoder myEncoder)
{
  motor = myMotor;
  encoder = myEncoder;
  target = 0;
  countOffset = 0;
  currentPWM = 0;
  slewing = false;
  
  //state = 0, 1, or 2
  //      0 is parked, 1 is slewing to position, 2 is moving at a rate
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
  int temp0 = input;

  //negative overflow? OR positive overflow?
  if (temp0 < 0) temp0 += maxcount;
  if (temp0 >= maxcount) temp0 -= maxcount;

  target = temp0;
  slewing = true;
}


// CHANGING
void Axis::processPosition()
{
  int distance = 0;
  int distance2 = 0;
  int maxCount = encoder.getMaxCount();

  distance = encoder.getCWDistance(getUserSyncCount(), target);

  // if the clockwise distance is greater than half
  // the degrees (i.e. Encoder Count) then we don't
  // want to traverse clockwise, but counterclockwise
  // so set clockwise to false, and make distance
  // the complement

  if (distance >= (maxCount / 2))
  {
    distance = 4096 - distance;
    motor.setClockwise(false);
  }
  else
  {
    motor.setClockwise(true);
  }

  //if distance >= 114 counts
  if (distance >= (maxCount / 64))
  {
    updatePWM(255);
  }
  else if (distance >= 25)
  {
    //value jumps from 255 to 100,
    //then jumps to 80,
    //then jumps to 60, 40, 20
    updatePWM((distance / 25) * 25);
  }
  else if (distance > 0)
  {
    updatePWM(22);
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


bool Axis::getClockwise()
{
  return motor.isClockwise();

}

void Axis::updatePWM(int intPWM)
{
  if (intPWM != currentPWM)
  {
    currentPWM = intPWM;
    motor.setPWM(currentPWM);
  }
}



void Axis::movePWM(int intPWM)
{
  motor.setClockwise(intPWM > 0);
  
  if (intPWM < 0) intPWM = -intPWM;
    
  if (intPWM != currentPWM)
  {
    currentPWM = intPWM;
    motor.setPWM(currentPWM);
  }
}


void Axis::setRate(int intRate)
{
  rate = intRate;
}


void Axis::processRate()
{
  
  
}



// Changes direction of the motor
void Axis::reverse()
{
  motor.setClockwise(!motor.isClockwise());
}


// Sets the pwm to 0, target to match current
void Axis::abort()
{
  slewing = false;
  updatePWM(0);

  // Delay may be necessary as motor
  // stops and settles on a position
  delay(50);
  target = getUserSyncCount();
  return;
}


int Axis::getUserSyncCount()
{
  int maxcount = encoder.getMaxCount();

  //sync = current + offset
  //offset = sync - current
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

  //sync = current + offset
  //offset = sync - current

  countOffset = input - encoder.getMECount();

}
