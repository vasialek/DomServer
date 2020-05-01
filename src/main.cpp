#include <Arduino.h>
#ifdef ESP32
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>
#else
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>
#endif

// SSID and PW for Config Portal
String ssid = "myWiFi";
String password = "red";

#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager

// const int PinRed = 15;
// const int PinBlue = 13;
// const int PinGreen = 12;
const int ledPin = 12;
const int ldrPin = A0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting configuration portal...");

  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  

  ESP_WiFiManager wifiManager("Wemos-Dev");
  
  wifiManager.setDebugOutput(true);

  ssid = wifiManager.WiFi_SSID();
  password = wifiManager.WiFi_Pass();
  Serial.println("Stored: SSID = " + ssid + ", password = " + password);

  if (ssid != "")
  {
    wifiManager.setConfigPortalTimeout(30);
  }
  
  wifiManager.autoConnect("myWiFi", "red");
}

void printHeartBeat()
{
  int ldrStatus = analogRead(ldrPin);

  static bool isLedOn = false;
  Serial.print("WiFi status: ");
  Serial.println(ldrStatus);
  Serial.println(WiFi.status() == WL_CONNECTED ? "connected" : "not connected");
  if (isLedOn)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      analogWrite(ledPin, 250);
    } else
    {
      analogWrite(ledPin, 150);
    }
  }else
  {
    analogWrite(ledPin, 0);
    analogWrite(ledPin, 0);
  }
  
  isLedOn = !isLedOn;
  
  
}

void loop() {
  static ulong checkstatusTimeoutMs = 0;
  if (millis() > checkstatusTimeoutMs || checkstatusTimeoutMs == 0)
  {
    printHeartBeat();
    checkstatusTimeoutMs += 1000;
  }
  
}
