#include <PWM.h>

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CHARGE_MODE 1
#define DISCHARGE_MODE 2
#define HOLD_CHARGE_MODE 3
#define TEST_END 4

#define CONSTANT_CURRENT 1
#define STEP_DISCHARGE 2
#define HOLD_TIME 1000 // Time to hold in HOLD_CHARGE_MODE

const int vBatPin = A0;        // Pin for the battery voltage
const int shuntHighPin = A1;   // Pin for shunt +
const int shuntLowPin = A2;    // Pin for shunt -
const int vResPin = A4;        // Pin for the voltage across the resistances
const int pwmPin = 3;          // Pin for the PWM signal
const int chargePin = 4;       // Pin for the charge relay
const int aquisitionPin = 5;   // Pin for telling aquistion to begin
const int tempStabPin = 6;     // Pin indicating temperature stabilization
const int tempCutPin = 7;      // Pin indicating temperature cutOff
const float resValue = 0.2;    // Value of a resistor
const int numRes = 4;          // Number of resistors
const float controlDelay = .1; // Delay in milliseconds for control (tested at .1)
const float Vmax = 5.0;        // High level output voltage of PWM _ 3.25 before

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
void setAquisition(boolean val);

#endif
