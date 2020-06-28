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
#include <string>
#include "blackjack.h"
#include <ArduinoJson.h>

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

const ulong HeartbeatTimeoutMs = 60000;

int wifiState = WiFiClientInitializing;

AsyncWebServer server(80);
BlackJackGame game;

char jsonBuffer[512];

void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Page not found");
}

void handleIndex(AsyncWebServerRequest *request) {
  request->send(200, "text/html", "<html><head><title>DomServer</title></head><body>Welcome to DomServer</body></html>");
}

void blackjack(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", "Welcome to Blackjack!");
}

void handleNewGame(AsyncWebServerRequest *request)
{
  // todo: shuffle cards
  // todo: serialize to JSON using library
  request->send(200, "application/json", "{\"id\":\"\123456\"}");
}

void handleStart(AsyncWebServerRequest *request)
{
  int cardId0 = game.GetNexCard(nullptr);
  int cardId1 = game.GetNexCard(nullptr);

  const int capacity = JSON_ARRAY_SIZE(2) + 2 * JSON_OBJECT_SIZE(2);
  StaticJsonDocument<capacity> doc;

  JsonObject card1 = doc.createNestedObject();
  card1["id"] = cardId0;
  card1["name"] = "XX";
  JsonObject card2 = doc.createNestedObject();
  card2["id"] = cardId1;
  card2["name"] = "YY";

  serializeJson(doc, jsonBuffer);

  Serial.println(jsonBuffer);
  request->send(200, "application/json", jsonBuffer);
}

void handleGet(AsyncWebServerRequest *request)
{
  int cardId = game.GetNexCard(nullptr);

  // todo: serialize to object, not array. See https://arduinojson.org/v6/doc/serialization/
  request->send(200, "application/json", "");
}

void handleLdr(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "LDR: " + String(analogRead(ldrPin)));
}

void setLedPinTo(int pin, const String paramValue){
  int v = atoi(paramValue.c_str());
  if(v >= 0 && v <= 255)
  {
    analogWrite(pin, v);
  }
}

void handlePage(AsyncWebServerRequest *request){
  request->send(200, "text/plain", "Simple text is " + String(analogRead(ledPin)));
}

void handleRgb(AsyncWebServerRequest *request)
{
  Serial.println("Got /led request...");
  if (request->hasParam("r"))
  { 
    setLedPinTo(PinRed, request->getParam("r")->value());
  }
  if(request->hasParam("g"))
  {
    setLedPinTo(PinGreen, request->getParam("g")->value());
  }
  if(request->hasParam("b"))
  { 
    setLedPinTo(PinBlue, request->getParam("b")->value());
  }
  request->send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting DomServer...");

  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(PinRed, INPUT);
  pinMode(PinGreen, INPUT);
  pinMode(PinBlue, INPUT);
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
  server.on("/led", HTTP_GET, handleRgb);
  server.on("/page", HTTP_GET, handlePage);
  server.on("/blackjack/newgame",HTTP_GET, handleNewGame);
  server.on("/blackjack/start",HTTP_GET, handleStart);
  server.on("/blackjack/get",HTTP_GET, handleGet);
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