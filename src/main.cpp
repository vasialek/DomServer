#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
// #include "ArduinoJWT.h"
#include "credentials.h"
#include <string>
#include <stdio.h>
#include "blackjack.h"
#include "translator.h"
#include "userrepository.h"
#include "jwthelper.h"

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

const ulong HeartbeatTimeoutMs = 10000;

int wifiState = WiFiClientInitializing;

AsyncWebServer server(80);
BlackJackGame game;
Translator translator;
UserRepository userRepository;
JwtHelper jwtEncoder("password");

char jsonBuffer[512];
char buf[80];

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page not found");
}

void handleIndex(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", "<html><head><title>DomServer</title></head><body>Welcome to DomServer</body></html>");
}

void blackjack(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", "Welcome to Blackjack!");
}

void reportError(AsyncWebServerRequest *request, const char *error, int errorCode)
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
  if (request->hasParam("email") == false)
  {
    reportError(request, "Please specify your email", 401);
    return;
  }
  if (request->hasParam("password") == false)
  {
    reportError(request, "Please specify your password", 401);
    return;
  }

  AsyncWebParameter *pEmail = request->getParam("email");
  const char *email = pEmail->value().c_str();
  AsyncWebParameter *pPassword = request->getParam("password");
  const char *password = pPassword->value().c_str();

  const char *userId = userRepository.GetUser(email, password);
  if (userId != nullptr)
  {
    const char *encoded = jwtEncoder.Encode("No Game ID", userId);
    reportError(request, encoded, 200);
    return;
  }

  reportError(request, "Email or Password is invalid", 401);
}

void handleNewGame(AsyncWebServerRequest *request)
{
  const char *gameId = game.NewGame();

  const int capacity = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> doc;
  JsonObject id = doc.to<JsonObject>();
  id["id"] = gameId;
  serializeJson(doc, jsonBuffer);

  request->send(200, "application/json", jsonBuffer);
}

void handleGetScores(AsyncWebServerRequest *request)
{
  // todo: move to separate method of game ID validation
  AsyncWebParameter *p = request->getParam("gameid");
  const char *gameId = p->value().c_str();

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
  // todo: move to separate method of game ID validation
  if (request->hasParam("gameid") == false)
  {
    reportError(request, "No Game Id", 401);
    return;
  }

  AsyncWebParameter *p = request->getParam("gameid");
  const char *gameId = p->value().c_str();

  if (game.IsValidGameId(gameId) == false)
  {
    reportError(request, "Invalid Game Id", 401);
    return;
  }

  int dealerCard = game.GetNextDealerCard(gameId);
  // const char* dealerCardName = translator.GetCardName(dealerCard);
  Serial.print("Dealer card: ");
  Serial.println(translator.GetCardName(dealerCard));

  int cardId1 = game.GetNextPlayerCard(gameId);
  int cardId2 = game.GetNextPlayerCard(gameId);

  char cardName1[32], cardName2[32];
  strncpy(cardName1, translator.GetCardName(cardId1), sizeof(cardName1));
  Serial.print("Card name 1: ");
  Serial.println(cardName1);
  strncpy(cardName2, translator.GetCardName(cardId2), sizeof(cardName2));
  Serial.print("Card name 2: ");
  Serial.println(cardName2);

  const int capacity = 2 * JSON_OBJECT_SIZE(2) + 2 * JSON_ARRAY_SIZE(2);
  StaticJsonDocument<capacity> doc;

  JsonObject card1 = doc.createNestedObject();
  card1["id"] = cardId1;
  card1["name"] = cardName1;
  JsonObject card2 = doc.createNestedObject();
  card2["id"] = cardId2;
  card2["name"] = cardName2;

  serializeJson(doc, jsonBuffer);

  // Serial.println(jsonBuffer);
  request->send(200, "application/json", jsonBuffer);
}

void handleGet(AsyncWebServerRequest *request)
{
  // todo: move to separate method of game ID validation
  if (request->hasParam("gameid") == false)
  {
    reportError(request, "No Game Id", 401);
    return;
  }

  AsyncWebParameter *p = request->getParam("gameid");
  const char *gameId = p->value().c_str();

  if (game.IsValidGameId(gameId) == false)
  {
    reportError(request, "Invalid Game Id", 401);
    return;
  }

  int cardId = game.GetNextPlayerCard(gameId);

  // todo: call for dealer card (do not return it to user)

  const char *cardName = translator.GetCardName(cardId);
  const int capacity = JSON_OBJECT_SIZE(2);
  StaticJsonDocument<capacity> doc;

  JsonObject card = doc.to<JsonObject>();
  card["id"] = cardId;
  card["name"] = cardName;
  serializeJson(doc, jsonBuffer);

  request->send(200, "application/json", jsonBuffer);
}

void handleTest(AsyncWebServerRequest *request)
{
  // ArduinoJWT jwtEncode("TestPass");
  // String payload = "{\"gameId\":\"GameId1234\",\"userId\":\"UserId1234354656565\"}";
  // String jwt = jwtEncode.encodeJWT(payload);
  // Serial.print("Encoded JWT: ");
  // Serial.println(jwt);

  // String decoded;
  // jwtEncode.decodeJWT(jwt, decoded);
  // Serial.print("Decoded: ");
  // Serial.println(decoded);

  JwtHelper jwtHelper("TestPass");

  if(request->hasHeader("Authorization"))
  {
    AsyncWebHeader* h = request->getHeader("Authorization");
    Serial.printf("Authorization: %s\n", h->value().c_str());
    if(strncmp(h->value().c_str(), "Bearer ", 7))
    { 
      String jwt(h->value().c_str() + 7);
      Serial.println(jwt);
     // jwtEncoder.Decode();
      // strcpy( )
    }
  }

  reportError(request, jwtHelper.Encode("GameId1234567890", "UserId12345678901234567890"), 200);
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting DomServer...");

  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(PinRed, INPUT);
  pinMode(PinGreen, INPUT);
  pinMode(PinBlue, INPUT);

  userRepository.Add("964c0d6eaaf34440baf0bd16dc966f27", "proglamer@gmail.com", "123456");
  userRepository.Add("627950083e564e07884cd07412d8ba89", "antoha.c2013@yandex.ru", "123456");
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

void setupServerHandlers()
{
  server.on("/", HTTP_GET, handleIndex);
  server.on("/test", HTTP_GET, handleTest);
  server.on("/auth", HTTP_POST, handleAuth);
  server.on("/blackjack/getscores", HTTP_GET, handleGetScores);
  server.on("/blackjack/newgame", HTTP_GET, handleNewGame);
  server.on("/blackjack/start", HTTP_GET, handleStart);
  server.on("/blackjack/get", HTTP_GET, handleGet);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop()
{
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
