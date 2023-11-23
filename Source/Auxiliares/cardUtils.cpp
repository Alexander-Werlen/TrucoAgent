#include "cardUtils.hpp"


cardUtils::cardUtils(){
    Deck fullDeck;
    
    //trucoValues
    {
    valorTrucoGivenCard["01e"] = 1;  
    valorTrucoGivenCard["01b"] = 2;  
    valorTrucoGivenCard["07e"] = 3;  
    valorTrucoGivenCard["07o"] = 4;  

    valorTrucoGivenCard["03b"] = 5;  
    valorTrucoGivenCard["03c"] = 5;  
    valorTrucoGivenCard["03e"] = 5;  
    valorTrucoGivenCard["03o"] = 5;  

    valorTrucoGivenCard["02b"] = 6;  
    valorTrucoGivenCard["02e"] = 6;  
    valorTrucoGivenCard["02c"] = 6;  
    valorTrucoGivenCard["02o"] = 6;  

    valorTrucoGivenCard["01c"] = 7;  
    valorTrucoGivenCard["01o"] = 7;  

    valorTrucoGivenCard["12c"] = 8; 
    valorTrucoGivenCard["12e"] = 8; 
    valorTrucoGivenCard["12o"] = 8; 
    valorTrucoGivenCard["12b"] = 8; 

    valorTrucoGivenCard["11b"] = 9;  
    valorTrucoGivenCard["11o"] = 9;  
    valorTrucoGivenCard["11e"] = 9;  
    valorTrucoGivenCard["11c"] = 9;  

    valorTrucoGivenCard["10b"] = 10;  
    valorTrucoGivenCard["10o"] = 10;  
    valorTrucoGivenCard["10c"] = 10;  
    valorTrucoGivenCard["10e"] = 10;  

    valorTrucoGivenCard["07c"] = 11;  
    valorTrucoGivenCard["07b"] = 11;  

    valorTrucoGivenCard["06c"] = 12;  
    valorTrucoGivenCard["06e"] = 12;  
    valorTrucoGivenCard["06b"] = 12;  
    valorTrucoGivenCard["06o"] = 12;  

    valorTrucoGivenCard["05b"] = 13;  
    valorTrucoGivenCard["05o"] = 13;  
    valorTrucoGivenCard["05e"] = 13;  
    valorTrucoGivenCard["05c"] = 13;  

    valorTrucoGivenCard["04b"] = 14;  
    valorTrucoGivenCard["04c"] = 14;  
    valorTrucoGivenCard["04e"] = 14;  
    valorTrucoGivenCard["04o"] = 14;  
    }
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

int cardUtils::valorTruco(const string & card){
    return valorTrucoGivenCard[card];
}


//validating
bool cardUtils::cardIsValid(const string & card){
    return fullDeck.cardIsInDeck(card);
}

//computing
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

//formating
void cardUtils::formatHand(vector<string> & hand){
    //orders cards in vector according to standard
    sort(hand.begin(),hand.end());
};
