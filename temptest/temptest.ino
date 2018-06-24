#include <math.h>
#include <SPI.h>
#include "mcp_can.h"

const char canID1 = 0x123;
const char canID2 = 0x124;
const int SPI_CS_PIN = 11; // SPI chip select pin
MCP_CAN CAN(SPI_CS_PIN);

int flowtemp = A0;
int temp1 = A5;
int temp2 = A4;
int temp3 = A3;
int temp4 = A2;
int temp5 = A1;
int flowint = 2;

int temp1Val;
int temp2Val;
int temp3Val;
int temp4Val;
int temp5Val;
int flowtempVal;

double V1;
double V2;
double V3;
double V4;
double V5;
double Vflow;

double R1;
double R2;
double R3;
double R4;
double R5;
double Rflow;

int Temp1;
int Temp2;
int Temp3;
int Temp4;
int Temp5;
int flowTemp;

double B = 3430.0;
double Bflow = 3950.0;
double T0 = 298.15;

int len = 6;
unsigned char data1[6];
unsigned char data2[6];

volatile int NbTopsFan; //measuring the rising edges of the signal
int Flow;
int hallsensor = 2; //The pin location of the sensor
void rpm () //This is the function that the interupt calls
{
  NbTopsFan++; //This function measures the rising and falling edge of the hall effect sensors signal
}

void setup() {
  // put your setup code here, to run once:
  pinMode(hallsensor, INPUT); //initializes digital pin 2 as an input
  attachInterrupt(0, rpm, RISING); //and the interrupt is attached
  while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  NbTopsFan = 0;
  sei(); //Enables interrupts
  delay(1000);
  cli(); //Disable interrupts
  Flow = ((NbTopsFan * 60.0) / 7.5);
  //Serial.print (Flow, DEC);
  //Serial.print (" L/hour\r\n");

  temp1Val = analogRead(temp1);
  temp2Val = analogRead(temp2);
  temp3Val = analogRead(temp3);
  temp4Val = analogRead(temp4);
  temp5Val = analogRead(temp5);
  flowtempVal = analogRead(flowtemp);

  V1 = temp1Val * 5.0 / 1023.0;
  V2 = temp2Val * 5.0 / 1023.0;
  V3 = temp3Val * 5.0 / 1023.0;
  V4 = temp4Val * 5.0 / 1023.0;
  V5 = temp5Val * 5.0 / 1023.0;
  Vflow = flowtempVal * 5.0 / 1023.0;

  R1 = V1 * 10000.0 / (5 - V1);
  R2 = V2 * 10000.0 / (5 - V2);
  R3 = V3 * 10000.0 / (5 - V3);
  R4 = V4 * 10000.0 / (5 - V4);
  R5 = V5 * 10000.0 / (5 - V5);
  Rflow = Vflow * 50000.0 / (5 - Vflow);

  Temp1 = (int) (10 * (B / (log(R1 / (10000.0 * exp(-B / T0)))) - 273.15));
  Temp2 = (int) (10 * (B / (log(R2 / (10000.0 * exp(-B / T0)))) - 273.15));
  Temp3 = (int) (10 * (B / (log(R3 / (10000.0 * exp(-B / T0)))) - 273.15));
  Temp4 = (int) (10 * ( B / (log(R4 / (10000.0 * exp(-B / T0)))) - 273.15));
  Temp5 = (int) (10 * (B / (log(R5 / (10000.0 * exp(-B / T0)))) - 273.15));
  flowTemp = (int) (10 * (Bflow / (log(Rflow / (50000.0 * exp(-Bflow / T0)))) - 273.15));

  data1[0] = highByte(flowTemp);
  data1[1] = lowByte(flowTemp);
  data1[2] = highByte(Flow);
  data1[3] = lowByte(Flow);
  data1[4] = highByte(Temp1);
  data1[5] = lowByte(Temp1);
  data1[6] = highByte(Temp2);
  data1[7] = lowByte(Temp2);
  data2[0] = highByte(Temp3);
  data2[1] = lowByte(Temp3);
  data2[2] = highByte(Temp4);
  data2[3] = lowByte(Temp4);
  data2[4] = highByte(Temp5);
  data2[5] = lowByte(temp5);

  CAN.sendMsgBuf(canID1, 0, 8, data1);
  CAN.sendMsgBuf(canID2, 0, 6, data2);
}
