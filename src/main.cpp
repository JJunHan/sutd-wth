#include <Arduino.h>

#define LEDPIN 23
#define IRPIN 4
#define TRIGPIN 5
#define ECHOPIN 18
#define SOUND_SPEED 0.034 // in micro secs

int IRsensorvalue;
long duration;
float distanceCm;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(IRPIN, OUTPUT);
  pinMode(TRIGPIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHOPIN, INPUT); // Sets the echoPin as an Input
}

void loop() {
  IRsensorvalue = analogRead(IRPIN);
  if(IRsensorvalue < 200) {
    digitalWrite(LEDPIN, HIGH);
    //delay(1000); 
  }
  else{
    digitalWrite(LEDPIN, LOW);
  }

  // Clears the trigPin
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHOPIN, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  
  //float voltage = IRsensorvalue * (3.3 / 4096.0);
  //Serial.println(IRsensorvalue);
  //Serial.println(voltage);
  //Serial.println(duration);
  delay(500);
}