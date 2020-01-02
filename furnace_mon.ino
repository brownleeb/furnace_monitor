#define aref_voltage 3.273

#define GAS 3
#define HEAT 0
#define FAN 2
#define COOL 1
#define FAN_CTRL 5

int garagePin = 1;
int returnTempPin = 2;
int supplyTempPin = 3;
int delay_tm = 500;
int cooldown = 800;
bool fanRunning = false;

bool gasOn = false;
bool heatOn = false;
bool fanOn = false;
bool coolOn = false;

String dv;

void setup(void)
{
  Serial.begin(9600);
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

int furnTemp()
{
  int  tempReading = analogRead(supplyTempPin);
  tempReading = analogRead(supplyTempPin);
  return (((tempReading - 31) * 57576) / 10000) - 400;
}

void readTemps()
{
  int tempReading = analogRead(garagePin);
  tempReading = analogRead(garagePin);
  Serial.print((((tempReading - 31) * 57576) / 10000) - 400);

  Serial.print(",");
  tempReading = analogRead(supplyTempPin);
  tempReading = analogRead(supplyTempPin);
  Serial.print((((tempReading - 31) * 57576) / 10000) - 400);

  Serial.print(",");
  tempReading = analogRead(returnTempPin);
  tempReading = analogRead(returnTempPin);
  Serial.print((((tempReading - 31) * 57576) / 10000) - 400);
  //  Serial.print((((((tempReading * 323535) - 50000000) / 100000) * 18) + 3200) / 10);
}

void loop(void)
{
  if (furnTemp() > cooldown)  {
    digitalWrite(FAN_CTRL, LOW);
  }
  else if (fanRunning == false) {
    digitalWrite(FAN_CTRL, HIGH);
  }
  if (Serial.available()) {
    int mode, LF;
    mode = Serial.read();
    switch (mode) {
      case 'P':
        readTemps();
        Serial.print(',');
        Serial.println(readInts());
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

void detectGAS()  {
  gasOn = true;
}
void detectHEAT()  {
  heatOn = true;
}
void detectCOOL()  {
  coolOn = true;
}
void detectFAN()  {
  fanOn = true;
}
