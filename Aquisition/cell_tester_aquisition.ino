#include "constants.h"
#include <PWM.h>

double resCur = 0;                // Current passing in the resistors
double vBat = 0;                  // Battery voltage
double vRes = 0;                  // Voltage across the resistors
double shuntCur = 0;              // Voltage across the charge shunt
double LM35Temp = 0;              // Temperature read by the lm35 sensor
double vCC = 5.00;                // Vcc, 5.10 for desktop
unsigned long curTime;            // Current time in milliseconds since the arduino started
unsigned long prevTimeAquisition; // Previous time that the aquisition loop ran
unsigned long prevTimeControl;    // Previous time that the aquisition loop ran
double ambTemp = 26.0;            // Ambient temperature
float ref = 0;
float curError = 0;
float fullError = 0;
float targetTemp = 40;
int inext;
float Kp = 1000;     // PID Proporcional constant
float Ki = 400;      // PID Integral constant
float Kpdc = Kp / 5; // PID Internal proporcional constant

boolean aquisition = false, prevAq = false; // Flag to send aquisition data to the serial monitor

void setup()
{
  Serial.begin(57600); // Initialize the serial
  Serial.println("Initialized... v05.0 aquisition!");

  pinMode(cutOffPins[0], OUTPUT);
  pinMode(cutOffPins[1], OUTPUT);
  pinMode(temperatureSoftCutPin, OUTPUT);
  pinMode(temperatureStabilizationPin, OUTPUT);
  pinMode(aquisitionPin, INPUT);
  digitalWrite(cutOffPins[0], LOW);
  digitalWrite(cutOffPins[1], LOW);

  pinMode(pwmPin, OUTPUT);
  pinMode(polarityPin, OUTPUT);
  InitTimersSafe();
  SetPinFrequencySafe(pwmPin, 20000);
  pwmWrite(pwmPin, 0);
  digitalWrite(polarityPin, LOW);
}

void loop()
{
  // Update the current time
  curTime = millis();
  ref = getRef();
  // Read the analog values in the pins and calculate currents
  vRes = readPinValue(vResPin, ref);
  vBat = readPinValue(vBatPin, ref);
  resCur = vRes / (resValue / numRes);
  shuntCur = (readPinValue(shuntHighPin, ref) - readPinValue(shuntLowPin, ref)) / (0.333 / 2);
  LM35Temp = readPinValue(LM35Pin, ref) * 100;
  ambTemp = readPinValue(ambTempPin, ref) * 100;
  aquisition = digitalRead(aquisitionPin) == HIGH;

  // Emergency test for current
  if (resCur > 45)
  {
    emergencyCutOff();
    while (1)
    {
      Serial.println(String(curTime / 1000.0, 3) + "," + String(vBat, 3) + "," + String(resCur, 3) + "," + String(LM35Temp, 2) + "," + String(ambTemp, 2) + "," + String(shuntCur, 3));

      Serial.println("EMERGENCY STOP!!! \n\rCURRENT > 45A \n\rDISCONNECT CELL, RESET");
      delay(10000);
    }
  }

  // Emergency test for temperature
  if (LM35Temp > 80)
  {
    emergencyCutOff();
    while (1)
    {
      Serial.println(String(curTime / 1000.0, 3) + "," + String(vBat, 3) + "," + String(resCur, 3) + "," + String(LM35Temp, 2) + "," + String(ambTemp, 2) + "," + String(shuntCur, 3));

      Serial.println("EMERGENCY STOP!!! \n\rTEMPERATURE > 80º \n\rDISCONNECT CELL, RESET");
      delay(10000);
    }
  }

  if (abs(ambTemp - targetTemp) < 1)
  {
    digitalWrite(temperatureStabilizationPin, HIGH);
  }

  if (LM35Temp > 75)
  {
    digitalWrite(temperatureSoftCutPin, HIGH);
  }

  // See if the delay time has passed since last aquisition loop
  if (curTime - prevTimeAquisition >= aquisitionDelay)
  {
    prevTimeAquisition = curTime;
    // Aquisition loop
    if (aquisition)
    {
      Serial.println(String(curTime / 1000.0, 3) + "," + String(vBat, 3) + "," + String(resCur, 3) + "," + String(LM35Temp, 2) + "," + String(ambTemp, 2) + "," + String(shuntCur, 3));
    }
    else if (!aquisition && prevAq)
    {
      Serial.println("End of discharge.");
    }

    prevAq = aquisition;
  }

  // See if the delay time has passed since last aquisition loop
  if (curTime - prevTimeControl >= controlDelay)
  {
    prevTimeControl = curTime;

    curError = targetTemp - ambTemp;                                                                        // Calculate the current error
    inext = ceil((Ki * (curError + fullError) * ((curTime - prevTimeControl) / 1000.0)) + curError * Kpdc); // PID control for the duty cycle

    digitalWrite(polarityPin, inext < 0 ? HIGH : LOW);
    if (inext < 0)
      inext = -inext;
    if (inext > 255)
      inext = 255;
    pwmWrite(pwmPin, inext); // Write the duty cycle to the PWM
    fullError += curError;   // Increase the full error
  }
}

float getRef()
{
  // Perform the read 20 times and average it out for better results
  float temp = 0;
  for (int i = 0; i < 10; i++)
  {
    temp += (float)analogRead(v5V);
  }
  temp = (temp) / (10.0);
  return temp;
}

// Helper function to read a pin value
// Takes as an argument the pin to read and returs it's value
float readPinValue(int pin, float ref)
{
  // Perform the read 20 times and average it out for better results
  float temp = 0;
  for (int i = 0; i < 10; i++)
  {
    temp += (float)analogRead(pin);
  }
  temp = (5.02 * temp) / (ref * 10.0);
  return temp;
}

// Helper funtion to open the emergency relay
void emergencyCutOff()
{
  digitalWrite(cutOffPins[0], HIGH); // sets the digital pin 13 on
  digitalWrite(cutOffPins[1], HIGH); // sets the digital pin 13 on
}
