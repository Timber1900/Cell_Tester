#include "constants.h"

double resCur = 0;   // Current passing in the resistors
double vBat = 0;     // Battery voltage
double vRes = 0;     // Voltage across the resistors
double shuntCur = 0; // Voltage across the charge shunt
double LM35Temp = 0; // Temperature read by the lm35 sensor

unsigned long curTime;            // Current time in milliseconds since the arduino started
unsigned long prevTimeAquisition; // Previous time that the aquisition loop ran
double ambTemp = 26.0;            // Ambient temperature

boolean aquisition = false, prevAq = false; // Flag to send aquisition data to the serial monitor

void setup()
{
  Serial.begin(9600); // Initialize the serial
  Serial.println("Initialized... v05.0 aquisition!");

  pinMode(cutOffPins[0], OUTPUT);
  pinMode(cutOffPins[1], OUTPUT);
  pinMode(temperatureSoftCutPin, OUTPUT);
  pinMode(temperatureStabilizationPin, OUTPUT);
  pinMode(aquisitionPin, INPUT);
  digitalWrite(cutOffPins[0], LOW);
  digitalWrite(cutOffPins[1], LOW);
}

void loop()
{
  // Update the current time
  curTime = millis();

  // Read the analog values in the pins and calculate currents
  vRes = readPinValue(vResPin);
  vBat = readPinValue(vBatPin);
  resCur = vRes / (resValue / numRes);
  shuntCur = (readPinValue(shuntHighPin) - readPinValue(shuntLowPin)) / (0.172);
  LM35Temp = readPinValue(LM35Pin) * 100;

  aquisition = digitalRead(aquisitionPin) == HIGH;

  // Emergency test for current
  if (resCur > 45)
  {
    emergencyCutOff();
    while (1)
    {
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
      Serial.println("EMERGENCY STOP!!! \n\rTEMPERATURE > 80º \n\rDISCONNECT CELL, RESET");
      delay(10000);
    }
  }

  if (LM35Temp < ambTemp)
  {
    digitalWrite(temperatureStabilizationPin, HIGH);
  }
  else if (LM35Temp > 75)
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
      Serial.println(String(curTime / 1000.0, 3) + "," + String(vBat, 3) + "," + String(resCur, 3) + "," + String(LM35Temp, 2));
    }
    else if (!aquisition && prevAq)
    {
      Serial.println("End of discharge.");
    }
  }

  prevAq = aquisition;
}

// Helper function to read a pin value
// Takes as an argument the pin to read and returs it's value
float readPinValue(int pin)
{
  // Perform the read 20 times and average it out for better results
  float temp = 0;
  for (int i = 0; i < 20; i++)
  {
    temp += (float)analogRead(pin);
  }
  temp = (5.0 * temp) / (1024.0 * 20.0);
  return temp;
}

// Helper funtion to open the emergency relay
void emergencyCutOff()
{
  digitalWrite(cutOffPins[0], HIGH); // sets the digital pin 13 on
  digitalWrite(cutOffPins[1], HIGH); // sets the digital pin 13 on
}
