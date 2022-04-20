// This is the project to handle the control of the discharges in the cell tester,
// The aquisition of data is handled by a second arduino to reduce errors and increase accuracy.

#include "constants.h"

unsigned long curTime;            // Current time in milliseconds since the arduino started
unsigned long prevTimeAquisition; // Previous time that the aquisition loop ran
unsigned long prevTimeControl;    // Previous time that the control loop ran
double resCur = 0;                // Current passing in the resistors
double vBat = 0;                  // Battery voltage
double vRes = 0;                  // Voltage across the resistors
double shuntCur = 0;              // Voltage across the charge shunt
double curError = 0;              // Current PID error
double fullError = 0;             // Total PID error
double ambTemp = 26.0;            // Ambient temperature
double inext;                     // Value of next PWM duty cycle
float Kp = 510;                   // PID Proporcional constant
float Ki = 340;                   // PID Integral constant
float Kpdc = Kp / Vmax;           // PID Internal proporcional constant
float chargedCur = 0.08;          // Current where we consider the cell charged
float chargedVBat = 4.21;         // Voltage where we consider the cell charge

int curTest = CONSTANT_CURRENT; // Current test to be ran
int control_mode = CHARGE_MODE; // Initial control mode
int charge_counter = 1;         // Counter for the charge sequence
int numTests = 4;               // Number of tests to run in a stepped test

unsigned long holdChargeMillis; // Time to hold charge
unsigned long initalHoldMillis; // Initial time where charge holding began
int next_mode;                  // Next mode after holding charge

boolean tempStable = false; // Indicates if the temperature is stable
boolean tempCut = false;    // Indicates if temperature is too high

float dischargeCurrents[3] = {10, 15, 20}; // Array of currents to do a full constant discharge, the theoretical minimum is 0.26A
int curDischarge = 0;                      // Current discharge being executed

void setup()
{
  Serial.begin(9600); // Initialize the serial
  Serial.println("Initialized... v05.0");
  Serial.println("This is a test to calculate resistance of lithium ion cells on a stepped discharge");
  Serial.println("To start press any key..");

  // Set pin modes and initial values for the charge and emergency pins
  pinMode(chargePin, OUTPUT);
  pinMode(aquisitionPin, OUTPUT);
  pinMode(tempCutPin, INPUT);
  pinMode(tempStabPin, INPUT);
  digitalWrite(chargePin, LOW);

  // Initialize the PWM timers
  InitTimersSafe();

  // Try to set the pwm frequency
  boolean test = SetPinFrequencySafe(pwmPin, 20000);
  if (!test)
  {
    Serial.println("Error setting pin frequency");
    while (1)
    {
    };
  }

  // Initialize the PWM with a 0% duty cycle
  pwmWrite(pwmPin, 0);

  // Wait for user input
  while (!Serial.available())
  {
  };

  Serial.println("Starting test in...3");
  delay(1000);
  Serial.println("...2");
  delay(1000);
  Serial.println("...1");
  delay(1000);
}

void loop()
{
  // Update the current time
  curTime = millis();

  // Read the analog values in the pins and calculate currents
  vRes = readPinValue(vResPin);
  vBat = readPinValue(vBatPin);

  if (vRes != 0)
    vRes += 0.013;

  if (vBat != 0)
    vBat += 0.013;

  resCur = vRes / (resValue / numRes);
  shuntCur = (readPinValue(shuntHighPin) - readPinValue(shuntLowPin)) / (0.172);

  tempStable = digitalRead(tempStabPin) == HIGH;
  tempCut = digitalRead(tempCutPin) == HIGH;

  // See if the delay time has passed since last control loop
  if (curTime - prevTimeControl >= controlDelay)
  {
    // Control loop
    switch (curTest)
    {
    case CONSTANT_CURRENT:
      constant_current();
      break;
    case STEP_DISCHARGE:
      stepped_discharge(10, 30 * 1000);
    default:
      break;
    }
    prevTimeControl = curTime;
  }
}

// Funtion to switch the control mode to hold discharge
// Takes as an argument the next mode the controller should take after waiting for a set time
void switchToHold(int next)
{
  if (next == DISCHARGE_MODE)
    Serial.println("About to begin a discharge, cell voltage is " + String(vBat) + " V");
  initalHoldMillis = millis();     // Time that we began holding
  control_mode = HOLD_CHARGE_MODE; // Change the control mode
  next_mode = next;                // Store the next control mode
  fullError = 0;                   // Reset the PID error
}
