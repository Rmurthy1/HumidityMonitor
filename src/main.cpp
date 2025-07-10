#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "secureConfig.h" // contains WiFi credentials and ThingSpeak API keys

const int uploadDelay = 60000; // delay in ms between uploads to ThingSpeak
unsigned long lastUploadTime = 0; // last time data was uploaded to ThingSpeak

int humidity = 0; // variable to store humidity data
int temperature = 0; // variable to store temperature data

// wifi
WiFiClient client;

// put function declarations here:
void uploadData();
void queryAHT10();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}
void loop() {
  // put your main code here, to run repeatedly:
}

void queryAHT10() {
  // Code to query the AHT10 sensor

}

void uploadData(int temp, int humidity) {
  if (millis() > lastUploadTime + uploadDelay) {
    lastUploadTime = millis();
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
  
    int x = ThingSpeak.writeField(SECRET_CH_ID, 5, humidity, THINGSPEAK_API_WRITE);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}