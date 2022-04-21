#include "constants.h"

extern int curTest;
extern unsigned int  control_mode;
extern unsigned long curTime, prevTimeControl, initalHoldMillis;
extern double LM35Temp, ambTemp, curError, fullError, vRes, inext;
extern float Kpdc, Ki;

void hold_charge(int nextTask)
{
  digitalWrite(chargePin, LOW);
  pwmWrite(pwmPin, 0);
  if (curTime - initalHoldMillis > HOLD_TIME && LM35Temp < ambTemp)
  {
    if (nextTask == CHARGE_MODE && curTest == CONSTANT_CURRENT)
      Serial.println("End of discharge.");

    control_mode = nextTask;
  }
}

float readPinValue(int pin)
{
  float temp = 0;
  for (int i = 0; i < 20; i++)
  {
    temp += (float)analogRead(pin);
  }
  temp = (5.0 * temp) / (1024.0 * 20.0);
  return temp;
}

void controlCurrent(float vTarget)
{
  curError = vTarget - vRes;
  inext = ceil((Ki * (curError + fullError) * ((curTime - prevTimeControl) / 1000.0)) + (curError * Kpdc));
  if(inext < 0)
    inext = 0;
  pwmWrite(pwmPin, inext);
  fullError += curError;
}

void emergencyCutOff()
{
  digitalWrite(cutOffPins[0], HIGH); // sets the digital pin 13 on
  digitalWrite(cutOffPins[1], HIGH); // sets the digital pin 13 on
}
