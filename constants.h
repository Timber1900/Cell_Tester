#include <PWM.h>

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CHARGE_MODE 1
#define DISCHARGE_MODE 2
#define HOLD_CHARGE_MODE 3
#define TEST_END 4

#define CONSTANT_CURRENT 1
#define STEP_DISCHARGE 2
#define SIMULATION 3
#define HOLD_TIME 180000

const int vBatPin = A15;
const int shuntHighPin = A14;
const int shuntLowPin = A13;
const int LM35Pin = A12;
const int vResPin = A11;
const int cutoffPin = 37;
const int pwmPin = 12;
const int chargePin = 30;
const int cutOffPins[2] = {33, 32};
const float resValue = 0.2;
const int numRes = 4;
const float aquisitionDelay = 10; // Delay in milliseconds
const float controlDelay = 0.1;
const float Vmax = 5.0; // High level output voltage of PWM _ 3.25 before

void switchToHold(int next);
void controlCurrent(float vTarget);
void charge_battery(int counter);
void charge_battery_step(float *curSoc);
void discharge_battery_constant(float cur);
void constant_current();
void emergencyCutOff();
void controlCurrent(float vTarget);
float readPinValue(int pin);
void hold_charge(int nextTask);
void stepped_discharge(float cur, int discharge_milliseconds);
void corrente_ref();
#endif
