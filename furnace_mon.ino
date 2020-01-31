#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 9
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int supplyTemp;

#define aref_voltage 3.273

#define GAS 3
#define HEAT 0
#define FAN 2
#define COOL 1
#define FAN_CTRL 5

int garagePin = 1;
int delay_tm = 500;
int cooldown = 3769;
int delta = 711;
bool fanRunning = false;

bool gasOn = false;
bool heatOn = false;
bool fanOn = false;
bool coolOn = false;

int gasPulses = 0;
int heatPulses = 0;
int fanPulses = 0;
int coolPulses = 0;

String dv;

uint8_t supplyTempAddr[8] = { 0x28, 0xAE, 0x8A, 0xE3, 0x08, 0x00, 0x00, 0xC3 };
uint8_t returnTempAddr[8] = { 0x28, 0xB7, 0x7B, 0xE3, 0x08, 0x00, 0x00, 0x65 };

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();  // Start up the library
  analogReference(EXTERNAL);
  pinMode(FAN_CTRL, OUTPUT);
  digitalWrite(FAN_CTRL, HIGH);

  pinMode(GAS, INPUT);
  pinMode(HEAT, INPUT);
  pinMode(FAN, INPUT);
  pinMode(COOL, INPUT);

  attachInterrupt(digitalPinToInterrupt(GAS), detectGAS, RISING);
  attachInterrupt(digitalPinToInterrupt(HEAT), detectHEAT, RISING);
  attachInterrupt(digitalPinToInterrupt(FAN), detectFAN, RISING);
  attachInterrupt(digitalPinToInterrupt(COOL), detectCOOL, RISING);
}

int systemOn()
{
  return gasOn or heatOn;
}

int readInts()
{
  int furn = 0;
  if (gasOn == true) {
    furn = furn + 8;
  }

  if (heatOn == true) {
    furn = furn + 4;
  }

  if (fanOn == true) {
    furn = furn + 2;
  }

  if (coolOn == true) {
    furn = furn + 1;
  }
  gasOn = fanOn = coolOn = heatOn = false;
  return furn;
}

int furnTemp(int pin)
{
  int  tempReading = analogRead(pin);
  tempReading = analogRead(pin);
  return (((tempReading - 31) * 57576) / 10000) - 400;
}

void loop(void)
{
  sensors.requestTemperatures();
  supplyTemp=sensors.getTemp(supplyTempAddr);
  if (( supplyTemp > cooldown ) and (systemOn() == false))  {
    digitalWrite(FAN_CTRL, LOW);
  }
  else if ((fanRunning == false) and (supplyTemp < (cooldown - delta))) {
    digitalWrite(FAN_CTRL, HIGH);
  }
  if (Serial.available()) {
    int mode, LF;
    mode = Serial.read();
    switch (mode) {
      case 'P':
        Serial.print(furnTemp(garagePin));
        Serial.print(",");
        Serial.print(supplyTemp);
        Serial.print(",");
        Serial.print(sensors.getTemp(returnTempAddr));
        Serial.print(",");
        Serial.print(readInts());
        Serial.print(',');
        Serial.print(gasPulses);
        Serial.print(',');
        Serial.print(heatPulses);
        Serial.print(',');
        Serial.print(fanPulses);
        Serial.print(',');
        Serial.print(coolPulses);
        gasPulses = heatPulses = coolPulses = fanPulses = 0;
        Serial.println();
        break;
      case'F':
        digitalWrite(FAN_CTRL, LOW);
        LF = Serial.read();
        fanRunning = true;
        break;
      case 'X':
        if (fanRunning == true) {
          digitalWrite(FAN_CTRL, HIGH);
          LF = Serial.read();
          fanRunning = false;
        }
        break;
      case'D':
        LF = Serial.read();
        dv = Serial.readStringUntil('\n');
        dv.trim();
        delay_tm = dv.toInt();
        break;
      case'C':
        LF = Serial.read();
        dv = Serial.readStringUntil('\n');
        dv.trim();
        cooldown = dv.toInt();
        break;
    }
  }
  delay(delay_tm);
}

void detectGAS()  {
  gasOn = true;
  gasPulses++;
}
void detectHEAT()  {
  heatOn = true;
  heatPulses++;
}
void detectCOOL()  {
  coolOn = true;
  coolPulses++;
}
void detectFAN()  {
  fanOn = true;
  fanPulses++;
}
