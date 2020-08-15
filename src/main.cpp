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
#include <stdio.h>
#include "blackjack.h"
#include <ArduinoJson.h>
#include "translator.h"

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
Translator translator;

char jsonBuffer[512];
char buf[80];

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

void reportError(AsyncWebServerRequest *request, const char* error, int errorCode)
{
  const int capacity = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> doc;
  JsonObject errorMessage = doc.to<JsonObject>();
  errorMessage["msg"] = error;

  serializeJson(doc, jsonBuffer);

  request->send(errorCode, "application/json", jsonBuffer);
}

void handleAuth(AsyncWebServerRequest *request)
{
  if(request->hasParam("email") == false)
  {
    reportError(request, "Please specify your email", 401);
    return;
  }
  if(request->hasParam("password") == false)
  {
    reportError(request, "Please specify your password", 401);
    return;
  }

  AsyncWebParameter *pEmail = request->getParam("email");
  AsyncWebParameter *pPassword = request->getParam("password");

  if(strcasecmp("antoha.c2013@yandex.ru", pEmail->value().c_str()) == 0 && strcmp("123456", pPassword->value().c_str()) == 0)
  {
    const int capacity = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<capacity> doc;
    JsonObject obj = doc.to<JsonObject>();
    obj["token"] = "jdsfhfgjdfghgdfghfhfghh";
    serializeJson(doc, jsonBuffer);

    request->send(200, "application/json", jsonBuffer);

    return;
  }

  reportError(request, "Email or Password is not valid", 401);
}

void handleNewGame(AsyncWebServerRequest *request)
{ 
  const char* gameId = game.NewGame();
  
  const int capacity = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> doc;
  JsonObject id = doc.to<JsonObject>();
  id["id"] = gameId;
  serializeJson(doc, jsonBuffer);

  request->send(200, "application/json", jsonBuffer);
}

void handleGetScores(AsyncWebServerRequest* request)
{
    AsyncWebParameter* p = request->getParam("gameid");
    const char* gameId = p->value().c_str();

    int playerScore = game.GetScore(gameId);
    int dealerScore = game.GetDealerScore(gameId);

    const int capacity = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> doc;
    JsonObject obj = doc.to<JsonObject>();
    obj["score"] = playerScore;
    obj["Dealer_score"] = dealerScore;
    serializeJson(doc, jsonBuffer);

    request->send(200, "application/json", jsonBuffer);
}

void handleStart(AsyncWebServerRequest *request)
{ 
  if(request->hasParam("gameid") == false)
  {
      reportError(request, "No Game Id", 401);
      return;
  }

  AsyncWebParameter* p = request->getParam("gameid");
  const char* gameId = p->value().c_str();

  if(game.IsValidGameId(gameId) == false)
  {
    reportError(request, "Invalid Game Id", 401);
    return;
  }

  int cardId0 = game.GetNexCard(gameId);
  int cardId1 = game.GetNexCard(gameId);
  const char* cardName0 = translator.GetCardName(cardId0);
  const char* cardName1 = translator.GetCardName(cardId1);

  const int capacity = JSON_ARRAY_SIZE(2) + 2 * JSON_OBJECT_SIZE(2);
  StaticJsonDocument<capacity> doc;

  JsonObject card1 = doc.createNestedObject();
  card1["id"] = cardId0;
  card1["name"] = cardName0;
  JsonObject card2 = doc.createNestedObject();
  card2["id"] = cardId1;
  card2["name"] = cardName1;

  serializeJson(doc, jsonBuffer);

  Serial.println(jsonBuffer);
  request->send(200, "application/json", jsonBuffer);
}

void handleGet(AsyncWebServerRequest *request)
{
  if(request->hasParam("gameid") == false)
  {
    reportError(request, "No Game Id", 401);
    return;
  }

  AsyncWebParameter* p = request->getParam("gameid");
  const char* gameId = p->value().c_str();

  if(game.IsValidGameId(gameId) == false)
  {
    reportError(request, "Invalid Game Id", 401);
    return;
  }

  int cardId = game.GetNexCard(gameId);
  const char* cardName = translator.GetCardName(cardId);
  const int capacity = JSON_OBJECT_SIZE(2);
  StaticJsonDocument<capacity> doc;

  JsonObject card = doc.to<JsonObject>();
  card["id"] = cardId;
  card["name"] = cardName;
  serializeJson(doc, jsonBuffer);

  request->send(200, "application/json", jsonBuffer);
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
  server.on("/auth", HTTP_POST, handleAuth);
  server.on("/blackjack/getscores", HTTP_GET, handleGetScores);
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