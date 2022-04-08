#include "constants.h"

// Get external values
extern int control_mode, curTest;
extern unsigned long curTime, prevTimeControl, initalHoldMillis;
extern double LM35Temp, ambTemp, curError, fullError, vRes, inext;
extern float Kpdc, Ki;

// Function to hold a charge for a certain time and after the change the control mode
// Takes as an argument the next task to run
void hold_charge(int nextTask)
{
  digitalWrite(chargePin, LOW); // Close the charge relay
  pwmWrite(pwmPin, 0);          // Write a 0% duty cycle to the PWM

  // Runs is the hold time is greater the the time to hold and the temperature is stable
  if (curTime - initalHoldMillis > HOLD_TIME && LM35Temp < ambTemp)
  {
    // If running a constant current test and where about to charge the battery print "End of discharge."
    if (nextTask == CHARGE_MODE && curTest == CONSTANT_CURRENT)
      Serial.println("End of discharge.");

    // Change the control mode
    control_mode = nextTask;
  }
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

// Helper funtion to control the current in the resistors
// Takes as an argument the target voltage
void controlCurrent(float vTarget)
{
  curError = vTarget - vRes;                                                                              // Calculate the current error
  inext = ceil((Ki * (curError + fullError) * ((curTime - prevTimeControl) / 1000.0)) + curError * Kpdc); // PID control for the duty cycle
  pwmWrite(pwmPin, inext);                                                                                // Write the duty cycle to the PWM
  fullError += curError;                                                                                  // Increase the full error
}

// Helper funtion to open the emergency relay
void emergencyCutOff()
{
  digitalWrite(cutOffPins[0], HIGH); // sets the digital pin 13 on
  digitalWrite(cutOffPins[1], HIGH); // sets the digital pin 13 on
}
