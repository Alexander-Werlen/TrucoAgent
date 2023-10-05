#include "deck.hpp"

using namespace std;

bool Deck::cardIsInDeck(const string & card){
    return deckSet.count(card);
}

Deck::Deck() {  
    deckArr = {
        "01b", "02b", "03b", "04b", "05b", "06b", "07b", "10b", "11b", "12b",
        "01c", "02c", "03c", "04c", "05c", "06c", "07c", "10c", "11c", "12c",
        "01e", "02e", "03e", "04e", "05e", "06e", "07e", "10e", "11e", "12e",
        "01o", "02o", "03o", "04o", "05o", "06o", "07o", "10o", "11o", "12o",
        };

    deckSet = {deckArr.begin(), deckArr.end()};
}

