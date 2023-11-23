#include "minimumHoleCardPredictor.hpp"

MinimumHoleCardPredictor::MinimumHoleCardPredictor(){
    
    int amountCards = deck.deckArr.size();
    //generating all possible querys
    {
    for (int card1ID = 0; card1ID < amountCards; card1ID++){ //choosing card1
        for(int card2ID = card1ID+1; card2ID<amountCards; card2ID++){ //choosing card2
            for(int card3ID = card2ID+1; card3ID<amountCards; card3ID++){ //choosing card3
                for (int card4ID = 0; card4ID < amountCards; card4ID++){ //choosing card4
                    for(int card5ID = card4ID+1; card5ID<amountCards; card5ID++){ //choosing card5
                        for(int card6ID = card5ID+1; card6ID<amountCards; card6ID++){ //choosing card6
                            //checking if is a valid deal
                            
                            vector<int> cardsID= {card1ID,card2ID,card3ID,card4ID,card5ID,card6ID};
                            set<string> cards;
                            for(int ID : cardsID) cards.insert(deck.deckArr[ID]);
                            if(cards.size()!=6) continue; //invalid deal

                            vector<string> handPlayer = {deck.deckArr[card1ID],deck.deckArr[card2ID],deck.deckArr[card3ID]};
                            vector<string> handOpponent = {deck.deckArr[card4ID],deck.deckArr[card5ID],deck.deckArr[card6ID]};
                            int pointsOpponent = cardUtils.envidoPointsOfHand(handOpponent);

                            cardUtils.formatHand(handPlayer);
                            cardUtils.formatHand(handOpponent);
                            string handPlayerQuery = "";
                            for(string card : handPlayer) handPlayerQuery+="["+card+"]";
                            string pointsOpponentQuery = "";
                            pointsOpponentQuery+="["+to_string(('0'+pointsOpponent/10))+to_string(('0'+pointsOpponent%10))+"]";

                            //queries sin cartas oponente
                            for(auto card : handOpponent) {
                                minimumHoleCardGivenData[handPlayerQuery] = 
                                max(minimumHoleCardGivenData[handPlayerQuery], cardUtils.valorTruco(card));
                            }
                            //queries sin cartas oponente pero con puntos oponente
                            for(auto card : handOpponent) {
                                minimumHoleCardGivenData[handPlayerQuery+pointsOpponentQuery] = 
                                max(minimumHoleCardGivenData[handPlayerQuery+pointsOpponentQuery], cardUtils.valorTruco(card));
                            }

                            //queries con 1 carta del oponente
                            for (int id = 0; id < 3; id++)
                            {
                                string handOpponentQuery="["+handOpponent[id]+"]";
                                for(string card : handOpponent) {
                                    if(card==handOpponent[id]) continue;
                                    minimumHoleCardGivenData[handPlayerQuery+handOpponentQuery] = 
                                    max(minimumHoleCardGivenData[handPlayerQuery+handOpponentQuery], cardUtils.valorTruco(card));
                                }
                            }
                            //queries con 1 carta del oponente pero con puntos oponente
                            for (int id = 0; id < 3; id++)
                            {
                                string handOpponentQuery="["+handOpponent[id]+"]";
                                for(string card : handOpponent) {
                                    if(card==handOpponent[id]) continue;
                                    minimumHoleCardGivenData[handPlayerQuery+handOpponentQuery+pointsOpponentQuery] = 
                                    max(minimumHoleCardGivenData[handPlayerQuery+handOpponentQuery+pointsOpponentQuery], cardUtils.valorTruco(card));
                                }
                            }

                            //queries con 2 cartas del oponente
                            for (int id = 0; id < 3; id++)
                            {
                                string handOpponentQuery="";
                                for(string card : handOpponent) if(card!=handOpponent[id]) handOpponentQuery+="["+card+"]";
                                
                                minimumHoleCardGivenData[handPlayerQuery+handOpponentQuery] = 
                                max(minimumHoleCardGivenData[handPlayerQuery+handOpponentQuery], cardUtils.valorTruco(handOpponent[id]));
                            }
                            //queries con 2 cartas del oponente con puntos oponente
                            for (int id = 0; id < 3; id++)
                            {
                                string handOpponentQuery="";
                                for(string card : handOpponent) if(card!=handOpponent[id]) handOpponentQuery+="["+card+"]";
                                
                                minimumHoleCardGivenData[handPlayerQuery+handOpponentQuery+pointsOpponentQuery] = 
                                max(minimumHoleCardGivenData[handPlayerQuery+handOpponentQuery+pointsOpponentQuery], cardUtils.valorTruco(handOpponent[id]));
                            }
                        }
                    }
                }
            }
        }
    }
    }

}

int MinimumHoleCardPredictor::getMinimumPosibleHoleCardTrucoValue(vector<string> P1Cards, vector<string> P2Cards, int P2EnvidoPoints){
    if(P1Cards.size()!=3 | P2Cards.size()>3) throw invalid_argument("gave invalid number of cards");
    
    cardUtils.formatHand(P1Cards);
    cardUtils.formatHand(P2Cards);
    
    string query="";
    for(string card : P1Cards) query+="["+card+"]";
    for(string card : P2Cards) query+="["+card+"]";
    if(P2EnvidoPoints!=-1) query+="["+to_string(('0'+P2EnvidoPoints/10))+to_string(('0'+P2EnvidoPoints%10))+"]";

    return minimumHoleCardGivenData[query]; //0 if not valid
}
