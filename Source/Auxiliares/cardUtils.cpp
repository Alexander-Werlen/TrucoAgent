#include "cardUtils.hpp"


cardUtils::cardUtils(){
    Deck fullDeck;
};

//accessing
char cardUtils::palo(const string & card){
    return card.back();
}
int cardUtils::valorEnvido(const string & card){
    int valor = (card[0]-'0')*10+(card[1]-'0');
    if(valor>=10) valor=0;
    return valor;
}

//computing
bool cardUtils::cardIsValid(const string & card){
    return fullDeck.cardIsInDeck(card);
}
int cardUtils::envidoPointsGivenTwoCards(const string & card1, const string & card2){
    if(palo(card1) != palo(card2)) return max(valorEnvido(card1),valorEnvido(card2));
    else return (20+valorEnvido(card1)+valorEnvido(card2));
}
int cardUtils::envidoPointsOfHand(const vector<string> & v){
    //trying all pairs or cards
    int bestEnvidoPoints=0;
    bestEnvidoPoints=max(bestEnvidoPoints, envidoPointsGivenTwoCards(v[0], v[1]));
    bestEnvidoPoints=max(bestEnvidoPoints, envidoPointsGivenTwoCards(v[1], v[2]));
    bestEnvidoPoints=max(bestEnvidoPoints, envidoPointsGivenTwoCards(v[2], v[0]));
    
    return bestEnvidoPoints;
};