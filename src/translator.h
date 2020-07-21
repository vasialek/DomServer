#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>

class Translator
{
private:
    char _buf[128];
    char GetCardRank(int cardId);
    char GetCardSuit(int cardId);
public:
    const char* GetCardName(int cardId);
    
};


const char* Translator::GetCardName(int cardId)
{
    char cardSuit = GetCardSuit(cardId);
    char cardRank = GetCardRank(cardId);

    sprintf(_buf, "Suit: %c Rank: %c", cardSuit, cardRank);
    return _buf;
}

char Translator::GetCardRank(int cardId)
{
    const int rank = (cardId % 13);
    switch (rank)
    {
    case 0:
        return 'A';
    case 9:
        return 'T';
    case 10:
       return 'J';
    case 11:
        return 'Q';
    case 12:
        return 'K';
    default:
        return '1' + rank;
    }
}

char Translator::GetCardSuit(int cardId)
{
    const int suit = cardId / 13;
    switch (suit)
    {
    case 0:
        return 'C';
    case 1:
        return 'D';
    case 2:
        return 'H';
    case 3:
        return 'S';
    default:
        return '?';
    }
}

#endif