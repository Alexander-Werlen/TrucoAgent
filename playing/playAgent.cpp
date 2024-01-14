#include "../source/auxiliares/cardUtils.hpp"
#include "../source/auxiliares/deck.hpp"
#include <map>
#include <vector>
#include <deque>
#include <set>
#include <random>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

cardUtils utils;
Deck deck;
map<string,int> valorTrucoGivenCard;
map<string,int> valorTrucoAbs5GivenCard;
map<string,int> whoWinsRound;
map<int, vector<int>> posibleEnvidoBets = { //no falta envido
    {0, {2,3}},
    {2, {4,5}},
    {4, {7}}
};

class Strategy{
    public:
    vector<string> actions;
    vector<double> probabilities;

    void addAction(string act, double p){
        actions.push_back(act);
        probabilities.push_back(p);
    }
};

map<string, Strategy> strategies;

static std::random_device rd;

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

pair<vector<string>, vector<string>> getRandomStart(){
    vector<string> cards1, cards2;
    string r;
    int n=0;
    set<string> manos;
    while(n<3){
        r = *select_randomly(deck.deckArr.begin(), deck.deckArr.end());
        if(manos.count(r)) continue;
        n++;
        cards1.push_back(r);
        manos.insert(r);
    }
    while(n<6){
        r = *select_randomly(deck.deckArr.begin(), deck.deckArr.end());
        if(manos.count(r)) continue;
        n++;
        cards2.push_back(r);
        manos.insert(r);
    }
    sort(cards1.begin(), cards1.end());
    sort(cards2.begin(), cards2.end());

    return {cards1, cards2};
}

string ptsToString(int envidoPoints){
    //converts int to envidoPoints string format
    string envidoPointsStr="";
    envidoPointsStr+=('0'+envidoPoints/10);
    envidoPointsStr+=('0'+envidoPoints%10);

    return envidoPointsStr;
}

string createIdOfEnvidoPointsBothPlayer(int ptsA, int ptsB){
    return "["+ptsToString(ptsA)+"-"+ptsToString(ptsB)+"]";
}

//actually a round
class Game{
    public:
    Game(int gamePointsPlayerIn, int gamePointsAgentIn, bool playerStartsAsMano){
        gamePointsPlayer = gamePointsPlayerIn;
        gamePointsAgent = gamePointsAgentIn;
        if(playerStartsAsMano) {
            gamePointsP1=gamePointsPlayerIn;
            gamePointsP2=gamePointsAgentIn;
        }else{
            gamePointsP1=gamePointsAgentIn;
            gamePointsP2=gamePointsPlayerIn;
        }
        playerIsMano = playerStartsAsMano;
        mesa = {"","","","","",""};
        currentRound=0;
        isP1Turn=true;
        //truco
        trucoValue=1;
        canBetTrucoP1=true;
        canBetTrucoP2=true;
        hasToRespondTruco=false;
        isP1TrucoTurn=true;
        P1StartsInTruco=true;
        roundWinsP1=0;
        roundWinsP2=0;
        //envido
        envidoValue=0;
        envidoBetValue=0;
        canBetEnvidoP1=true;
        canBetEnvidoP2=true;
        hasToRespondEnvido=false;
        inFaltaEnvido=false;
        valueFaltaEnvido=30-max(gamePointsP1, gamePointsP2);

        //getting hands
        auto bothHands = getRandomStart();
        handP1=bothHands.first;
        handP2=bothHands.second;

        utils.formatHandTrucoValue(handP1);
        utils.formatHandTrucoValue(handP2);

        envidoPointsP1 = utils.envidoPointsOfHand(handP1);
        envidoPointsP2 = utils.envidoPointsOfHand(handP2);

        //infoStates
        infoStateTrucoIdP1="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P1-R1|";
        infoStateTrucoIdP1+=getBucketsAbs5Str(handP1)+"[X]|";
        infoStateTrucoIdP2="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P2-R1|";
        infoStateTrucoIdP2+=getBucketsAbs5Str(handP2)+"[X][M][P]|";
        history="";
        whoWonHistory="";

        infoStateEnvidoP1="0|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|["+ptsToString(envidoPointsP1)+"]|";
        infoStateEnvidoP2="0|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|["+ptsToString(envidoPointsP2)+"]|";
        inEnvidoStageP1=true;
        inEnvidoStageP2=true;

        terminal=false;

        //idxs
        idxR1ThrownCard=24;
        idxR1AmountBetter=27;
        idxR1AmountParda=30;
        idxR2FThrownCard=16;
        idxR2SThrownCard=22;
        idxR2SAmountBetter=25; 
        idxR2SAmountParda=28; 
        idxR3thrownCardStart=20;
        playerStartedAsMano=playerStartsAsMano;
    }

