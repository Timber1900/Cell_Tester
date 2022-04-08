#include "constants.h"

extern double shuntCur, vBat, LM35Temp;
extern float chargedCur, chargedVBat;
extern int curDischarge, control_mode, charge_counter, curTest, next_mode;
extern int dischargeCurrents[1];
extern unsigned long holdChargeMillis;
extern boolean aquisition;

void charge_battery(int counter)
{
  digitalWrite(chargePin, HIGH);
  if ((shuntCur < chargedCur || vBat > chargedVBat) && counter % 500 == 0)
  {
    switchToHold(DISCHARGE_MODE);
  }
}

void discharge_battery_constant(float cur)
{
  if (vBat >= 2.50 && LM35Temp < 75)
  {
    controlCurrent(cur * (resValue / numRes));
  }
  else
  {
    if (curDischarge == (sizeof(dischargeCurrents) / sizeof(dischargeCurrents[0]) - 1))
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

void constant_current()
{
  switch (control_mode)
  {
  case CHARGE_MODE:
    aquisition = false;
    charge_battery(charge_counter);
    charge_counter++;
    break;
  case DISCHARGE_MODE:
    aquisition = true;
    discharge_battery_constant(dischargeCurrents[curDischarge]);
    break;
  case HOLD_CHARGE_MODE:
    aquisition = true;
    hold_charge(next_mode);
    break;
  default:
    aquisition = false;
    digitalWrite(chargePin, LOW);
    pwmWrite(pwmPin, 0);
    break;
  }
}
