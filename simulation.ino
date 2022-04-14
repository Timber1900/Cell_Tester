#include "constants.h"

#if TEST == 1 //Teste 1 realiza uma volta
  #include "corrente_celltester_1volta.h"
#elif TEST == 2 //Teste 2 realiza uma corrida 
   //#include "corrente_celltester_corrida.h" //Não da para usar. Não tem memoria
   //Correntes de referência divididas em 6 ficheiros por limitações de memória
  #if VOLTA == 1
    #include "corrente_celltester_corrida1.h"
  #elif VOLTA == 2
    #include "corrente_celltester_corrida2.h"
  #elif VOLTA == 3
    #include "corrente_celltester_corrida3.h"
  #elif VOLTA == 4
    #include "corrente_celltester_corrida4.h"
  #elif VOLTA == 5
    #include "corrente_celltester_corrida5.h"
  #elif VOLTA == 6
    #include "corrente_celltester_corrida6.h"
  #endif
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
  case CHARGE_MODE: //Carregamento da célula
    aquisition = false;
    charge_battery(charge_counter);
    charge_counter++;
    break;
  case DISCHARGE_MODE:  //Teste em si
    aquisition = true;
    index_t=(int)((curTime-discharge_begin)/step);  //Atualizar o index em função do tempo de simulação
    prev_index = index_t;
    if(prev_index != index_t && abs((Id[index_t] - Id[prev_index])*pow(10,-3)) > 8) { //Atualizar o erro
      fullError = 0;
    }
    if(index_t >= index_t_total) {//Para terminar o teste
      switchToHold(TEST_END);
    } else {//Corrente de referência
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
