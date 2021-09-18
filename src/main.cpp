#include <Arduino.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define LEDPIN 23
#define IRPIN 34
#define TRIGPIN 5
#define ECHOPIN 18
#define SOUND_SPEED 0.034 // in micro secs

// Server Variables
const char* _ssid = "SINGTEL-BE9C (2.4G)";
const char* _password = "oopheishoh";
AsyncWebServer server(80);
String processors(const String& var);
StaticJsonDocument<200> doc;
AsyncEventSource events("/events");

int IRsensorvalue;
long duration;
float distanceCm;
int objcount = 0;

char objectcounter[128];
char filldepth[128];

unsigned long lastTime = 0; 
unsigned long lastTimeDelay = 500; 

String processors(const String& var){

  snprintf(objectcounter, sizeof(objectcounter), "%d ", objcount );
  snprintf(filldepth, sizeof(filldepth), "%f", distanceCm);
  
  if(var == "objectPlaceholder"){
    return objectcounter;
  }
  else if(var == "depthPlaceholder"){
    return filldepth;
  }
  return String();
}

void initWIFI(){
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
}

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Error mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

String getPackageData(){
  String holder;
  doc["objectcounter"] = String(objcount);
  doc["filldepth"] = String(distanceCm);
  serializeJson(doc, holder);
  doc.clear();
  return String(holder);
}

void setup() {
  Serial.begin(115200);
  initWIFI();
  initSPIFFS();

  pinMode(LEDPIN, OUTPUT);
  pinMode(IRPIN, OUTPUT);
  pinMode(TRIGPIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHOPIN, INPUT); // Sets the echoPin as an Input

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html", false, processors);
  });

  server.on("/chart.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/chart.min.js", "text/javascript");
  });

  server.serveStatic("/", SPIFFS, "/");

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "The content you are looking for was not found.");
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  // Start server
  server.begin();

}
// https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.5.1/chart.min.js
void loop() {
  IRsensorvalue = analogRead(IRPIN);
  if(IRsensorvalue < 200) {
    digitalWrite(LEDPIN, HIGH);
    objcount++;
    delay(500); 
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
  
  // Calculate the distance, prevent errors
  if (duration < 64000) 
    distanceCm = duration * SOUND_SPEED/2;
  
  if ((millis() - lastTime) > lastTimeDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getPackageData().c_str(),"packageData",millis());
    lastTime = millis();
    Serial.println(getPackageData().c_str());
  }

  // Prints the distance in the Serial Monitor
  //Serial.print("Distance (cm): ");
  //Serial.println(objcount);
  //Serial.println(distanceCm);
  
  //float voltage = IRsensorvalue * (3.3 / 4096.0);
  //Serial.println(IRsensorvalue);
  //Serial.println(voltage);
  //Serial.println(duration);
  //delay(500);
}