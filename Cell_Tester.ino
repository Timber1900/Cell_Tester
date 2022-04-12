#include "constants.h"

unsigned long curTime;
unsigned long prevTimeAquisition;
unsigned long prevTimeControl;
double resCur = 0;
double vBat = 0;
double vRes = 0;
double shuntCur = 0;
double LM35Temp = 0;
double curError = 0;
double fullError = 0;
double ambTemp = 26.0;
double inext;
float Kp = 275;
float Ki = 800;
float Kpdc = Kp / Vmax;
float chargedCur = 0.2;
float chargedVBat = 4.21;

boolean aquisition = false;

int curTest = SIMULATION;

int control_mode = CHARGE_MODE;
int charge_counter = 1;
int numTests = 2;

unsigned long holdChargeMillis;
unsigned long initalHoldMillis;
int next_mode;

int dischargeCurrents[1] = {20};
int curDischarge = 0;

extern int index_t;
extern int Id[];

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Initialized... v05.0");
  Serial.println("This is a test to calculate resistance of lithium ion cells on a stepped discharge");
  Serial.println("To start press any key..");
  pinMode(chargePin, OUTPUT);
  pinMode(cutOffPins[0], OUTPUT);
  pinMode(cutOffPins[1], OUTPUT);
  digitalWrite(cutOffPins[0], LOW);
  digitalWrite(cutOffPins[1], LOW);
  digitalWrite(chargePin, LOW);

  InitTimersSafe();
  boolean test = SetPinFrequencySafe(pwmPin, 20000);
  if (!test)
  {
    Serial.println("Error setting pin frequency");
    while (1)
    {
    };
  }
  pwmWrite(pwmPin, 0);

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
  curTime = millis();
  vRes = readPinValue(vResPin);
  resCur = vRes / (resValue / numRes);
  vBat = readPinValue(vBatPin);
  shuntCur = (readPinValue(shuntHighPin) - readPinValue(shuntLowPin)) / (0.172);
  LM35Temp = readPinValue(LM35Pin) * 100;

  if (resCur > 45)
  {
    emergencyCutOff();
    while (1)
    {
      Serial.println("EMERGENCY STOP!!! \n\rCURRENT > 45A \n\rDISCONNECT CELL, RESET");
      delay(10000);
    }
  }

  if (LM35Temp > 80)
  {
    emergencyCutOff();
    while (1)
    {
      Serial.println("EMERGENCY STOP!!! \n\rTEMPERATURE > 80º \n\rDISCONNECT CELL, RESET");
      delay(10000);
    }
  }

  if (curTime - prevTimeAquisition >= aquisitionDelay)
  {
    prevTimeAquisition = curTime;
    // Loop de aquisição
    if (aquisition)
    {
      Serial.println(String(curTime / 1000.0, 3) + "," + String(vBat, 3) + "," + String(resCur, 3) + "," + String(LM35Temp, 2));
    }
  }

  if (curTime - prevTimeControl >= controlDelay)
  {
    // Loop de controlo
    switch (curTest)
    {
    case CONSTANT_CURRENT:
      constant_current();
      break;
    case STEP_DISCHARGE:
      stepped_discharge(10, 30 * 1000);
    case SIMULATION:
      corrente_ref();
    default:
      break;
    }
    prevTimeControl = curTime;
  }
}

void switchToHold(int next)
{
  initalHoldMillis = millis();
  control_mode = HOLD_CHARGE_MODE;
  next_mode = next;
  fullError = 0;
}
