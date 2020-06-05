#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// SSID and password for connection
String ssid = "Cgates_E03973";
String password = "";

// const int PinRed = 15;
// const int PinBlue = 13;
// const int PinGreen = 12;
const int ledPin = 12;
const int ldrPin = A0;

const char* PARAM_MESSAGE = "message";

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

void turnOff(AsyncWebServerRequest *reqeust)
{
  // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });
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
    analogWrite(ledPin, 150);
    Serial.print("IP of DomServer: ");
    Serial.println(WiFi.localIP());
  }
}

void setupServerHandlers() {
  server.on("/get", HTTP_GET, turnOff);
  server.on("/", HTTP_GET, handleIndex);
  server.on("/ldr", HTTP_GET, handleLdr);
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
    Serial.print("Going to connect to WiFi: ");
    Serial.print(ssid.c_str());
    Serial.print("/");
    Serial.println(password.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
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
