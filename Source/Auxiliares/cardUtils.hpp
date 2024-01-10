/* 
    Provides auxiliar functions that involve cards
*/
#pragma once
#include "deck.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include <map>

using namespace std;
class cardUtils{
    public:
    cardUtils();
    //accessing
    char palo(const string &);
    int valorEnvido(const string &);
    int valorTruco(const string &);

    //validating
    bool cardIsValid(const string &);

    //computing
    int envidoPointsGivenTwoCards(const string &, const string &);
    int envidoPointsOfHand(const vector<string> &);
    bool firstWinsTruco(const string &, const string &);

    //formating
    void formatHand(vector<string> &);
    void formatHandTrucoValue(vector<string> &);

    private:
    Deck fullDeck;
    map<string, int> valorTrucoGivenCard;
};