    //play
    void playAgent(){
        if((playerIsMano?inEnvidoStageP2:inEnvidoStageP1)){//playEnvido
            string stateAgent = ((!playerIsMano)? infoStateEnvidoP1 : infoStateEnvidoP2);
            
            if(strategies.find(stateAgent)==strategies.end()){
                cout<<"Agent state: "<<stateAgent<<endl;
                cout<<"####ERROR envido state not found"<<endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            }
            std::mt19937 gen(std::random_device{}());
            std::discrete_distribution<std::size_t> d{strategies[stateAgent].probabilities.begin(), strategies[stateAgent].probabilities.end()};

            string action = strategies[stateAgent].actions[d(gen)];
            
            cout<<"Agent state: "<<stateAgent<<endl;
            for (int i = 0; i < strategies[stateAgent].actions.size(); i++)
            {
                cout<<strategies[stateAgent].actions[i]<<" "<<strategies[stateAgent].probabilities[i]<<endl;
            }
            

            if(action=="(q)"){
                acceptEnvido();
            }else if(action=="(nq)"){
                rejectEnvido();
            }else if(action=="(p)"){
                if(playerIsMano) inEnvidoStageP2=false;
                else inEnvidoStageP1=false;
                appendToEnvidoInfoStates("(p)");
            }else if(action=="(b2)") betEnvido(2);
            else if(action=="(b3)") betEnvido(3);
            else if(action=="(b4)") betEnvido(4);
            else if(action=="(b5)") betEnvido(5);
            else if(action=="(b7)") betEnvido(7);
            else if(action=="(bF)") betFaltaEnvido();
            else {
                cout<<"#####ERROR accion envido no valida"<<endl;
                cout<<action<<endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(10000));}


        }else{//play truco
            string stateAgent = ((!playerIsMano)? infoStateTrucoIdP1 : infoStateTrucoIdP2)+"["+whoWonHistory+"]"+history;
            
            if(strategies.find(stateAgent)==strategies.end()){
                cout<<"!WARNING truco state not found, random strat used"<<endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                if(hasToRespondTruco) rejectTruco();
                else{
                    throwCard(0);
                }
                informMesa();
                return;
            }
            std::mt19937 gen(std::random_device{}());
            std::discrete_distribution<std::size_t> d{strategies[stateAgent].probabilities.begin(), strategies[stateAgent].probabilities.end()};

            string action = strategies[stateAgent].actions[d(gen)];

            cout<<"Agent state: "<<stateAgent<<endl;
            for (int i = 0; i < strategies[stateAgent].actions.size(); i++)
            {
                cout<<strategies[stateAgent].actions[i]<<" "<<strategies[stateAgent].probabilities[i]<<endl;
            }

            vector<string> actions = getActionsArray(action);

            for(string act : actions){
                if(playerIsMano==isP1Turn) cout<<"####ERROR agente intenta jugar en turno de player"<<endl;
                if(act=="(q)") acceptTruco();
                else if(act=="(nq)") rejectTruco();
                else if(act=="(b)") betTruco();
                else if(act=="(T1)") throwCard(0);
                else if(act=="(T2)") throwCard(1);
                else if(act=="(T3)") throwCard(2);
                else cout<<"####ERROR trucoAction no identificada"<<endl;
            }
            informMesa();
        }
    }

    void throwCard(int idxCard){
        if(currentRound==0){
            if(isP1TrucoTurn){
                mesa[0]=handP1[idxCard];
                //stateP2
                int c=0,p=0;
                for(auto card : handP2) if(utils.firstWinsTruco(card, handP1[idxCard])) c++;
                for(auto card : handP2) if(utils.valorTruco(card)==utils.valorTruco(handP1[idxCard])) p++;
                infoStateTrucoIdP2[idxR1AmountBetter]='0'+c;
                infoStateTrucoIdP2[idxR1AmountParda]='0'+p;
                infoStateTrucoIdP2[idxR1ThrownCard]='0'+valorTrucoAbs5GivenCard[handP1[idxCard]];
                //stateP1
                infoStateTrucoIdP1[idxR1ThrownCard]='0'+idxCard;
                //history
                history+="(TP1)";

                handP1.erase(handP1.begin()+idxCard);
                isP1TrucoTurn=false;
                isP1Turn=false;
            }else{
                mesa[1]=handP2[idxCard];
                handP2.erase(handP2.begin()+idxCard);
                history+="(TP2)";
            }
        }else if(currentRound==1){
            if(P1StartsInTruco){
                if(isP1TrucoTurn){
                    mesa[2]=handP1[idxCard];
                    //stateP1
                    infoStateTrucoIdP1[idxR2FThrownCard]='0'+idxCard;
                    //stateP2
                    int c=0,p=0;
                    for(auto card : handP2) if(utils.firstWinsTruco(card, handP1[idxCard])) c++;
                    for(auto card : handP2) if(utils.valorTruco(card)==utils.valorTruco(handP1[idxCard])) p++;
                    infoStateTrucoIdP2[idxR2SAmountBetter]='0'+c;
                    infoStateTrucoIdP2[idxR2SAmountParda]='0'+p;
                    infoStateTrucoIdP2[idxR2SThrownCard]='0'+valorTrucoAbs5GivenCard[handP1[idxCard]];
                    //history
                    history+="(TP1)";

                    handP1.erase(handP1.begin()+idxCard);
                    isP1TrucoTurn=false;
                    isP1Turn=false;
                }else{
                    mesa[3]=handP2[idxCard];
                    handP2.erase(handP2.begin()+idxCard);
                    history+="(TP2)";
                }
            }else{
                if(isP1TrucoTurn){
                    mesa[2]=handP1[idxCard];
                    handP1.erase(handP1.begin()+idxCard);
                    history+="(TP1)";
                }else{
                    mesa[3]=handP2[idxCard];
                    //stateP1
                    infoStateTrucoIdP2[idxR2FThrownCard]='0'+idxCard;
                    //stateP2
                    int c=0,p=0;
                    for(auto card : handP1) if(utils.firstWinsTruco(card, handP2[idxCard])) c++;
                    for(auto card : handP1) if(utils.valorTruco(card)==utils.valorTruco(handP2[idxCard])) p++;
                    infoStateTrucoIdP1[idxR2SAmountBetter]='0'+c;
                    infoStateTrucoIdP1[idxR2SAmountParda]='0'+p;
                    infoStateTrucoIdP1[idxR2SThrownCard]='0'+valorTrucoAbs5GivenCard[handP2[idxCard]];
                    //history
                    history+="(TP2)";

                    handP2.erase(handP2.begin()+idxCard);
                    isP1TrucoTurn=true;
                    isP1Turn=true;
                }
            }
        }else if(currentRound==2){
            if(isP1TrucoTurn){
                if(P1StartsInTruco){
                    mesa[4]=handP1[idxCard];
                    infoStateTrucoIdP1[idxR3thrownCardStart]=ptsToString(valorTrucoGivenCard[handP1[idxCard]])[0];
                    infoStateTrucoIdP1[idxR3thrownCardStart+1]=ptsToString(valorTrucoGivenCard[handP1[idxCard]])[1];
                    infoStateTrucoIdP2[idxR3thrownCardStart]=ptsToString(valorTrucoGivenCard[handP1[idxCard]])[0];
                    infoStateTrucoIdP2[idxR3thrownCardStart+1]=ptsToString(valorTrucoGivenCard[handP1[idxCard]])[1];
                    isP1TrucoTurn=false;
                    isP1Turn=false;
                    history+="(TP1)";
                }else{
                    mesa[4]=handP1[idxCard];
                }
            }else{
                if(P1StartsInTruco){
                    mesa[5]=handP2[idxCard];
                }else{
                    mesa[5]=handP2[idxCard];
                    infoStateTrucoIdP1[idxR3thrownCardStart]=ptsToString(valorTrucoGivenCard[handP2[idxCard]])[0];
                    infoStateTrucoIdP1[idxR3thrownCardStart+1]=ptsToString(valorTrucoGivenCard[handP2[idxCard]])[1];
                    infoStateTrucoIdP2[idxR3thrownCardStart]=ptsToString(valorTrucoGivenCard[handP2[idxCard]])[0];
                    infoStateTrucoIdP2[idxR3thrownCardStart+1]=ptsToString(valorTrucoGivenCard[handP2[idxCard]])[1];
                    isP1TrucoTurn=true;
                    isP1Turn=true;
                    history+="(TP2)";
                }
            }
        }
        
        if(mesa[2*currentRound]!="" && mesa[2*currentRound+1]!=""){
            if(utils.firstWinsTruco(mesa[2*currentRound], mesa[2*currentRound+1])){
                roundWinsP1++;
                isP1TrucoTurn=true;
                isP1Turn=true;
                P1StartsInTruco=true;
                whoWonHistory+="1";
            }else if(utils.firstWinsTruco(mesa[2*currentRound+1], mesa[2*currentRound])){
                roundWinsP2++;
                isP1TrucoTurn=false;
                isP1Turn=false;
                P1StartsInTruco=false;
                whoWonHistory+="2";
            }else{
                roundWinsP1++;
                roundWinsP2++;
                isP1TrucoTurn=true;
                isP1Turn=true;
                P1StartsInTruco=true;
                whoWonHistory+="E";
            }

            if(whoWinsRound[whoWonHistory]==1){
                terminal=true;
                gamePointsP1+=trucoValue;
            }else if(whoWinsRound[whoWonHistory]==2){
                terminal=true;
                gamePointsP2+=trucoValue;
            }else{
                currentRound++;
                if(currentRound==1){
                    if(P1StartsInTruco){
                        infoStateTrucoIdP1="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P1-R2F|";
                        infoStateTrucoIdP1+="[X]"+getBucketsAbs14Str(handP1)+"|";
                        infoStateTrucoIdP2="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P2-R2S|";
                        infoStateTrucoIdP2+=getBucketsAbs5Str(handP2)+"[X][M][P]|";
                    }else{
                        infoStateTrucoIdP1="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P1-R2S|";
                        infoStateTrucoIdP1+=getBucketsAbs5Str(handP1)+"[X][M][P]|";
                        infoStateTrucoIdP2="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P2-R2F|";
                        infoStateTrucoIdP2+="[X]"+getBucketsAbs14Str(handP2)+"|";
                    }
                }else if(currentRound==2){
                    if(P1StartsInTruco){
                        infoStateTrucoIdP1="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P1-R3F|";
                        infoStateTrucoIdP1+=getBucketsAbs14Str(handP1)+"[XX]|";
                        infoStateTrucoIdP2="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P2-R3S|";
                        infoStateTrucoIdP2+=getBucketsAbs14Str(handP2)+"[XX]|";
                    }else{
                        infoStateTrucoIdP1="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P1-R3S|";
                        infoStateTrucoIdP1+=getBucketsAbs14Str(handP1)+"[XX]|";
                        infoStateTrucoIdP2="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|P2-R3F|";
                        infoStateTrucoIdP2+=getBucketsAbs14Str(handP2)+"[XX]|";
                    }
                }
            }
            
        }
    }
    void betTruco(){
        if(isP1Turn){
            if(hasToRespondTruco) trucoValue++;
            hasToRespondTruco=true;
            isP1Turn=false;
            canBetTrucoP1=false;
            canBetTrucoP2=trucoValue<3;
        }else{
            if(hasToRespondTruco) trucoValue++;
            hasToRespondTruco=true;
            isP1Turn=true;
            canBetTrucoP2=false;
            canBetTrucoP1=trucoValue<3;
        }
        history+="(b)";
        informTrucoBet();
    }
    void acceptTruco(){
        informAcceptTruco();
        isP1Turn=isP1TrucoTurn;
        hasToRespondTruco=false;
        trucoValue++;
        history+="(q)";
    }
    void rejectTruco(){
        informRejectTruco();
        terminal=true;
        if(isP1Turn) gamePointsP2+=trucoValue;
        else gamePointsP1+=trucoValue;
    }
    
    void betFaltaEnvido(){
        appendToEnvidoInfoStates("(bF)");
        inEnvidoStageP1=true;
        inEnvidoStageP2=true;
        informBetFaltaEnvido();
        inFaltaEnvido=true;
        envidoValue=envidoBetValue;
        if(envidoValue==0) envidoValue=1;
        envidoBetValue=valueFaltaEnvido;
        hasToRespondEnvido=true;
        isP1Turn=!isP1Turn;
        canBetEnvidoP1=false;
        canBetEnvidoP2=false;
    }

    void betEnvido(int bet){
        appendToEnvidoInfoStates(getBetAction(bet));
        inEnvidoStageP1=true;
        inEnvidoStageP2=true;
        informBetEnvido(bet);
        envidoValue=envidoBetValue;
        if(envidoValue==0) envidoValue=1;
        envidoBetValue=bet;
        hasToRespondEnvido=true;
        isP1Turn=!isP1Turn;
        canBetEnvidoP1=isP1Turn;
        canBetEnvidoP2=!isP1Turn;
    }

    void rejectEnvido(){
        inEnvidoStageP1=false;
        inEnvidoStageP2=false;
        canBetEnvidoP1=false;
        canBetEnvidoP2=false;
        hasToRespondEnvido=false;

        informEnvidoReject();
        if(!isP1Turn){ //gana P1
            gamePointsP1+=envidoValue;
        }else{
            gamePointsP2+=envidoValue;
        }
        isP1Turn=isP1TrucoTurn;
        if(gamePointsP1>=30 || gamePointsP2>=30) terminal=true;
        else{
            string pathTruco = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/solvedStrategies/trucoStrategies/";
            string fileNameStrategyTruco = pathTruco+"trucoStrategies"+createIdOfEnvidoPointsBothPlayer(getGamePointsP1(),getGamePointsP2())+".txt";
            ifstream FileInTruco(fileNameStrategyTruco);
            string id, act;
            int cantActions;
            double p;
            FileInTruco>>p;
            while(FileInTruco>>id){
                FileInTruco>>cantActions;
                for (int i = 0; i < cantActions; i++)
                {
                    FileInTruco>>act;
                    FileInTruco>>p;
                    strategies[id].addAction(act, p);
                }
            }
            FileInTruco.close();
        }
        infoStateTrucoIdP1="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|"+infoStateTrucoIdP1.substr(8);
        infoStateTrucoIdP2="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|"+infoStateTrucoIdP2.substr(8);
        if(hasToRespondTruco) isP1Turn=hasToRespondTrucoIsP1;
    }

    void acceptEnvido(){
        inEnvidoStageP1=false;
        inEnvidoStageP2=false;
        canBetEnvidoP1=false;
        canBetEnvidoP2=false;
        hasToRespondEnvido=false;
        envidoValue = envidoBetValue;
        isP1Turn=isP1TrucoTurn;

        if(envidoPointsP1>=envidoPointsP2){ //gana P1
            gamePointsP1+=envidoValue;
            informEnvidoPointsP1Wins();
        }else{
            gamePointsP2+=envidoValue;
            informEnvidoPointsP2Wins();
        }

        if(gamePointsP1>=30 || gamePointsP2>=30) terminal=true;
        else{
            string pathTruco = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/solvedStrategies/trucoStrategies/";
            string fileNameStrategyTruco = pathTruco+"trucoStrategies"+createIdOfEnvidoPointsBothPlayer(getGamePointsP1(),getGamePointsP2())+".txt";
            ifstream FileInTruco(fileNameStrategyTruco);
            string id, act;
            int cantActions;
            double p;
            FileInTruco>>p;
            while(FileInTruco>>id){
                FileInTruco>>cantActions;
                for (int i = 0; i < cantActions; i++)
                {
                    FileInTruco>>act;
                    FileInTruco>>p;
                    strategies[id].addAction(act, p);
                }
            }
            FileInTruco.close();
        }
        infoStateTrucoIdP1="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|"+infoStateTrucoIdP1.substr(8);
        infoStateTrucoIdP2="1|"+ptsToString(gamePointsP1)+"-"+ptsToString(gamePointsP2)+"|"+infoStateTrucoIdP2.substr(8);
        if(hasToRespondTruco) isP1Turn=hasToRespondTrucoIsP1;
    }

    void playerPassEnvido(){
        appendToEnvidoInfoStates("(p)");
        if(playerIsMano) inEnvidoStageP1=false;
        else inEnvidoStageP2=false;
    }

    //access
    bool isPlayerTurn(){
        return isP1Turn==playerIsMano;
    }
    bool gameHasFinished(){
        return gamePointsP1>29 || gamePointsP2>29;
    }
    bool roundHasFinished(){
        return terminal;
    }
    int getGamePointsP1(){
        return gamePointsP1;
    }
    int getGamePointsP2(){
        return gamePointsP2;
    }
    int getGamePointsPlayer(){
        return playerIsMano?gamePointsP1:gamePointsP2;
    }
    int getGamePointsAgent(){
        return playerIsMano?gamePointsP2:gamePointsP1;
    }
    bool getPlayerStartedAsMano(){
        return playerIsMano;
    }

    //informing
    void informPlayerActions(){
        informPlayersHand();
        if((playerIsMano && inEnvidoStageP1) || (!playerIsMano && inEnvidoStageP2)){
            cout<<"Envido stage"<<endl;
            cout<<"0: (p)"<<endl;
            cout<<"1: (q)"<<endl;
            cout<<"2: (nq)"<<endl;
            cout<<"3: (bX)"<<endl;
        }else{
            cout<<"Truco stage"<<endl;
            cout<<"4: (q)"<<endl;
            cout<<"5: (nq)"<<endl;
            cout<<"6: (bt)"<<endl;
            cout<<"7: (T1)"<<endl;
            cout<<"8: (T2)"<<endl;
            cout<<"9: (T3)"<<endl;
        }
    }
    void informRejectTruco(){
        cout<<string(7,'+')<<endl;
        cout<<(isP1Turn?"P1":"P2");
        cout<<" rejects truco bet"<<endl;
        cout<<string(7,'+')<<endl;
    }
    void informAcceptTruco(){
        cout<<string(7,'+')<<endl;
        cout<<(isP1Turn?"P1":"P2");
        cout<<" acepta truco bet"<<endl;
        cout<<string(7,'+')<<endl;
    }
    void informTrucoBet(){
        cout<<string(7,'+')<<endl;
        cout<<(isP1Turn?"P1":"P2");
        cout<<" made truco bet: "<<trucoValue+1<<endl;
        cout<<string(7,'+')<<endl;
    }
    void informBetFaltaEnvido(){
        cout<<string(7,'+')<<endl;
        cout<<(isP1Turn?"P1":"P2");
        cout<<" made falta envido bet"<<endl;
        cout<<string(7,'+')<<endl;
    }
    void informBetEnvido(int bet){
        cout<<string(7,'+')<<endl;
        cout<<(isP1Turn?"P1":"P2");
        cout<<" made envido bet: "<<bet<<endl;
        cout<<string(7,'+')<<endl;
    }
    void informEnvidoReject(){
        cout<<string(7,'+')<<endl;
        cout<<(isP1Turn?"P1":"P2");
        cout<<" rejected envido"<<endl;
        cout<<string(7,'+')<<endl;
    }
    void informPlayersHand(){
        cout<<"Players hand: ";
        if(playerIsMano){
            for(auto card : handP1) cout<<card<<" ";
        }else{
            for(auto card : handP2) cout<<card<<" ";
        }
        cout<<endl;
    }
    void informMesa(){
        cout<<string(6,'/')<<endl;
        cout<<"Mesa:"<<endl;
        for (int i = 1; i < 6; i+=2) cout<<mesa[i]<<" ";
        cout<<endl;
        for (int i = 0; i < 6; i+=2) cout<<mesa[i]<<" ";
        cout<<endl;
        cout<<string(6,'/')<<endl;
    }
    void informEnvidoPointsP1Wins(){
        cout<<string(7,'+')<<endl;
        cout<<"P1 wins envido with "<<envidoPointsP1<<endl;
        cout<<string(7,'+')<<endl;
    }
    void informEnvidoPointsP2Wins(){
        cout<<string(7,'+')<<endl;
        cout<<"P2 wins envido with "<<envidoPointsP2<<endl;
        cout<<"P1 had "<<envidoPointsP1<<" points"<<endl;
        cout<<string(7,'+')<<endl;
    }
    void informWin(){
        if(playerIsMano){
            if(gamePointsP1>=30){
                cout<<"PLAYER WON"<<endl;
            } else cout <<"AGENT WON"<<endl;
        } else{
            if(gamePointsP2>=30){
                cout<<"PLAYER WON"<<endl;
            } else cout <<"AGENT WON"<<endl;
        }
        cout<<gamePointsP1<<"-"<<gamePointsP2<<endl;
        system("pause");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    void informPlayersGamePoints(){
        if(playerIsMano){
            cout<<"Players GP: "<<gamePointsP1<<endl;
            cout<<"Agents GP: "<<gamePointsP2<<endl;
        }else{
            cout<<"Players GP: "<<gamePointsP2<<endl;
            cout<<"Agents GP: "<<gamePointsP1<<endl;
        }
    }


    private:
    int gamePointsPlayer, gamePointsAgent;
    int gamePointsP1, gamePointsP2; 
    bool playerIsMano;
    bool terminal;
    vector<string> mesa;
    vector<string> handP1;
    vector<bool> handP1Used;
    vector<string> handP2;
    vector<bool> handP2Used;
    int currentRound;
    int trucoValue;
    bool canBetTrucoP1;
    bool canBetTrucoP2;
    bool hasToRespondTruco;
    bool isP1TrucoTurn;
    bool isP1Turn;
    bool P1StartsInTruco;
    bool hasToRespondTrucoIsP1;
    int roundWinsP1;
    int roundWinsP2;
    int envidoValue;
    int envidoBetValue;
    int canBetEnvidoP1;
    int canBetEnvidoP2;
    int hasToRespondEnvido;
    int envidoPointsP1;
    int envidoPointsP2;
    int valueFaltaEnvido;
    bool inFaltaEnvido;

    string infoStateTrucoIdP1;
    string infoStateTrucoIdP2;
    int idxR1AmountBetter; 
    int idxR1AmountParda; 
    int idxR1ThrownCard;
    int idxR2SAmountBetter; 
    int idxR2SAmountParda; 
    int idxR2SThrownCard;
    int idxR2FThrownCard;
    int idxR3thrownCardStart;
    bool playerStartedAsMano;

    string infoStateEnvidoP1; //historia compartida
    string infoStateEnvidoP2;
    string history;
    string whoWonHistory;

    bool inEnvidoStageP1;
    bool inEnvidoStageP2;

    vector<string> getActionsArray(string action){
        deque<char> q;
        for(char ch : action) q.push_back(ch);
        vector<string> actions;
        string act = "";
        while(q.size()){
            act+=q.front();
            if(q.front()==')'){
                actions.push_back(act);
                act="";
            }
            q.pop_front();
        }
        return actions;
    }

    void appendToEnvidoInfoStates(string action){
        infoStateEnvidoP1+=action;
        infoStateEnvidoP2+=action;
    }

    string getBucketsAbs5Str(vector<string> & v){
        vector<string> b;
        for (string card : v)
        {
            b.push_back("["+to_string(valorTrucoAbs5GivenCard[card])+"]");
        }
        sort(b.begin(),b.end());
        string res="";
        for(string bucketId : b) res+=bucketId;
        return res;
    }
    string getBucketsAbs14Str(vector<string> & v){
        vector<string> b;
        for (string card : v)
        {
            b.push_back("["+ptsToString(valorTrucoGivenCard[card])+"]");
        }
        sort(b.begin(),b.end());
        string res="";
        for(string bucketId : b) res+=bucketId;
        return res;
    }
    
    string getBetAction(int betSize){//not for (bf)
        return "(b"+to_string(betSize)+")";
    }
};

