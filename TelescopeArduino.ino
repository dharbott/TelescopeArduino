#include "Motor.h"
#include "MagneticEncoder.h"

#define bufflen 8
#define codelen 16

#define SELECT_PIN 12
#define CLOCK_PIN 13
#define ME1_DATA_PIN 11
#define ME2_DATA_PIN 10

#define MD1_PWM 9
#define MD1_INA 8
#define MD1_INB 7

#define MD2_PWM 6
#define MD2_INA 5
#define MD2_INB 4

//#define QUAD_A_PIN 2 //not used
//#define QUAD_B_PIN 3 //not used

#define HALL_PIN A4

#define MD1CS_PIN A1
#define MD2CS_PIN A2

#define LSOUT A3
#define LSOVR A0

struct mdriver {
	Motor motor;
	MagneticEncoder encoder;
};

mdriver Azimuth = {
	Motor(MD2_PWM, MD2_INA, MD2_INB),
	MagneticEncoder(SELECT_PIN, CLOCK_PIN, ME1_DATA_PIN)
};

mdriver Altitude = {
	Motor(MD1_PWM, MD1_INA, MD1_INB),
	MagneticEncoder(SELECT_PIN, CLOCK_PIN, ME2_DATA_PIN)
};

//Motor myMotorAzm = Motor(MD2_PWM, MD2_INA, MD2_INB);
//Motor myMotorAlt = Motor(MD1_PWM, MD1_INA, MD1_INB);

//MagneticEncoder myMEAzm = MagneticEncoder(SELECT_PIN, CLOCK_PIN, ME1_DATA_PIN);
//MagneticEncoder myMEAlt = MagneticEncoder(SELECT_PIN, CLOCK_PIN, ME2_DATA_PIN);


//=============================================================//

// 00000;11111;22222;33333;44444;55555;66666;77777;88888;99999;

// queue works, Arduino Serial buffer still limited at 62 bytes
byte byteArray[bufflen][codelen] = { 
};

int stringCount = 0;
int i = 0;
int current = 0;
int nextIn = 0;

//Not efficient? may make too many calls to getMECount or getDistance
void motorGO(mdriver axis, int inputMECount) {
	int temp1 = axis.encoder.getCWDistance(axis.encoder.getMECount, inputMECount);
	int temp2 = axis.encoder.getCCWDistance(axis.encoder.getMECount, inputMECount);

	if (temp1<temp2)
		motorGOCW
}


void motorGOCW(mdriver axis, int inputMECount) {
  int temp = 0;
  int tempspeed;

  while (true) {
//    temp = getCWDistance (getMECount(), inputMECount);
    if  (temp >= 100)
      tempspeed = 255; 
    else if (temp >= 40)
      tempspeed = 100;
    else if (temp >= 1)
      tempspeed = 20;
    else
      break;
	axis.motor.motorGo(tempspeed);
    delay(1);
  }
  axis.motor.motorGo(0);
}


//SAVE MEMORY - MERGE SIMILAR FUNCTIONS
void motorGOCCW(mdriver axis, int inputMECount) {
  int temp = 0;
  int tempspeed;  

  while (true) {
   // temp = getCWDistance (inputMECount, getMECount());
    if  (temp >= 100)
      tempspeed = -255;   
    else if (temp >= 40)
      tempspeed = -100;
    else if (temp >= 1)
      tempspeed = -20;  
    else
      break;
	axis.motor.motorGo(tempspeed);
    delay(10);
  }
  axis.motor.motorGo(0);
}



