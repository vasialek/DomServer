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
// #include "ArduinoJson-v6.15.2.h"


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
BlackJackGame game;

// temporary for output
char buf[512];


void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Page not found");
}

void handleIndex(AsyncWebServerRequest *request) {
  request->send(200, "text/html", "<html><head><title>DomServer</title></head><body>Welcome to DomServer</body></html>");
}

void blackjack(AsyncWebServerRequest *request)
{
/*todo:
/blackjack/newgame
/blackjack/start
/blackjack/get
*/

  request->send(200, "text/plain", "Welcome to Blackjack!");
}

void handleNewGame(AsyncWebServerRequest *request)
{
  //todo: shuffle cards
  request->send(200, "application/json", "{\"id\":\"\123456\"}");
}

void handleStart(AsyncWebServerRequest *request)
{
  //todo: get id of the game -> validate id of the game -> give cards
  //request->send(200, "application/json", "\"[{\"id\":1,\"name\":\"2D\"},{\"id\":2,\"name\":\"AC\"}]\"");
  int cardId0 = game.GetNexCard(nullptr);
  int cardId1 = game.GetNexCard(nullptr);
  sprintf(buf, "[{\"id\":%d,\"name\":\"2D\"},{\"id\":%d,\"name\":\"AC\"}]", cardId0, cardId1);
  Serial.println(buf);
 request->send(200, "application/json", buf); 
}

void handleGet(AsyncWebServerRequest *request)
{
  //todo: get id of the game -> check player's score
  int cardId = game.GetNexCard(nullptr);
  // request->send(200, "application/json", "\"{\"id\":1,\"name\":\"2D\"}\"");
  sprintf(buf, "{\"id\":%d,\"name\":\"2D\"}", cardId);
  Serial.println(buf);
  request->send(200, "application/json", buf);
}

void handleLdr(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "LDR: " + String(analogRead(ldrPin)));
}

void setLedPinTo(int pin, const String paramValue){
  // todo: convert to int. If value >= 0 and value < 255 then set pin to this value
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