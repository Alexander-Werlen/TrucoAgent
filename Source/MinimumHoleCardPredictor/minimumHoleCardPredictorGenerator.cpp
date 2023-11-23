/* 
    handles batch proccessing of minimumHoleCardPredictorGenerator
*/
#include "../auxiliares/cardUtils.hpp"
#include "../auxiliares/deck.hpp"

#include <fstream>
#include <string>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <stdexcept>
#include <fstream>
#include <iostream>

using namespace std;

map<string,int> minimumHoleCardGivenData;

void overWriteCache(int numberOfBatch, string pathOfCache){
    ofstream cache;
    cache.open(pathOfCache+"/cacheMinimumHoleCardGivenInfo"+to_string(numberOfBatch)+".txt");

    cache<<numberOfBatch<<endl;
    for(auto element : minimumHoleCardGivenData){
        cache<<element.first<<" "<<element.second<<endl;
    }

    cache.close();
}

void computeBatch(int numberOfBatch){
    //numberOfBatch also indicates the first card chosen

    cardUtils cardUtils;
    Deck deck;

    int amountCards = deck.deckArr.size();
    //generating all possible querys for batch
    {
    int card1ID = numberOfBatch; //choosing card1
        for(int card2ID = card1ID+1; card2ID<amountCards; card2ID++){ //choosing card2
            cout<<"realizado: "<<card2ID<<"/40"<<endl;
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
                            string pointsOpponentQuery;
                            char decenaAux = '0'+pointsOpponent/10;
                            char unidadAux = '0'+pointsOpponent%10;
                            
                            pointsOpponentQuery="["+string(1,decenaAux)+string(1,unidadAux)+"]";                  

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

int main(){
    /* 
        cache of previous computations is stored in cachedData/minimumHoleCardPredictorCache/cacheMinimumHoleCardGivenInfo.txt
        format of file:
        <number of iterations done>
        <state1[string]> <minimumHoleCardValueFound1[int]>
        <state2> <minimumHoleCardValueFound2>
        ...
        <stateN> <minimumHoleCardValueFoundN>
        endOfFile

        for first iteration file has to start with a cero.
    */
    cout<<"Procesamiento batch"<<endl;
    cout<<string(20,'-')<<endl;

    string pathOfCache = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/minimumHoleCardPredictorCache";
    ifstream cache;

    cout<<"Escriba numero de cache a computar: "<<endl;
    int iterationsDone;
    cin>>iterationsDone;

    cout<<"Inicio computacion batch N"<<iterationsDone<<endl;
    computeBatch(iterationsDone);
    cout<<"Computacion finalizada"<<endl;
    cout<<string(20,'-')<<endl;

    cout<<"Inicio sobreescritura del cache"<<endl;
    cout<<"NO INTERRUMPIR PROCESO"<<endl;
    overWriteCache(iterationsDone+1, pathOfCache);
    cout<<"Escritura finalizada"<<endl;
    cout<<string(20,'-')<<endl;
}