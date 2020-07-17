#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>

class Translator
{
private:
    char _buf[128];
public:
    const char* GetCardName(int cardId);
};

const char* Translator::GetCardName(int cardId)
{
    sprintf(_buf, "%d", cardId);
    return _buf;
}

#endif
