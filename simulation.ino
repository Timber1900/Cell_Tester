#include "constants.h"

#define TEST 1

#if TEST == 1
  #include "corrente_celltester_1volta.h"
#elif TEST == 2
   #include "corrente_celltester_corrida.h" //Não da para usar. Não tem memoria
#endif

extern int step;
extern int Id[];
int index_t = 0, prev_index = 0;
const int index_t_total = sizeof(Id) / sizeof(Id[0]);

extern double shuntCur, vBat, LM35Temp, fullError;
extern float chargedCur, chargedVBat;
extern int curDischarge, control_mode, charge_counter, curTest, next_mode;
extern unsigned long holdChargeMillis;
extern boolean aquisition;
extern unsigned long discharge_begin, curTime;

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

void corrente_ref(void){
  switch (control_mode)
  {
  case CHARGE_MODE:
    aquisition = false;
    charge_battery(charge_counter);
    charge_counter++;
    break;
  case DISCHARGE_MODE:
    aquisition = true;
    index_t=(int)((curTime-discharge_begin)/step);

    if(prev_index != index_t && abs((Id[index_t] - Id[prev_index])*pow(10,-3)) > 8) {
      fullError = 0;
    }

    prev_index = index_t;

    if(index_t >= index_t_total) {
      switchToHold(TEST_END);
    } else {
      discharge_battery((Id[index_t]*pow(10,-3)));
    }
    break;
  case HOLD_CHARGE_MODE:
    aquisition = false;
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
