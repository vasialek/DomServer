#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "credentials.h"


const int PinRed = 15;
const int PinBlue = 13;
const int PinGreen = 12;
const int ledPin = 12;
const int ldrPin = A0;

// Possible WiFi states
const int WiFiClientInitializing = 0;
const int WiFiClientConnecting = 1; 
const int WiFiClientConnected = 2;
const int WiFiAccessPoint = 4;
const int WiFiSetup = 8;
const int WiFiServing = 16;

const ulong HeartbeatTimeoutMs = 2000;

int wifiState = WiFiClientInitializing;

AsyncWebServer server(80);

void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Page not found");
}

void handleIndex(AsyncWebServerRequest *request) {
  request->send(200, "text/html", "<html><head><title>DomServer</title></head><body>Welcome to DomServer</body></html>");
}

void handleLdr(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "LDR: " + String(analogRead(ldrPin)));
}

void handlePage(AsyncWebServerRequest *request){
  request->send(200, "text/plain", "Simple text is " + String(analogRead(ledPin)));
}

void turnLed(AsyncWebServerRequest *request)
{
  Serial.println("Got /turnonLed request...");
  String value, valueR, valueG, valueB;
  if (request->hasParam("r")) 
  {
      value = request->getParam("r")->value();
      request->send(200, "text/plain", "RED LED value is: " + value);
      Serial.println("Going to set RED LED to: " + value);
      int v = atoi(value.c_str());
      Serial.print("  value (int) is: ");
      Serial.println(v);
      analogWrite(PinRed, v);
  }
  else if(request->hasParam("g"))
  { 
    value = request->getParam("g")->value();
    request->send(200, "text/plain", "GREEN LED value is: " + value);
    Serial.println("Going to set GREEN LED to: " + value);
    int v = atoi(value.c_str());
    Serial.print(" value (int) is: ");
    Serial.println(v);
    analogWrite(PinGreen, v);
  }
  else if(request->hasParam("b"))
  {
    value = request->getParam("b")->value();
    request->send(200, "text/plain", "BLUE LED value is: " + value);
    Serial.println("Going to set BLUE LED to: " + value);
    int v = atoi(value.c_str());
    Serial.print(" value (int) is: ");
    Serial.println(v);
    analogWrite(PinBlue, v);
  }
  else if(request->hasParam("r") && request->hasParam("g") && request->hasParam("b"))
  {
    //value = request->getParam("rgb")->value();
    valueR = request->getParam("r")->value();
    valueG = request->getParam("g")->value();
    valueB = request->getParam("b")->value();
    request->send(200, "text/plain", "RED LED value is: " + valueR + "\nGREEN LED value is: " + valueG + "\nBLUE LED value is: " + valueB);
    Serial.println("Going to set RED, GREEN and BLUE values to: " + valueR + valueG + valueB);
    int r = atoi(valueR.c_str());
    int g = atoi(valueG.c_str());
    int b = atoi(valueB.c_str());
    Serial.print(" values (int) are: ");
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    analogWrite(PinRed, r);
    analogWrite(PinGreen, g);
    analogWrite(PinBlue, b);
  }
  /* show all values
  else if(request->hasParam("values"))
  {
    int r = analogRead(PinRed);
    int g = analogRead(PinGreen);
    int b = analogRead(PinBlue);
    request->send(200, "text/plain", "RED LED value is: " + r + "\nGREEN LED values is: " + g + "\nBLUE LED value is: " + b);
  } */
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting DomServer...");

  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
}

void printHeartBeat()
{
  Serial.print("State of WiFi client: ");
  Serial.println(wifiState);
  Serial.print("WiFi status: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "connected" : "not connected");
  if (WiFi.status() == WL_CONNECTED)
  {
    // analogWrite(ledPin, 150);
    Serial.print("IP of DomServer: ");
    Serial.println(WiFi.localIP());
  }
}

void setupServerHandlers() {
  server.on("/", HTTP_GET, handleIndex);
  server.on("/ldr", HTTP_GET, handleLdr);
  server.on("/led", HTTP_GET, turnLed);
  server.on("/page", HTTP_GET, handlePage);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop() {
  static ulong wifiConnectionTimeoutMs = 0;
  static ulong checkstatusTimeoutMs = 0;

  switch (wifiState)
  {
  case WiFiClientInitializing:
    Serial.print("Going to connect to WiFi:");
    Serial.print(ssid);
    Serial.print("/");
    Serial.println("*******");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    wifiState = WiFiClientConnecting;
    break;

  case WiFiClientConnecting:
    if (millis() > wifiConnectionTimeoutMs || wifiConnectionTimeoutMs == 0)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        wifiState = WiFiSetup;
      }
      wifiConnectionTimeoutMs += HeartbeatTimeoutMs;
    }
    break;

    case WiFiSetup:
      Serial.println("Going to start WEB server...");
      setupServerHandlers();
      wifiState = WiFiServing;
      break;
  }

  if (millis() > checkstatusTimeoutMs || checkstatusTimeoutMs == 0)
  {
    printHeartBeat();
    checkstatusTimeoutMs += HeartbeatTimeoutMs;
  }
  
}
