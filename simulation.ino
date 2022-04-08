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
const int index_t_total =sizeof(Id);

extern boolean aquisition;
extern int control_mode, charge_counter, next_mode;
unsigned long discharge_begin;

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
    controlCurrent(Id[index_t] * (resValue / numRes));
    break;
    case HOLD_CHARGE_MODE:
    aquisition = true;
    discharge_begin = curTime;
    hold_charge(next_mode);
    break;
  case TEST_END:
    aquisition = false;
    Serial.println("End of test.\r\n\r\n");
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
