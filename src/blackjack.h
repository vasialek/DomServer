#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "idgenerator.h"

class BlackJackGame
{
private:
    char _gameId[IdGenerator::LengthOfId + 1];
    bool _cardsDealt[52];
    IdGenerator _generator;
public:
    BlackJackGame();
    const char* NewGame();
    void Shuffle(const char *gameId);
    int GetNexCard(const char *gameId);
    ~BlackJackGame();
};

BlackJackGame::BlackJackGame()
{
    srand(time(0));
}

const char* BlackJackGame::NewGame()
{
    strncpy(_gameId, _generator.Generate(), IdGenerator::LengthOfId);
    return _gameId;
}

int BlackJackGame::GetNexCard(const char *gameId)
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
