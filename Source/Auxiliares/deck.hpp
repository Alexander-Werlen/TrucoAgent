/* 
    Provides deck full of cards that can be used in the game.
    Also has function to detect if a card belongs in such deck.
*/
#pragma once
#include <vector>
#include <set>
#include <string>

using namespace std;

class Deck{
    public:
    Deck();
    set<string> deckSet;
    vector<string> deckArr;
    bool cardIsInDeck(const string & card);
};