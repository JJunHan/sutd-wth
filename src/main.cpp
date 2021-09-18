#include <Arduino.h>

const int ledPin = 23;
const int irPin = 4;
int IRsensorvalue;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(irPin, OUTPUT);
}

void loop() {
  IRsensorvalue = analogRead(irPin);
  if(IRsensorvalue < 200) {
    digitalWrite(ledPin, HIGH);
    delay(1000); 
  }
  else{
    digitalWrite(ledPin, LOW);
  }
  
  // float voltage = sensorvalue * (3.3 / 4096.0);
  // Serial.println(sensorvalue);
  // Serial.println(voltage);
  delay(500);
}