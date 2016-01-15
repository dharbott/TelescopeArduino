#ifndef Altitude_H
#define Altitude_H

#include "Axis.h"


class Altitude : public Axis
{
  
  public:
    Altitude(Motor myMotor, MagneticEncoder myEncoder);
    
    void findLimits();
  
};

#endif
