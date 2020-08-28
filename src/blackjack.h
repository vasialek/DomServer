#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "idgenerator.h"
#include "translator.h"

class BlackJackGame
{
private:
    char _gameId[IdGenerator::LengthOfId + 1];
    bool _cardsDealt[52];
    IdGenerator _generator;
    Translator _translator;
    int _score = 0;
    int _dealerScore = 0;
    int GetNextCard(const char *gameId);
public:
    BlackJackGame();
    const char* NewGame();
    int GetScore(const char *gameId){return _score;}
    int GetDealerScore(const char *gameId){return _dealerScore;}
    void Shuffle(const char *gameId);
    int GetNextPlayerCard(const char *gameId);
    int GetNextDealerCard(const char *gameId);
    bool IsValidGameId(const char *gameId);
    ~BlackJackGame();
};

BlackJackGame::BlackJackGame()
{
    srand(time(0));
}

const char* BlackJackGame::NewGame()
{
    strncpy(_gameId, _generator.Generate(), IdGenerator::LengthOfId);
    Shuffle(_gameId);
    return _gameId;
}

int BlackJackGame::GetNextPlayerCard(const char *gameId)
{
    int cardId = GetNextCard(gameId);
               
    // This card is for the player, count his score
    _score += _translator.GetCardValue(cardId);

    return cardId;
}

int BlackJackGame::GetNextDealerCard(const char *gameId)
{
    int dealerCardId = GetNextCard(gameId);
    _dealerScore += _translator.GetCardValue(dealerCardId);

    return -1;
}

int BlackJackGame::GetNextCard(const char *gameId)
{
    int newCard;
    
    // todo: check if all cards are used
    while (true)
    {
        newCard = (rand() % 52);
        if (this->_cardsDealt[newCard] == false)
        {
            this->_cardsDealt[newCard] = true;
            return newCard;
        }
    }
    
    return -1;
}

bool BlackJackGame::IsValidGameId(const char *gameId)
{
    if (strcmp(gameId, _gameId) == 0)
    {
        return true;
    }

    return false;
}

void BlackJackGame::Shuffle(const char *gameId)
{
    Serial.println("Shuffle: game ID: ");
    Serial.println(gameId);
    for (int i = 0; i < 52; ++i)
    {
        this->_cardsDealt[i] = false;
    }
}

BlackJackGame::~BlackJackGame()
{
}

// void PrintCard(int Card);
// void PrintHand(int Hand[], const int CardCount);
// int ScoreHand(int Hand[], const int CardCount);
// void PrintScoresAndHands(int HouseHand[], const int HouseCardCount, int PlayerHand[], const int PlayerCardCount);

#endif
