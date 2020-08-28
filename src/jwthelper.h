#ifndef JWTHELPER_H
#define JWTHELPER_H

#include <string.h>
#include "ArduinoJWT.h"
#include "AsyncJson.h"

class JwtHelper
{
private:
    ArduinoJWT *_jwtEncoder = nullptr;
    char _jwtBuffer[512];
    char _jsonBuffer[512];
public:
    JwtHelper(const char *secret);
    const char *Encode(const char *gameId, const char *userId);
    ~JwtHelper();
};

JwtHelper::JwtHelper(const char *secret)
{
    _jwtEncoder = new ArduinoJWT(secret);
}

const char *JwtHelper::Encode(const char *gameId, const char *userId)
{
    const int capacity = JSON_OBJECT_SIZE(2); // game ID and user ID
    StaticJsonDocument<capacity> doc;
    JsonObject obj = doc.to<JsonObject>();
    
    obj["gameId"] = gameId;
    obj["userId"] = userId;

    serializeJson(doc, _jsonBuffer);
    _jwtEncoder->encodeJWT(_jsonBuffer, _jwtBuffer);

    return _jwtBuffer;
}

JwtHelper::~JwtHelper()
{
    if (_jwtEncoder != nullptr)
    {
        delete _jwtEncoder;
    }
}

#endif
