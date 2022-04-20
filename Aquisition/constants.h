#ifndef CONSTANTS_H
#define CONSTANTS_H

const int vBatPin = A15;                    // Pin for the battery voltage
const int shuntHighPin = A14;               // Pin for shunt +
const int shuntLowPin = A13;                // Pin for shunt -
const int LM35Pin = A12;                    // Pin for the temperature sensor
const int vResPin = A11;                    // Pin for the voltage across the resistances
const int aquisitionPin = 22;               // Pin for reading the aquisition signal from the control microcontroller
const int temperatureStabilizationPin = 23; // Pin for telling the control microcontroller that temperature has stabilized
const int temperatureSoftCutPin = 24;       // Pin for telling the control microcontroller that temperature is too high
const int cutOffPins[2] = {33, 32};         // Pins for the emergency cut-off
const float resValue = 0.2;                 // Value of a resistor
const int numRes = 4;                       // Number of resistors
const float aquisitionDelay = 500;          // Delay in milliseconds for aquisition (tested to .1)

#endif
