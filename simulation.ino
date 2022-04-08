#include "constants.h"

#define TEST 1

#if TEST == 1
  #include "corrente_celltester_1volta.h"
#elif TEST == 2
   #include "corrente_celltester_corrida.h"
#endif

extern int step;
extern float Id[];
int index_t = 0;
const int index_t_total =sizeof(Id) / sizeof(Id[0]);

extern double shuntCur, vBat, LM35Temp;
extern float chargedCur, chargedVBat;
extern int curDischarge, control_mode, charge_counter, curTest, next_mode;
extern unsigned long holdChargeMillis;
extern boolean aquisition;
extern unsigned long discharge_begin;

void discharge_battery(float cur)
{
  if (vBat >= 2.50 && LM35Temp < 75)
  {
    controlCurrent(cur * (resValue / numRes));
  }
  else
  {
    switchToHold(TEST_END);
  }
}

void corrente_ref(long curTime){
  switch (control_mode)
  {
  case CHARGE_MODE:
    aquisition = true;
    charge_battery(charge_counter);
    break;
  case DISCHARGE_MODE:
    aquisition = true;
    if((int)(curTime-discharge_begin)%step==0){
      index_t=curTime/step;
    }

    if(index_t >= index_t_total)
      switchToHold(TEST_END);
      break;

    discharge_battery(Id[index_t]);
    break;
  case HOLD_CHARGE_MODE:
    aquisition = true;
    discharge_begin = curTime;
    hold_charge(next_mode);
    break;
  case TEST_END:
    aquisition = false;
    while(1) {
      Serial.println("End of test.\r\n\r\n");
      digitalWrite(chargePin, LOW);
      pwmWrite(pwmPin, 0);
      delay(10000);
    }
    break;
  default:
    aquisition = false;
    digitalWrite(chargePin, LOW);
    pwmWrite(pwmPin, 0);
    break;
  }
}
