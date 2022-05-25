#include "constants.h"

// Get external values
extern int control_mode, curTest, curDischarge;
extern unsigned long curTime, prevTimeControl, initalHoldMillis, discharge_begin;
extern double LM35Temp, ambTemp, curError, fullError, vRes, inext, vBat;
extern float Kpdc, Ki;
extern boolean tempStable;

// Function to hold a charge for a certain time and after the change the control mode
// Takes as an argument the next task to run
void hold_charge(int nextTask)
{
  digitalWrite(chargePin, LOW); // Close the charge relay
  pwmWrite(pwmPin, 0);          // Write a 0% duty cycle to the PWM

  // Runs is the hold time is greater the the time to hold and the temperature is stable
  if (curTime - initalHoldMillis > HOLD_TIME && tempStable)
  {
    // If running a constant current test and where about to charge the battery print "End of discharge."
    if (nextTask == CHARGE_MODE && curTest == CONSTANT_CURRENT)
      Serial.println("End of discharge.");
    if (nextTask == DISCHARGE_MODE)
    {
      Serial.println("Starting discharge, current voltage is " + String(vBat) + "V, target discharge current is " + String(dischargeCurrents[curDischarge]) + "A.");
      setMosfetRelay(HIGH);
    }
    // Change the control mode
    control_mode = nextTask;
  }
}

float getRef()
{
  // Perform the read 20 times and average it out for better results
  float temp = 0;
  for (int i = 0; i < 2; i++)
  {
    temp += (float)analogRead(v5V);
  }
  temp = (temp) / (2.0);
  return temp;
}

// Helper function to read a pin value
// Takes as an argument the pin to read and returs it's value
float readPinValue(int pin, float ref)
{
  // Perform the read 20 times and average it out for better results
  float temp = 0;
  for (int i = 0; i < 2; i++)
  {
    temp += (float)analogRead(pin);
  }
  temp = (5.02 * temp) / (ref * 2.0);
  return temp;
}

// Helper funtion to control the current in the resistors
// Takes as an argument the target voltage
void controlCurrent(float vTarget)
{
  curError = vTarget - vRes;                                                                              // Calculate the current error
  inext = ceil((Ki * (curError + fullError) * ((curTime - prevTimeControl) / 1000.0)) + curError * Kpdc); // PID control for the duty cycle
  if (inext < 0)
    inext = 0;
  pwmWrite(pwmPin, inext); // Write the duty cycle to the PWM
  fullError += curError;   // Increase the full error
}

void setAquisition(boolean val)
{
  digitalWrite(aquisitionPin, val ? HIGH : LOW);
}

void setMosfetRelay(int val)
{
  pwmWrite(pwmPin, 0);
  digitalWrite(mosfetRelayPin, val);
  setAquisition(true);
  delay(5000);
  discharge_begin = curTime; // Update the discharge begin value in preparation for the next discharge
}