int main(){
    {
        valorTrucoAbs5GivenCard["01e"] = 1;  
        valorTrucoAbs5GivenCard["01b"] = 1;  
        valorTrucoAbs5GivenCard["07e"] = 1;  
        valorTrucoAbs5GivenCard["07o"] = 1;  

        valorTrucoAbs5GivenCard["03b"] = 2;  
        valorTrucoAbs5GivenCard["03c"] = 2;  
        valorTrucoAbs5GivenCard["03e"] = 2;  
        valorTrucoAbs5GivenCard["03o"] = 2;  

        valorTrucoAbs5GivenCard["02b"] = 2;  
        valorTrucoAbs5GivenCard["02e"] = 2;  
        valorTrucoAbs5GivenCard["02c"] = 2;  
        valorTrucoAbs5GivenCard["02o"] = 2;  

        valorTrucoAbs5GivenCard["01c"] = 3;  
        valorTrucoAbs5GivenCard["01o"] = 3;  

        valorTrucoAbs5GivenCard["12c"] = 3; 
        valorTrucoAbs5GivenCard["12e"] = 3; 
        valorTrucoAbs5GivenCard["12o"] = 3; 
        valorTrucoAbs5GivenCard["12b"] = 3; 

        valorTrucoAbs5GivenCard["11b"] = 3;  
        valorTrucoAbs5GivenCard["11o"] = 3;  
        valorTrucoAbs5GivenCard["11e"] = 3;  
        valorTrucoAbs5GivenCard["11c"] = 3;  

        valorTrucoAbs5GivenCard["10b"] = 4;  
        valorTrucoAbs5GivenCard["10o"] = 4;  
        valorTrucoAbs5GivenCard["10c"] = 4;  
        valorTrucoAbs5GivenCard["10e"] = 4;  

        valorTrucoAbs5GivenCard["07c"] = 4;  
        valorTrucoAbs5GivenCard["07b"] = 4;  

        valorTrucoAbs5GivenCard["06c"] = 5;  
        valorTrucoAbs5GivenCard["06e"] = 5;  
        valorTrucoAbs5GivenCard["06b"] = 5;  
        valorTrucoAbs5GivenCard["06o"] = 5;  

        valorTrucoAbs5GivenCard["05b"] = 5;  
        valorTrucoAbs5GivenCard["05o"] = 5;  
        valorTrucoAbs5GivenCard["05e"] = 5;  
        valorTrucoAbs5GivenCard["05c"] = 5;  

        valorTrucoAbs5GivenCard["04b"] = 5;  
        valorTrucoAbs5GivenCard["04c"] = 5;  
        valorTrucoAbs5GivenCard["04e"] = 5;  
        valorTrucoAbs5GivenCard["04o"] = 5;  
    }
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
    {
        whoWinsRound["11"] = 1;
        whoWinsRound["22"] = 2;
        whoWinsRound["E1"] = 1;
        whoWinsRound["E2"] = 2;
        whoWinsRound["1E"] = 1;
        whoWinsRound["2E"] = 2;

        whoWinsRound["121"] = 1;
        whoWinsRound["122"] = 2;
        whoWinsRound["12E"] = 1;
        whoWinsRound["212"] = 2;
        whoWinsRound["211"] = 1;
        whoWinsRound["21E"] = 2;
        whoWinsRound["EEE"] = 1;
        whoWinsRound["EE1"] = 1;
        whoWinsRound["EE2"] = 2;
    }
    
    int gamePointsPlayer=15, gamePointsAgent=15;
    bool playerStartsAsMano = true;
    Game game(gamePointsPlayer, gamePointsAgent, playerStartsAsMano);
    while(true){
        //get init strategies
        strategies.clear();

        string pathTruco = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/solvedStrategies/trucoStrategies/";
        string fileNameStrategyTruco = pathTruco+"trucoStrategies"+createIdOfEnvidoPointsBothPlayer(game.getGamePointsP1(),game.getGamePointsP2())+".txt";
        ifstream FileInTruco(fileNameStrategyTruco);
        string id, act;
        int cantActions;
        double p;
        FileInTruco>>p;
        while(FileInTruco>>id){
            FileInTruco>>cantActions;
            for (int i = 0; i < cantActions; i++)
            {
                FileInTruco>>act;
                FileInTruco>>p;
                strategies[id].addAction(act, p);
            }
        }
        FileInTruco.close();

        string pathEnvido = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/solvedStrategies/envidoStrategies/";
        string fileNameStrategyEnvido = pathEnvido+"strategyEnvido"+createIdOfEnvidoPointsBothPlayer(game.getGamePointsP1(),game.getGamePointsP2())+".txt";
        ifstream FileInEnvido(fileNameStrategyEnvido);
        FileInEnvido>>p;
        while(FileInEnvido>>id){
            FileInEnvido>>cantActions;
            for (int i = 0; i < cantActions; i++)
            {
                FileInEnvido>>act;
                FileInEnvido>>p;
                strategies[id].addAction(act, p);
            }
        }
        FileInEnvido.close();

        //play
        cout<<string(20,'*')<<endl;
        cout<<"NEW ROUND"<<endl;
        game.informPlayersGamePoints();
        game.informPlayersHand();
        while(!game.roundHasFinished()){
            cout<<string(15,'-')<<endl;
            if(game.isPlayerTurn()){
                cout<<"PLAYERS turn"<<endl;
                game.informPlayerActions();
                int actId;
                cin>>actId;
                if(actId==0) game.playerPassEnvido();
                else if(actId==1) game.acceptEnvido();
                else if(actId==2) game.rejectEnvido();
                else if(actId==3){
                    cout<<"Choose valid bet: ";
                    cin>>actId;
                    if(actId==30) game.betFaltaEnvido();
                    else game.betEnvido(actId);
                    cout<<endl;
                }
                else if(actId==4) game.acceptTruco();
                else if(actId==5) game.rejectTruco();
                else if(actId==6) game.betTruco();
                else if(actId==7) game.throwCard(0);
                else if(actId==8) game.throwCard(1);
                else if(actId==9) game.throwCard(2);
            }else{
                cout<<"AGENTS turn"<<endl;
                game.playAgent();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }  

        if(game.gameHasFinished()) break;
        game = Game(game.getGamePointsPlayer(), game.getGamePointsAgent(), !game.getPlayerStartedAsMano());
    }
    game.informWin();
}