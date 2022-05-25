#include "constants.h"

// Get external values
extern double shuntCur, vBat, LM35Temp;
extern float chargedCur, chargedVBat;
extern int curDischarge, control_mode, charge_counter, curTest, next_mode;
extern unsigned long holdChargeMillis;
extern boolean aquisition, tempCut;

// Function to charge the battery, takes as an argument a counter to check for the current
// passing throught the shunt and voltage of battery.
// Was needed because it takes a few iterations for the charging to begin
void charge_battery(int counter)
{
  digitalWrite(chargePin, HIGH); // Set the charge relay to high
  Serial.println("Charging battery, current voltage: " + String(vBat) + "V, current charge current: " + String(shuntCur) + "A");
  // If charged switch to hold and exepect a next discharge
  if ((shuntCur < chargedCur || vBat > chargedVBat) && counter % 50 == 0)
  {
    Serial.println("Done charging, will hold for voltage and temperature stabilization for " + String((int)((int)HOLD_TIME / (1000))) + " seconds.");
    switchToHold(DISCHARGE_MODE);
  }
}

// Funtion to discharge the battery at a constant current,
// takes as an argument the current to discharge.
void discharge_battery_constant(float cur)
{
  // If battery isn't fully discharged and temperature is bellow 75º keep controlling the current
  if (vBat >= 2.50 && tempCut == false)
  {
    controlCurrent(cur * (resValue / numRes));
  }
  else
  {
    // If all tests have been run hold the charge and after that end the tests
    if (curDischarge >= NUM_CURRENTS - 1)
    {
      Serial.println("Finished all " + String(NUM_CURRENTS) + " tests, current discharge was " + String(curDischarge) + ".");
      switchToHold(TEST_END);
    }
    else
    {
      Serial.println("Done discharging, will begin the next discharge after charging.");
      Serial.println("Final Voltage: " + String(vBat));
      curDischarge++;
      switchToHold(CHARGE_MODE);
    }
  }
}

// Function that is called in control loop to run the constant discharge test
void constant_current()
{
  // Choose what to do based on the control_mode
  switch (control_mode)
  {
  // Runs when charging
  case CHARGE_MODE:
    digitalWrite(mosfetRelayPin, LOW);
    pwmWrite(pwmPin, 0);
    setAquisition(false);           // Don't send data to the serial monitor
    charge_battery(charge_counter); // Charge the battery
    charge_counter++;
    break;

  // Runs when discharging the battry
  case DISCHARGE_MODE:
    digitalWrite(mosfetRelayPin, HIGH);
    setAquisition(true);                                         // Send data to the serial monitor
    discharge_battery_constant(dischargeCurrents[curDischarge]); // Keep the discharge current constant at the current test
    break;

  // Runs while holding a charge
  case HOLD_CHARGE_MODE:
    digitalWrite(mosfetRelayPin, LOW);
    pwmWrite(pwmPin, 0);
    setAquisition(false);   //  Don't send data to the serial monitor
    hold_charge(next_mode); // Hold the charge and after that change to next_mode
    break;

  // Runs in case an unfamiliar mode, or TEST_END
  default:
    digitalWrite(mosfetRelayPin, LOW);
    setAquisition(false);          // Don't send data to the serial monitor
    digitalWrite(chargePin, HIGH); // Close the charge relay
    pwmWrite(pwmPin, 0);           // Write a 0% duty cycle to the PWM
    break;
  }
}
