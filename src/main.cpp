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
String ssid = "";
String password = "";

#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager

const int PinRed = 15;
const int PinBlue = 13;
const int PinGreen = 12;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting configuration portal...");

  pinMode(PinBlue, OUTPUT);
  pinMode(PinGreen, OUTPUT);
  pinMode(PinRed, OUTPUT);

  ESP_WiFiManager wifiManager("Wemos-Dev");
  
  wifiManager.setDebugOutput(true);

  ssid = wifiManager.WiFi_SSID();
  password = wifiManager.WiFi_Pass();
  Serial.println("Stored: SSID = " + ssid + ", password = " + password);

  if (ssid != "")
  {
    wifiManager.setConfigPortalTimeout(30);
  }
  
  wifiManager.autoConnect("Wemos_AutoConnectAP", "");
}

void printHeartBeat()
{
  static bool isLedOn = false;
  Serial.print("WiFi status: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "connected" : "not connected");
  if (isLedOn)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      analogWrite(PinBlue, 150);
    } else
    {
      analogWrite(PinRed, 150);
    }
  }else
  {
    analogWrite(PinBlue, 0);
    analogWrite(PinRed, 0);
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
