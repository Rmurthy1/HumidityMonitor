#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "secureConfig.h" // contains WiFi credentials and ThingSpeak API keys
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN D2     
#define DHTTYPE DHT11


const int uploadDelay = 60000; // delay in ms between uploads to ThingSpeak, 30000 works too
unsigned long lastUploadTime = 0; // last time data was uploaded to ThingSpeak

int queryDelay = 2000; // overwritten by DHT sensor's min_delay
unsigned long lastQueryTime = 0; // last time sensor data was queried

int humidity = 0; // variable to store humidity data
int temperature = 0; // variable to store temperature data

String status = "ok for now"; // variable to store status message

// wifi
WiFiClient client;

// put function declarations here:
void uploadData(int temp, int humidity);
void querySensor();


DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  queryDelay = sensor.min_delay / 1000;
}

void querySensor() {
  if (millis() - lastQueryTime > queryDelay) {
    lastQueryTime = millis();
    // Get temperature event and print its value.
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
      status = "Error reading temperature!";
    } else {
      temperature = event.temperature; // store temperature value
    }
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
      status = "Error reading humidity!";
    } else {
      humidity = event.relative_humidity; // store humidity value
    }
  }
}

void loop() {
  // Delay between measurements.
  querySensor();
  uploadData(temperature, humidity);
}

void uploadData(int temp, int humidity) {
  if (millis() > lastUploadTime + uploadDelay) {
    lastUploadTime = millis();
    
    if(WiFi.status() != WL_CONNECTED){
      WiFi.disconnect();
      delay(5200);
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        Serial.print(".");
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
    int degreesF = (int)(temp * 9.0 / 5.0 + 32.0); // convert Celsius to Fahrenheit

    ThingSpeak.setField(1, humidity);
    ThingSpeak.setField(2, temperature); // degrees Celsius
    ThingSpeak.setField(3, degreesF); // degrees Fahrenheit
    ThingSpeak.setStatus(status); // optional status message

    int x = ThingSpeak.writeFields(SECRET_CH_ID, THINGSPEAK_API_WRITE);


    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}