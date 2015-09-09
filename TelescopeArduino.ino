#include "Motor.h"
#include "MagneticEncoder.h"
#include "Axis.h"

#define bufflen 8
#define codelen 16

#define CLOCK_PIN 13
#define SELECT_PIN 12
#define ME1_DATA_PIN 11
#define ME2_DATA_PIN 10

#define MD1_PWM 9
#define MD1_INA 8
#define MD1_INB 7

#define MD2_PWM 6
#define MD2_INA 5
#define MD2_INB 4

#define QUAD_A_PIN 2 //not used
#define QUAD_B_PIN 3 //not used

#define HALL_PIN A4

#define MD1CS_PIN A1
#define MD2CS_PIN A2

#define LSOUT A3
#define LSOVR A0


/**
struct bytepair {
	//FORMAT    2    1
	//binary 0010 0001
	//       MSBy LSBy
	byte MSByte;
	byte LSByte;
};

typedef struct bytepair myBP;
//Structs don't get procesed until after the functions
//problematic Arduion compiler
**/

// ByteArray, my working internal command queue
// Arduino Serial buffer still limited at 62 bytes
// I wanted the command buffer to be fast and stable
byte byteArray[bufflen][codelen] = {};

// 00000;11111;22222;33333;44444;55555;66666;77777;88888;99999;

int stringCount = 0;
int i = 0;
int current = 0;
int nextIn = 0;
//bool slewingAsync = false;

Axis Azimuth = Axis(
                 Motor(MD2_PWM, MD2_INA, MD2_INB),
                 MagneticEncoder(SELECT_PIN, CLOCK_PIN, ME1_DATA_PIN)
               );

Axis Altitude = Axis(
                  Motor(MD1_PWM, MD1_INA, MD1_INB),
                  MagneticEncoder(SELECT_PIN, CLOCK_PIN, ME2_DATA_PIN)
                );


