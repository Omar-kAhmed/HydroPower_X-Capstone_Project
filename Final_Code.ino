#include <SoftwareSerial.h>
SoftwareSerial BTserial(10, 11); // RX | TX

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
hd44780_I2Cexp lcd;

//Flowrate
int FLInterrupt = 1;
int FLPin = 3;
float calibrationFactor = 4.5;
volatile byte pulseCount;  
float flowRate;
long oldTime;
void pulseCounter()
{
pulseCount++;
}

//RPM
int RPMInterrupt = 0;
float rev=0;
int rpm;
int oldtime=0;
int time;
void revCounter()
{
rev++;
}

byte pin13 = 13;
byte pin12 = 12;
byte pin8 = 8;

void setup() {
  
  BTserial.begin(9600);

  //Flowrate
  pinMode(FLPin, INPUT);
  digitalWrite(FLPin, HIGH);
  pulseCount = 0;
  flowRate = 0.0;
  oldTime = 0;
  attachInterrupt(FLInterrupt, pulseCounter, FALLING);

  //RPM
  attachInterrupt(RPMInterrupt, revCounter, RISING);

  //LCD
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Welcome to Group");
  lcd.setCursor(6,1);
  lcd.print("334");
  delay(5000);

  //LEDs
  pinMode(pin13, OUTPUT);
  pinMode(pin12, OUTPUT);

  //Buzzer
  pinMode(pin8, OUTPUT);
  digitalWrite(pin8, HIGH);
  delay(300);
  digitalWrite(pin8, LOW);
  delay(300);
  digitalWrite(pin8, HIGH);
  delay(300);
  digitalWrite(pin8, LOW);
  delay(300);
  digitalWrite(pin8, HIGH);
  delay(300);
  digitalWrite(pin8, LOW);
  delay(300);
}

void loop() {
  
//Flowrate
  if((millis() - oldTime) > 1000)    // Only process counters once per second
    { 
      detachInterrupt(FLInterrupt);
      flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
      oldTime = millis();
      pulseCount = 0;
      attachInterrupt(FLInterrupt, pulseCounter, FALLING);
    }

//RPM
  delay(1000);
  detachInterrupt(RPMInterrupt);           //detaches the interrupt
  time=millis()-oldtime;        //finds the time 
  rpm=(rev/time)*60000;         //calculates rpm
  oldtime=millis();             //saves the current time
  rev=0;
  attachInterrupt(RPMInterrupt,revCounter,RISING);

//LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Flowrate:");
  lcd.setCursor(9, 0);
  lcd.print(flowRate);
  lcd.setCursor(13, 0);
  lcd.print("L/M");

  lcd.setCursor(0,1);
  lcd.print("Speed: ");
  lcd.setCursor(8,1);
  lcd.print(rpm);
  lcd.print(" RPM");

//Bluetooth
  BTserial.print(float(flowRate)); 
  BTserial.print(",");
  BTserial.print(rpm); 
  BTserial.print(";");

//LEDs
  char value = BTserial.read();
  if (int(flowRate) < 15 || value == "0") {
    digitalWrite(pin12, HIGH);
    digitalWrite(pin13, LOW); }

  else if (int(flowRate >= 15) || value == "1") {
    digitalWrite(pin12, LOW);
    digitalWrite(pin13, HIGH); }
}
