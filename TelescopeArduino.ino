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


static bool tempCW = false;

void loop()
{

  //360 degrees/rev * 60 minutes/degree = 21600 minutes/rev
  //21600 fits in 16-bits, a 2-byte integer on Arduino

  int param1 = 0;
  int param2 = 0;
  int param3 = 0;
  float tempf;

  //Do Async Motor Actions even if no commands in queue

  if (Azimuth.getSlewing())
    Azimuth.processPosition();

  if (Altitude.getSlewing())
  {
    Altitude.processPosition();

    if (digitalRead(LSOUT) == LOW)
    {
      //abort motion on Azimuth motor
      Azimuth.abort();

      //abort motion on Altitude motor
      Altitude.reverse();
      Altitude.updatePWM(50);

      delay(50);

      digitalWrite(A0, HIGH);
      delay(50);

      while (analogRead(A3) < 600)
      {
        delay(50);
      }

      digitalWrite(A0, LOW);

      Altitude.abort();
      delay(50);
    }
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


      // NOT USED!!!
      //

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

        //Q: Do we accept SlewAltAz Sync commands while slewing asynch?
        //A: I don't know yet

        //if (Azimuth.getSlewing() || Altitude.getSlewing()) return; //motor(s) busy with a slew async! RETURN

        param1 = getParam1(byteArray[current]);
        param2 = getParam2(byteArray[current]);

        Azimuth.motorSetup(Azimuth.getEncoder().minutesToCount(param1));
        Altitude.motorSetup(Altitude.getEncoder().minutesToCount(param2));


        while (Azimuth.getSlewing() || Altitude.getSlewing())
        {
          if (Azimuth.getSlewing())
            Azimuth.processPosition();

          if (Altitude.getSlewing())
            Altitude.processPosition();

          if (digitalRead(LSOUT) == LOW)
          {
            //abort motion on Azimuth motor
            Azimuth.abort();

            //abort motion on Altitude motor
            Altitude.reverse();
            Altitude.updatePWM(50);

            delay(50);

            digitalWrite(A0, HIGH);
            delay(50);

            while (analogRead(A3) < 600)
            {
              delay(50);
            }

            digitalWrite(A0, LOW);

            Altitude.abort();
            delay(50);

            //if we hit the hard limit, we fail
            //Serial.write("Altitude Limit Switch Triggered - ");
            //Q: IT SHOULDN"T THROW EXCEPTION ON LIMIT????
            //A: it's not handled in the documentation....
          }
        }

        Serial.write("Slewing Operation Finished");
        Serial.write('~');
        break;

      //DRIVER.Azimuth get()
      case '2':
        param2 = Azimuth.getUserSyncCount();
        tempf = Azimuth.getEncoder().countToAngleFloat(param2);

        Serial.print(tempf);
        Serial.write('~');
        break;

      //DRIVER.Altitude get()
      case '3':
        param2 = Altitude.getUserSyncCount();
        tempf = Altitude.getEncoder().countToAngleFloat(param2);

        Serial.print(tempf);
        Serial.write('~');
        break;

      //MOVE(Axis, Rate)
      // Axis 1 -> Altitude
      // Axis 2 -> Azimuth
      // Rate -> originally intended as deg/sec, but now assigned
      //         as arcminutes/second
      case '4':
        //Serial.write("You sent a char '4'.\n");

        //if (Azimuth.getSlewing() || Altitude.getSlewing()) return; //motor(s) busy with a command

        param1 = getParam(byteArray[current], 2);
        param2 = getParam(byteArray[current], 4);
        //Azimuth.motorSetup(Azimuth.getEncoder().minutesToCount(param1));
        //Altitude.motorSetup(Altitude.getEncoder().minutesToCount(param2));

        //Convert from Arcminutes/Second to some PWM value
        //crappy style
        param2 = param2 / 60;

        if (param1 == '1')
        {
          //Azimuth.movePWM(param2);
          Serial.write("Speed : "); Serial.print(param2, DEC);
          Serial.write(" || ");

          Serial.write("Move Axis -Azimuth- Started");
          Serial.write('~');
        }
        else if (param1 == '2')
        {
          //Altitude.movePWM(param2);
          Serial.write("Move Axis -Altitude- Started");
          Serial.write('~');
        }
        else
        {
          Serial.write("Move Axis Fail");
          Serial.write('~');
        }

        break;

      //Find limits on Altitude axis
      //and compute rate limit on Altitude axis
      //and store internally, temporarily??
      //maybe we can invest in flash memory
      case '5':
        //Serial.write("You sent a char '5'.\n");
        Serial.println("[result]");
        break;

      //Find limits on rotation
      case '6':

        Serial.print(Altitude.getRate());
        Serial.write('~');
        //Serial.write("You sent a char '6'.\t");
        break;

      //SYNC USER COORD: ALTITUDE, SYNC USER COORD: AZIMUTH
      case '7':
        //Serial.write("You sent a char '7'.\t");

        //DRIVER.SyncToAltAz(Altitude,Azimuth), parameters in "arcminutes"
        if (Altitude.getSlewing() || Azimuth.getSlewing()) return; //motor(s) busy with a command

        param1 = Altitude.getEncoder().minutesToCount(getParam1(byteArray[current]));
        param2 = Azimuth.getEncoder().minutesToCount(getParam2(byteArray[current]));

        Altitude.setUserSyncCount(param1);
        Azimuth.setUserSyncCount(param2);

        Serial.write("SyncToAltAz Complete");
        Serial.write('~');
        break;

      //SLEW ASYNCHRONOUS, RETURN IMMEDIATELY
      case '8':
        //Serial.write("You sent a char '8'.\t");

        //if (Azimuth.getSlewing() || Altitude.getSlewing()) return; //motor(s) busy with a command

        param1 = getParam1(byteArray[current]);
        param2 = getParam2(byteArray[current]);

        Azimuth.motorSetup(Azimuth.getEncoder().minutesToCount(param1));
        Altitude.motorSetup(Altitude.getEncoder().minutesToCount(param2));

        Serial.write("Slewing Async Started");
        Serial.write('~');
        break;

      //SO, is there anything there to notify the Driver that
      //SlewAsync finished? Not yet....

      //DRIVER.ABORTSLEW()
      case '9':
        //Serial.write("You sent a char '9'.\t");

        Azimuth.abort();
        Altitude.abort();

        Serial.write("Slewing Async Aborted");
        Serial.write('~');
        break;

      //We can make as many cases as there are ASCII 8-bit characters
      default:
        //Serial.write("You sent a non-cmd : ");
        //Serial.write(byteArray[current][0]);
        //Serial.write('~');
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
  Serial.print(" microseconds.");
  Serial.write('~');
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
  byte byteStringLength;
  int i = 0;
  int upperbound = 0;
  int lowerbound = 0;
  int getrated = 0;
  bool dire = true;

  //STEP 1: read in length of incoming byte stream
  //STEP 2: read those number of bytes
  //STEP 1.1: If it's a stop code??


  //keep adding instructions as long as there are bytes in serial available
  //and while there is at least one empty spot for an instruction in the buffer
  //this stringCount+1, because nextIn points to an empty spot in the buffer
  while (Serial.available() && ((stringCount + 1) < bufflen))
  {
    //inByte = Serial.read();

    byteStringLength = Serial.read();


    //************************************************************//

    if (byteStringLength == '5') {
      
      dire = !(Serial.read() == '-');

      while (Serial.available()) Serial.read();

      Serial.println("Hello David");
      Serial.println("starting speed tests");

      //Hardest part is overcoming static friction to start moving
      //but once moving, it may be capable of slower speeds

      //TEST part 1, minimal voltage for movement from a standstill

      //TEST part 2, minimal voltage for movement while already moving

      //
      // PART 1. Direction 1
      //////
      
      /***RESULTS
      AZUMITH:
        counterclockwise : minimum pwm is 15-17, for 2 count per 1000ms
        clockwise : minimum pwm is 15-17, for 2 count per 1000ms
        -maybe able to achieve lower PWM values with fully charged battery!
      ALTITUDE:
        clockwise (decreasing angle): min is 18-20, rate 2-3count/1000ms
        counterclockwise (increasing angle): min is 16-18, rate 2-3count/1000ms
        -maybe able to achieve lower PWM values with fully charged battery!
      ***/

      //SET HIGH,LOW //clockwise??
      Serial.print(" -starting motor PWM test, clockwise="); Serial.println(dire);
      delay(1000);

      i = 128;
      upperbound = 128;
      lowerbound = 0;
      while (true)
      {

        Serial.print("pwm: "); Serial.println(i);

        Altitude.setClockwise(dire);
        Altitude.updatePWM(i);
        delay(1000);
        getrated = Altitude.getRate(); //sample time 1 second I think

        Serial.print("rate: "); Serial.println(getrated);
        Altitude.updatePWM(0);
        delay(500); //slow down and stop

        if (getrated > 1)
          //upperbound becomes old i value
          upperbound = i;
        else
          //lowerbound becomes old i value
          lowerbound = i;

        if ((upperbound - lowerbound) <= 2)
          break;

        i = (upperbound + lowerbound) / 2;
      }

      Serial.println("Minimum PWM for Altitude motor: ");
      Serial.println(upperbound);
      return;

    }


    //************************************************************//


    byteStringLength = byteStringLength - 2;
    //subtract 2 bytes, the first byteStringLength, and the
    //terminating character's "blank space"


    //Serial.write("byteStreamLength : ");
    //Serial.print(byteStreamLength);
    //Serial.write('~');


    for (i = 0; i < byteStringLength; i++)
    {
      inByte = Serial.read();
      byteArray[nextIn][i] = inByte;
    }

    //if there's no terminating character '~' we'll basically
    //discard this instruction
    if (inByte == '~')
    {
      Serial.read(); //last byte should be empty, " "
      nextInPlus();
      stringCount++;
      //Serial.print("received terminating character 'TILDE' (U+007E)");
      //Serial.write('~');
    }
    else
    {
      Serial.print("Incomplete Message : byteStringLength : ");
      Serial.print(byteStringLength + 2);
      Serial.write('~');
    }
  }
}


//dangerous??!
unsigned int getParam(byte bytesIn[], int pos)
{
  return (bytesIn[pos] + (bytesIn[pos + 1] << 8));
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




/******************

#define MD1_PWM 9
#define MD1_INA 8
#define MD1_INB 7

#define LSOUT A3
#define LSOVR A0

bool state = false;
bool motorCW = false;

void setup() {
  // put your setup code here, to run once:

  pinMode(MD1_PWM, OUTPUT);
  pinMode(MD1_INA, OUTPUT);
  pinMode(MD1_INB, OUTPUT);

  pinMode(A3, INPUT);
  pinMode(A0, OUTPUT);

  digitalWrite(A0, LOW);

  Serial.begin(19200);

  motorCW = true;
  digitalWrite(MD1_INB, !motorCW);
  digitalWrite(MD1_INA, motorCW);

  analogWrite(MD1_PWM, 60);
}

void loop() {

  Serial.print("override on - ");
  Serial.print(state); Serial.print(" : limit switch status - ");
  Serial.print(analogRead(A3)); Serial.print(" :: ");
  Serial.println(digitalRead(A3));

  delay(1000);

  if (digitalRead(A3) == LOW)
  {

    digitalWrite(MD1_INB, LOW);
    digitalWrite(MD1_INA, LOW);

    delay(200);

    motorCW = !motorCW;
    digitalWrite(MD1_INB, !motorCW);
    digitalWrite(MD1_INA, motorCW);

    digitalWrite(A0, HIGH);
    while (digitalRead(A3) == LOW)
    {
      delay(50);
    }

    digitalWrite(A0, LOW);

    digitalWrite(MD1_INB, LOW);
    digitalWrite(MD1_INA, LOW);

    delay(200);

    motorCW = !motorCW;
    digitalWrite(MD1_INB, !motorCW);
    digitalWrite(MD1_INA, motorCW);
  }
}


******************/
