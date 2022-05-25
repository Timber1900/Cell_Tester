#ifndef CONSTANTS_H
#define CONSTANTS_H

const int vBatPin = A15;      // Pin for the battery voltage
const int shuntHighPin = A12; // Pin for shunt +
const int shuntLowPin = A13;  // Pin for shunt -
const int LM35Pin = A14;      // Pin for the temperature sensor
const int ambTempPin = A7;    // Pin for the temperature sensor of ambient temperature
const int vResPin = A11;      // Pin for the voltage across the resistances
const int v5V = A10;
const int aquisitionPin = 22;               // Pin for reading the aquisition signal from the control microcontroller
const int temperatureStabilizationPin = 26; // Pin for telling the control microcontroller that temperature has stabilized
const int temperatureSoftCutPin = 24;       // Pin for telling the control microcontroller that temperature is too high
const int pwmPin = 12;
const int polarityPin = 13;
const int cutOffPins[2] = {33, 32}; // Pins for the emergency cut-off
const float resValue = 0.2;         // Value of a resistor
const int numRes = 4;               // Number of resistors
const float aquisitionDelay = 1;    // Delay in milliseconds for aquisition (tested to .1)
const float controlDelay = 1000;
#endif
