#include <PWM.h>

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CHARGE_MODE 1
#define DISCHARGE_MODE 2
#define HOLD_CHARGE_MODE 3
#define TEST_END 4

#define CONSTANT_CURRENT 1
#define STEP_DISCHARGE 2
#define HOLD_TIME 180000 // Time to hold in HOLD_CHARGE_MODE

const int vBatPin = A15;            // Pin for the battery voltage
const int shuntHighPin = A14;       // Pin for shunt +
const int shuntLowPin = A13;        // Pin for shunt -
const int LM35Pin = A12;            // Pin for the temperature sensor
const int vResPin = A11;            // Pin for the voltage across the resistances
const int pwmPin = 12;              // Pin for the PWM signal
const int chargePin = 30;           // Pin for the charge relay
const int cutOffPins[2] = {33, 32}; // Pins for the emergency cut-off
const float resValue = 0.2;         // Value of a resistor
const int numRes = 4;               // Number of resistors
const float aquisitionDelay = 0.1;  // Delay in milliseconds for aquisition (tested to .1)
const float controlDelay = 0.1;     // Delay in milliseconds for control (tested at .1)
const float Vmax = 5.0;             // High level output voltage of PWM _ 3.25 before

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

#endif