//TODO : DOUBLE CHECK PIN NUMBERS
void setup() {

  pinMode(SELECT_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
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
  digitalWrite(13,LOW);

  delay(1000);

  Serial.println("start");
}


//round robin index, array position of current instruction
void currentPlus() {
  current = current + 1;
  if (current >= bufflen)
    current = 0;
}

//round robin index, a spot to insert next instruction
void nextInPlus() {
  nextIn = nextIn + 1;
  if (nextIn >= bufflen)
    nextIn = 0;
}


void timeCheck() {
  unsigned long timestart = micros();
  unsigned long timeend = micros() - timestart;
  Serial.print("TIME : ");
  Serial.print(timeend);
  Serial.print(" microseconds.\n"); 
}


void loop() {

  //360 degrees/rev * 60 minutes/degree = 21600 minutes/rev
  //21600 fits in 16-bits, a 2-byte integer on Arduino

  int tempParam1 = 0;
  int tempParam2 = 0;

  if (current!=nextIn)
  {     
    Serial.write("instructions left : ");
    Serial.print(stringCount);
    Serial.write('\t');

    switch (byteArray[current][0])
    {
    case 48:
      Serial.write("You sent a char '0'.\t");
      break;

    case 49:
      //CASE 49, my GOTO Function 1, probably the ClockwiseGOTO
      //FORMAT : CODE - Azimuth in ArcMin - Altitude in ArcMin

      tempParam1 = getParam1(byteArray[current]);
      tempParam2 = getParam2(byteArray[current]);

      Serial.write("You sent a char '1', GOTO Command.\t");
      Serial.print ("P1 Azimuth : ");
      Serial.print (tempParam1);
      Serial.print ("\tP2 Altitude : ");
      Serial.print (tempParam2);

      //Clockwise or CounterClockwise?
      //check distance, go whichever way is shortest

      //change motorGOCW to 2 parameters, to include motor object
	  //no, yes? need to implement asynchronous slewing
	  //
      //motorGOCW(tempParam1);

      //Clockwise or CounterClockwise?
      //check distance, go whichever way is shortest
      //motorAlt.motorGo(tempParam2);

      break;

    case 50:
      Serial.write("You sent a char '2'.\t");
      break;

    case 51:
      Serial.write("You sent a char '3'.\t");
      break;

    case 52:
      Serial.write("You sent a char '4'.\t");
      break;

    case 53:
      Serial.write("You sent a char '5'.\t");
      break;

    case 54:
      Serial.write("You sent a char '6'.\t");
      break;

    case 55:
      Serial.write("You sent a char '7'.\t");
      break;

    case 56:
      Serial.write("You sent a char '8'.\t");
      break;

    case 57:
      Serial.write("You sent a char '9'.\t");
      break;

    default:
      Serial.write("You sent a non-cmd : ");
      Serial.write(byteArray[current][0]);
      Serial.write('\t');
      break; 
    }

    //clear out the instruction code
    for (int j = 0; j < codelen; j++) byteArray[current][j] = 0;

    currentPlus();
    stringCount--;

    Serial.write(";\n");
  }
  delay(500);
}


//void serialReady()
//{
//  Serial.write(Serial.available());
//    Serial.write(stringCount);
//    Serial.write(';');
//}


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
  while (Serial.available() && ((stringCount+1) < bufflen)){

    inByte = Serial.read();

    if (inByte == ';') {
      nextInPlus();
      stringCount++;
      i = 0;
    }
    else {
      byteArray[nextIn][i++] = inByte;
    }
  }
}


//ENDIAN-NESS IS IMPORTANT
//These functions work, serial input "22323;"
//output is "12851;"
//which is 00110010 00110011
//corresponding to "50" "51" 

//Version 1: Parameter 1 is typically a 16-bit integer
//representing computed 'arcminutes' that represent a position
//on the altitude local coordinate system
unsigned int getParam1(byte bytesIn[])
{
  unsigned int retval = bytesIn[2];
  retval += bytesIn[1] << 8;
  return (retval);
}


//ENDIAN-NESS IS IMPORTANT

//Version 1: Parameter 2 is typically a 16-bit integer
//representing computed 'arcminutes' that represent a position
//on the azimuth local coordinate system
unsigned int getParam2(byte bytesIn[])
{
  unsigned int retval = bytesIn[4];
  retval += bytesIn[3] << 8;
  return (retval);
}


