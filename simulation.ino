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

void corrente_ref(long curTime, long aquisitionStart){
    if(((int)(curTime-aquisitionStart)%step==0) {
      index_t=curTime/step;
    }
    controlCurrent(Id[index_t] * (resValue / numRes));
}