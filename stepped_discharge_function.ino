#include "constants.h"

// Get external values
extern double shuntCur, vBat, resCur;
extern unsigned long prevTimeControl, curTime;
extern int curDischarge, control_mode, charge_counter, curTest, next_mode, numTests;
extern float chargedCur, chargedVBat;
extern unsigned long holdChargeMillis;
extern boolean aquisition;

unsigned long discharge_begin; // Time in milliseconds that the discharge began
float soc = 0;                 // Soc value that was used in discharge

// Function that charges the battery the amount that was discharged
// Takes as an argument the pointer of the SOC value
void charge_battery_step(float *curSoc)
{
  digitalWrite(chargePin, HIGH); // Start

  // If the value of curSoc is greater than 0 then the cell is charged
  if (*curSoc >= 0)
  {
    switchToHold(DISCHARGE_MODE);
  }

  *curSoc += shuntCur * (curTime - prevTimeControl); // Update the value of curSoc
}

// Funtion the discharge the battery in the stepped discharge test
// Takes as an argument the current at wich to dischage, the time to discharge and a pointer to the SOC value
void discharge_battery_step(float cur, int discharge_milliseconds, float *curSoc)
{
  // If the voltage in the cell is greater than 2.5, and the time since the discharge began is less then the time to discharge
  // control the current and update the SOC value
  if (vBat >= 2.5 && (int)(curTime - discharge_begin) <= discharge_milliseconds)
  {
    controlCurrent(cur * (resValue / numRes));
    *curSoc -= resCur * (curTime - prevTimeControl);
  }
  else
  {
    // If the battery voltage is less than 2.5 consider the test over
    if (vBat < 2.5)
    {
      // Infinite loop to end the test
      while (1)
      {
        aquisition = false;
        Serial.println("End of stepped discharge test. Please reset arduino and remove cell."); // Warn user in serial monitor
        digitalWrite(chargePin, LOW);                                                           // Close the charge relay
        pwmWrite(pwmPin, 0);                                                                    // Write a 0% duty cycle to the PWM
        delay(10000);                                                                           // Wait ten seconds and run again
      }
    }
    else if (curDischarge >= numTests - 1)
    {
      switchToHold(TEST_END); // Change to TEST_END if we've run all tests
    }
    else
    {
      curDischarge++;
      switchToHold(CHARGE_MODE); // Recharge the cell for the next test
    }
  }
}

// Function that is called in control loop to run the stepped discharge test
void stepped_discharge(float cur, int discharge_milliseconds)
{
  // Choose what to do based on the control_mode
  switch (control_mode)
  {
  // Runs when charging
  case CHARGE_MODE:
    aquisition = true;         // Send data to the serial monitor
    charge_battery_step(&soc); // Charge the battery
    break;

  // Runs when discharging the battry
  case DISCHARGE_MODE:
    aquisition = true;                                         // Send data to the serial monitor
    discharge_battery_step(cur, discharge_milliseconds, &soc); // Discharge the battery for a certain time
    break;

  // Runs while holding a charge
  case HOLD_CHARGE_MODE:
    aquisition = true;         // Send data to the serial monitor
    discharge_begin = curTime; // Update the discharge begin value in preparation for the next discharge
    hold_charge(next_mode);    // Hold the charge
    break;

  // Runs at the end of a test
  case TEST_END:
    aquisition = false;                     // Don't send data to the serial monitor
    Serial.println("End of test.\r\n\r\n"); // Warn the user of a test end
    soc = 0;                                // Reset the soc value
    curDischarge = 0;                       // Reset the curDischarge value
    control_mode = CHARGE_MODE;             // Reset the control mode
    digitalWrite(chargePin, LOW);           // Close the charge relay
    pwmWrite(pwmPin, 0);                    // Write a 0% duty cycle to the PWM
    break;

  // Runs in case an unfamiliar mode
  default:
    aquisition = false;           // Don't send data to the serial monitor
    digitalWrite(chargePin, LOW); // Close the charge relay
    pwmWrite(pwmPin, 0);          // Write a 0% duty cycle to the PWM
    break;
  }
}
