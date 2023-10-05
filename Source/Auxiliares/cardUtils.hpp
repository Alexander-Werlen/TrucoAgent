/* 
    Provides auxiliar functions that involve cards
*/

#include "deck.hpp"
#include <string>

using namespace std;
class cardUtils{
    public:
    cardUtils();
    //accessing
    char palo(const string &);
    int valorEnvido(const string &);

    //computing
    bool cardIsValid(const string &);
    int envidoPointsGivenTwoCards(const string &, const string &);
    int envidoPointsOfHand(const vector<string> &);

    private:
    Deck fullDeck;
};
