#define BLACKJACK_H

#include <iostream>
#include <time.h>
#include <stdlib.h>

void Shuffle(bool CardsDealt[]);
void PrintCard(int Card);
void PrintHand(int Hand[], const int CardCount);
int GetNextCard(bool CardsDealt[]);
int ScoreHand(int Hand[], const int CardCount);
void PrintScoresAndHands(int HouseHand[], const int HouseCardCount, int PlayerHand[], const int PlayerCardCount);


int main()
{
    using namespace std;

    srand(time(0));

    bool CardsDealt[52];
    int HouseCardCount = 0;
    int HouseHand[12];
    int PlayerCardCount = 0;
    int PlayerHand[12];
    char PlayAgain;


    while(true)
    {
        //shuffles
        Shuffle(CardsDealt);

        // generates two cards to the player and the dealer
        PlayerHand[0] = GetNextCard(CardsDealt);
        HouseHand[0] = GetNextCard(CardsDealt);
        PlayerHand[1] = GetNextCard(CardsDealt);
        HouseHand[1] = GetNextCard(CardsDealt);
        HouseCardCount = 2;
        PlayerCardCount = 2;

        cout<<"\n-----------------------NEW HAND-------------------------\n"<<endl;
        char choice;
        bool Hits = true;
        int PlayerScore = ScoreHand(PlayerHand, PlayerCardCount);

        do {
            // shows 1 dealer's card
            cout<<"Dealer's hand"<<endl;
            cout<<"**";
            PrintCard(HouseHand[1]);
            cout<<endl;
            // shows player's hand
            cout<<"Player's hand: Score = " << ScoreHand(PlayerHand, PlayerCardCount)<<endl;
            PrintHand(PlayerHand, PlayerCardCount);
            if(PlayerScore == 21)
            {
                break;
            }

            //hit or stand
            cout<<"Hit(h) or Stand(s)? ";
            cin>>choice;
            if(choice == 'h')
            {
                PlayerHand[PlayerCardCount] = GetNextCard(CardsDealt);
                ++PlayerCardCount;
            }
            else if(choice == 's')
            {
                Hits = false;
            }
            else {
                cout<<"\nTry again!"<<endl;
            }
            cout<<endl;
            PlayerScore = ScoreHand(PlayerHand, PlayerCardCount);
        }
        // checks whether player busted
        while(Hits && PlayerScore < 22);
        // busted
        if(PlayerScore > 21)
        {
            cout<<"\tYOU LOSE!\n"<<endl;
            // show scores
            PrintScoresAndHands(HouseHand, HouseCardCount, PlayerHand, PlayerCardCount);
        }
        else   // if player didn't bust dealer hits below 17
         {
            int HouseScore = ScoreHand(HouseHand, HouseCardCount);
            while(HouseScore < 17)
             {
                HouseHand[HouseCardCount] = GetNextCard(CardsDealt);
                ++HouseCardCount;
                HouseScore = ScoreHand(HouseHand, HouseCardCount);
             }
             bool HouseBusts = (HouseScore > 21);
             if(HouseBusts)
             {
                 // the dealer's busted => player wins
                 cout<<"\tYOU WIN!\n"<<endl;
                 PrintScoresAndHands(HouseHand, HouseCardCount, PlayerHand, PlayerCardCount);
             }
             else // if not, compares scores and determines the winner
             {

                 // if player score = house score => push
                 if(PlayerScore == HouseScore)
                 {
                     //push
                     cout<<"\tPUSH.\n"<<endl;
                     PrintScoresAndHands(HouseHand, HouseCardCount, PlayerHand, PlayerCardCount);
                 }
                 else if (PlayerScore > HouseScore)
                 {
                     //player wins
                     cout<<"\tYOU WIN!\n"<<endl;
                     PrintScoresAndHands(HouseHand, HouseCardCount, PlayerHand, PlayerCardCount);
                 }
                 else
                 {
                     //dealer wins
                     cout<<"\tYOU LOSE!\n"<<endl;
                     PrintScoresAndHands(HouseHand, HouseCardCount, PlayerHand, PlayerCardCount);
                 }
             }

         }
         //after hand is finished asks whether you want to play again
        cout<<"\nPlay again? y/n \n";
        cin>>PlayAgain;
        if(PlayAgain == 'y')
        {
            continue;
        }
        else if(PlayAgain == 'n' || PlayAgain == 's')
        {
            cout << "\n\tThanks for playing!\n";
            return 0;
        }
        else
        {
            continue;
        }

    }
}


// shuffles deck
void Shuffle(bool CardsDealt[])
{
    for (int i = 0; i < 52; ++i)
    {
        CardsDealt[i] = false;
    }
}

// shows card
void PrintCard(int Card)
{
    using namespace std;
    const int Rank = (Card % 13);
    if(Rank == 0)
    {
        cout<<"A";
    }
    else if(Rank < 9)
    {
        cout<<(Rank+1);
    }
    else if(Rank == 9)
    {
        cout<<"10";
    }
    else if(Rank == 10)
    {
        cout<<"J";
    }
    else if(Rank == 11)
    {
        cout<<"Q";
    }
    else {
        cout<<"K";
    }

    // c - clubs; d - diamonds; h - hearts; s - spades;
    const int Suit = (Card/13);
    if(Suit == 0)
    {
        cout<<"C";
    }
    else if(Suit == 1)
    {
        cout<<"D";
    }
    else if (Suit == 2)
    {
        cout<<"H";
    }
    else {
        cout<<"S";
    }
}

// shows hand
void PrintHand(int Hand[], const int CardCount)
{
    using namespace std;
    for(int i = 0; i < CardCount; ++i)
    {
        const int NextCard = Hand[i];
        PrintCard(NextCard);
        cout<<" ";
    }
    cout<<endl;
}

// generates next card
int GetNextCard(bool CardsDealt[])
{
    bool CardDealt = true;
    int NewCard = -1;
    do {
        NewCard = (rand() % 52);
        if(!CardsDealt[NewCard])
        {
            CardDealt = false;
        }
    }
    while (CardDealt);
    return NewCard;
}

// counts score
int ScoreHand(int Hand[], const int CardCount)
{
    int Score = 0;
    int AceCount = 0;
    for(int i = 0; i < CardCount; ++i)
    {
        const int NextCard = Hand[i];
        const int Rank = (NextCard % 13);
        if(Rank == 0)
        {
            ++AceCount;
            ++Score;
        }
        else if(Rank < 9)
        {
            Score += (Rank + 1);
        }
        else {
            Score += 10;
        }
    }
    while (AceCount > 0 && Score < 12)
    {
        --AceCount;
        Score +=  10;
    }
    return Score;
}

// prints scores
void PrintScoresAndHands(int HouseHand[], const int HouseCardCount, int PlayerHand[], const int PlayerCardCount)
{
    using namespace std;
    cout<<"Dealer's hand: Score = " << ScoreHand(HouseHand, HouseCardCount)<<endl;
    PrintHand(HouseHand, HouseCardCount);
    cout<<"Player's hand: Score = " << ScoreHand(PlayerHand, PlayerCardCount)<<endl;
    PrintHand(PlayerHand, PlayerCardCount);
}

