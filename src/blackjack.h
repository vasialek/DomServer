#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <iostream>
#include <time.h>
#include <stdlib.h>

class BlackJackGame
{
private:
    bool _cardsDealt[52];
public:
    BlackJackGame();
    int GetNexCard(const char *gameId);
    void Shuffle(const char *gameId);
    ~BlackJackGame();
};

BlackJackGame::BlackJackGame()
{
    srand(time(0));
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