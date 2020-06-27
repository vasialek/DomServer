#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <iostream>
#include <time.h>
#include <stdlib.h>

class BlackJackGame
{
private:
    /* data */
public:
    BlackJackGame();
    int GetNexCard(bool cardsDealt[]);
    ~BlackJackGame();
};

BlackJackGame::BlackJackGame()
{
}

int BlackJackGame::GetNexCard(bool cardsDealt[])
{
    static int cardId = 1;
    return ++cardId;
}

BlackJackGame::~BlackJackGame()
{
}


// void Shuffle(bool CardsDealt[]);
// void PrintCard(int Card);
// void PrintHand(int Hand[], const int CardCount);
// int GetNextCard(bool CardsDealt[]);
// int ScoreHand(int Hand[], const int CardCount);
// void PrintScoresAndHands(int HouseHand[], const int HouseCardCount, int PlayerHand[], const int PlayerCardCount);


#endif