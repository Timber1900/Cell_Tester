#include "constants.h"

extern double shuntCur, vBat, resCur;
extern unsigned long prevTimeControl, curTime;
extern int curDischarge, charge_counter, curTest, next_mode, numTests;
extern unsigned int control_mode;
extern float chargedCur, chargedVBat;
extern unsigned long holdChargeMillis;
extern boolean aquisition;

unsigned long discharge_begin;
float soc = 0;

void charge_battery_step(float *curSoc)
{
  digitalWrite(chargePin, HIGH);
  if (*curSoc >= 0)
  {
    switchToHold(DISCHARGE_MODE);
  }
  *curSoc += shuntCur * (curTime - prevTimeControl);
}

void discharge_battery_step(float cur, int discharge_milliseconds, float *curSoc)
{
  if (vBat >= 2.5 && (int)(curTime - discharge_begin) <= discharge_milliseconds)
  {
    controlCurrent(cur * (resValue / numRes));
    *curSoc -= resCur * (curTime - prevTimeControl);
  }
  else
  {
    if (vBat < 2.5)
    {
      while (1)
      {
        Serial.println("End of stepped discharge test. Please reset arduino and remove cell.");
        digitalWrite(chargePin, LOW);
        pwmWrite(pwmPin, 0);
        delay(10000);
      }
    }
    else if (curDischarge >= numTests)
    {
      switchToHold(TEST_END);
    }
    else
    {
      curDischarge++;
      switchToHold(CHARGE_MODE);
    }
  }
}

void stepped_discharge(float cur, int discharge_milliseconds)
{
  switch (control_mode)
  {
  case CHARGE_MODE:
    aquisition = true;
    charge_battery_step(&soc);
    break;
  case DISCHARGE_MODE:
    aquisition = true;
    discharge_battery_step(cur, discharge_milliseconds, &soc);
    break;
  case HOLD_CHARGE_MODE:
    aquisition = true;
    discharge_begin = curTime;
    hold_charge(next_mode);
    break;
  case TEST_END:
    aquisition = false;
    Serial.println("End of test.\r\n\r\n");
    soc = 0;
    curDischarge = 0;
    control_mode = CHARGE_MODE;
    digitalWrite(chargePin, LOW);
    pwmWrite(pwmPin, 0);
    break;
  default:
    aquisition = false;
    digitalWrite(chargePin, LOW);
    pwmWrite(pwmPin, 0);
    break;
  }
}
