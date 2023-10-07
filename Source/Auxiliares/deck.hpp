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
    private:
    vector<string> deckArr;
    set<string> deckSet;

    public:
    Deck();
    bool cardIsInDeck(const string & card);
};