//TODO : DOUBLE CHECK PIN NUMBERS
void setup()
{

  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(SELECT_PIN, OUTPUT);
  pinMode(ME1_DATA_PIN, INPUT);
  pinMode(ME2_DATA_PIN, INPUT);

  pinMode(MD1_PWM, OUTPUT);
  pinMode(MD1_INA, OUTPUT);
  pinMode(MD1_INB, OUTPUT);

  pinMode(MD2_PWM, OUTPUT);
  pinMode(MD2_INA, OUTPUT);
  pinMode(MD2_INB, OUTPUT);

  //give some default values
  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(SELECT_PIN, HIGH);

  analogWrite(MD1_PWM, 0);
  digitalWrite(MD1_INA, LOW);
  digitalWrite(MD1_INB, LOW);

  analogWrite(MD2_PWM, 0);
  digitalWrite(MD2_INA, LOW);
  digitalWrite(MD2_INB, LOW);

  //setup pins for Quadrature, not used
  //pinMode(QUAD_A_PIN, INPUT);
  //pinMode(QUAD_B_PIN, INPUT);

  pinMode(HALL_PIN, INPUT);

  pinMode(LSOUT, INPUT);
  pinMode(LSOVR, OUTPUT);

  // put your setup code here, to run once:
  Serial.begin(19200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  //delay(1000);
}


void loop()
{

  //360 degrees/rev * 60 minutes/degree = 21600 minutes/rev
  //21600 fits in 16-bits, a 2-byte integer on Arduino

  int param1 = 0;
  int param2 = 0;
  int param3 = 0;
  float tempf;

  //Do Async Motor Actions even if no commands in queue
  if (Azimuth.getSlewing() || (Altitude.getSlewing()))
  {
    Azimuth.processME();
    Altitude.processME();

    //SKETCHY!!!
    //if (!Azimuth.getSlewing()) Serial.write ("Slewing Finished;");

    //if ((!Azimuth.getSlewing()) && (!Altitude.getSlewing()))
    //Serial.write ("Slewing Finished;");
  }

  //if the command queue is not empty
  if (current != nextIn)
  {
    //Serial.write("#Instructions left : [");
    //Serial.print(stringCount);
    //Serial.write("] - Processing...;\t");

    //driver sent a 2-byte char, but we only need to check
    //  the first byte, since Arduino uses 1-byte ASCII
    switch (byteArray[current][0])
    {

      //CHECK IF SLEWING ASYNCHRONOUS, MOTORS BUSY
      case '0':
        //Serial.write("You sent a char '0'.\n");

        if (Azimuth.getSlewing() || Altitude.getSlewing())
        {
          //Serial.write("Motors Busy;");
          //return; //motor(s) busy with a command
        }
        else
        {
          //Serial.write("Ready;");
        }

        break;

      //SLEW TO ALT AZM SYNCHRONOUS, RETURN WHEN DONE
      case '1':

        //CASE 49, my GOTO Function 1
        //parameters are integers - arcminutes
        //motorSetup determines shortest path
        //determines clockwise or counter-

        //check logic, where and when to check if motor is busy

        //Q: Do we accept SlewAltAz Sync commands while slewing asynch?
        //A: I don't know yet

        //if (Azimuth.getSlewing() || Altitude.getSlewing()) return; //motor(s) busy with a slew async! RETURN

        param1 = getParam1(byteArray[current]);
        param2 = getParam2(byteArray[current]);

        Azimuth.motorSetup(Azimuth.getEncoder().minutesToCount(param1));
        Altitude.motorSetup(Altitude.getEncoder().minutesToCount(param2));

        //SKETCHY!!!
        //Removed the while loop processME, relying solely on the
        //processME on the outside to catch everything...

        //LOOP WHILE still processing
        while (Azimuth.getSlewing() || Altitude.getSlewing())
        {
          //when we do processME(), we calculate the distance
          // from current angle to target angle, but once we reach
          // a distance of 0 (or was it 1) then getSlewing()
          // returns false, and processME() doesn't get called
          // again, even the value somehow shifts or changes after
          // it has stopped

          if (Azimuth.getSlewing())
            Azimuth.processME();
          if (Altitude.getSlewing())
            Altitude.processME();
        }

        Serial.write("Slewing Finished;");

        break;

      //DRIVER.Azimuth get()
      case '2':

        param2 = Azimuth.getEncoder().getMECount();

        //INSERT FUNCITON: INT TO BYTE PAIR - HERE
        //ASSUME THAT COUNTTOMINUTES NEVER EXCEEDS 32K..
        //ELSE CONVERTING TO BYTES AND SENDING IT WILL
        //YIELD A PROBLEM WITH THE NEGATIVE BIT??
        tempf = Azimuth.getEncoder().countToAngleFloat(param2);

        Serial.print(tempf);
        
        //Serial.print(param2 >> 8);
        //Serial.print(param2 & 255);
        
        Serial.write(";");
        break;

      //DRIVER.Altitude get
      case '3':
      
        param2 = Altitude.getEncoder().getMECount();
        
        tempf = Altitude.getEncoder().countToAngleFloat(param2);

        Serial.print(tempf);
        
        //INSERT FUNCITON: INT TO BYTE PAIR - HERE
        //Serial.write(param3 >> 8);
        //Serial.write(param3 & 255);
        
        Serial.write(";");
        break;

      case '4':
        //Serial.write("You sent a char '4'.\n");
        break;

      case '5':
        //Serial.write("You sent a char '5'.\n");
        break;

      case '6':
        //Serial.write("You sent a char '6'.\t");
        break;

      //SYNC USER COORD: AZIMUTH, SYNC USER COORD: ALTITUDE
      case '7':
        //Serial.write("You sent a char '7'.\t");

        //DRIVER.SyncToAltAz(Azimuth,Altitude), parameters in "arcminutes"
        if (Azimuth.getSlewing() || Altitude.getSlewing()) return; //motor(s) busy with a command

        param1 = Azimuth.getEncoder().minutesToCount(getParam1(byteArray[current]));
        param2 = Altitude.getEncoder().minutesToCount(getParam2(byteArray[current]));

        Azimuth.setUserSyncCount(param1);
        Altitude.setUserSyncCount(param2);

        //Serial.write("Sync User Coords (Azm, Alt), Started;");
        break;

      //SLEW ASYNCHRONOUS, RETURN IMMEDIATELY
      case '8':
        //Serial.write("You sent a char '8'.\t");

        //FORMAT : CODE - Azimuth in ArcMin - Altitude in ArcMin
        //This function determines whether going clockwise or counter
        //is the shortest path, and takes it

        if (Azimuth.getSlewing() || Altitude.getSlewing()) return; //motor(s) busy with a command

        param1 = getParam1(byteArray[current]);
        param2 = getParam2(byteArray[current]);

        Azimuth.motorSetup(Azimuth.getEncoder().minutesToCount(param1));
        //Altitude.motorSetup(Altitude.getEncoder().mintesToCount(param2));

        //Serial.write("Slewing Async Started;");
        break;

      //SO, is there anything there to notify the Driver that
      //SlewAsync finished? Not yet....

      //DRIVER.ABORTSLEW()
      case '9':
        //Serial.write("You sent a char '9'.\t");

        Azimuth.abort();
        Altitude.abort();

        //Serial.write("Slewing Async Aborted;");
        break;

      //We can make as many cases as there are ASCII 8-bit characters

      default:
        //Serial.write("You sent a non-cmd : ");
        //Serial.write(byteArray[current][0]);
        //Serial.write(";");
        break;
    }


    //clear out the instruction code
    for (int j = 0; j < codelen; j++) byteArray[current][j] = 0;

    currentPlus();
    stringCount--;

  }
  delay(500);
}


void timeCheck()
{
  unsigned long timestart = micros();
  unsigned long timeend = micros() - timestart;
  Serial.print("TIME : ");
  Serial.print(timeend);
  Serial.print(" microseconds.\n");
}


//round robin index, array position of current instruction
void currentPlus()
{
  current = current + 1;
  if (current >= bufflen)
    current = 0;
}

//round robin index, a spot to insert next instruction
void nextInPlus()
{
  nextIn = nextIn + 1;
  if (nextIn >= bufflen)
    nextIn = 0;
}



//SERIALEVENT() is run after every loop() returns
//This function will feed bytes from Serial Stream into
//a buffer, to be stored internally, because Serial buffer
//is only 63 bytes, or 63 chars
void serialEvent()
{
  byte inByte;

  //keep adding instructions as long as there are bytes in serial available
  //and while there is at least one empty spot for an instruction in the buffer
  //this stringCount+1, because nextIn points to an empty spot in the buffer
  while (Serial.available() && ((stringCount + 1) < bufflen))
  {
    inByte = Serial.read();

    if (inByte == ';')
    {
      //THIS FIX: because ';' is sent as 2 bytes,
      //  but Serial.read() only reads in 1 byte at a time
      //  so we have to get rid of the trailing byte
      Serial.read();
      nextInPlus();
      stringCount++;
      i = 0;
    }
    else
    {
      byteArray[nextIn][i++] = inByte;
    }
  }

  //EMERGENCY STOP -> '!'
  //if the command is abort, abort()
  //happens the instant command '!' is received
  //not with command '9', command '!' is EMERGENCYSTOP
  if (byteArray[nextIn][1] == '!')
  {
    Azimuth.abort();
    Altitude.abort();

    //Serial.write("Slewing (Sync) Aborted;");

    //clear current abort command
    for (int j = 0; j < codelen; j++) byteArray[current][j] = 0;

    currentPlus();
    stringCount--;

    //clear all motor commands?
    //currently it only aborts current SlewAsync command
  }
}


//Version 1: Parameter 1 is typically a 16-bit integer
//representing computed 'arcminutes' that represent a position
//on the altitude local coordinate system
unsigned int getParam1(byte bytesIn[])
{
  //	unsigned int retval = bytesIn[1] + (bytesIn[2] << 8);
  //	return retval;
  return (bytesIn[2] + (bytesIn[3] << 8));
}


//Version 1: Parameter 2 is typically a 16-bit integer
//representing computed 'arcminutes' that represent a position
//on the azimuth local coordinate system
unsigned int getParam2(byte bytesIn[])
{
  //unsigned int retval = bytesIn[3];
  //retval += bytesIn[4] << 8;
  //return (retval);
  return (bytesIn[4] + (bytesIn[5] << 8));
}
