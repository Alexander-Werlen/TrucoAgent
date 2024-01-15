#include <windows.h>
#include "../source/auxiliares/cardUtils.hpp"
#include "../source/auxiliares/deck.hpp"
#include <chrono>
#include <map>
#include <vector>
#include <set>
#include <random>
#include <fstream>
#include <iostream>

using namespace std;

cardUtils utils;

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

double lambdaFactor(int iteration){
    return ((double)iteration/(iteration+1))*((double)iteration/(iteration+1));
}
double betaFactor(int iteration){
    return 0.5;
}
double gammaFactor(int iteration){
    return (double)pow(iteration, 1.5)/(pow(iteration, 1.5)+1);
}

class InfoState{
    public:
    //aux
    bool seen;
    //basic
    string id;
    int cantActions;
    vector<string> actions;
    //cfr
    double reachPr;
    vector<double> strategy;
    vector<double> regretSum;
    vector<double> strategySum;

    InfoState(){
        seen=false;
        cantActions=0;
    }

    void addAction(string act){
        actions.push_back(act);
        cantActions++;
    }
    void initialize(){
        //assumes input of basic data was done
        reachPr=0;
        for (int i = 0; i < cantActions; i++)
        {
            strategy.push_back((double)1/cantActions);
            strategySum.push_back(0);
            regretSum.push_back(0);
        }
    }
    void updateStrategy(){
        for (int i = 0; i < cantActions; i++)
        {
            strategySum[i]+=reachPr*strategy[i];
        }
        strategy=getStrategy();
        reachPr=0;
    }
    vector<double> getStrategy(){
        vector<double> regrets;
        double normalizingSum=0;
        for (int i = 0; i < cantActions; i++)
        {
            regrets.push_back(regretSum[i]>=0 ? regretSum[i] : 0);
            normalizingSum+=regrets.back();
        }

        if(normalizingSum>0) {
            for (int i = 0; i < cantActions; i++)
            {
                regrets[i]=regrets[i]/normalizingSum;
            }
        } else{
            for (int i = 0; i < cantActions; i++)
            {
                regrets[i]=(double)1/cantActions;
            }
        }
        return regrets;
    }
    vector<double> get_average_strategy(){
        double reachPrSum=0;
        for (int i = 0; i < cantActions; i++)
        {
            reachPrSum+=strategySum[i];
        }
        if(reachPrSum==0) return vector<double> (cantActions, (double)1/cantActions);
        vector<double> strategyAv = strategySum;
        for (int i = 0; i < cantActions; i++)
        {
            strategyAv[i]/=reachPrSum;
        }
        return strategyAv;
    }
    void discount(int currentIteration){
        for(double & e : regretSum){
            if(e>=0) e*=gammaFactor(currentIteration);
            else e*=betaFactor(currentIteration);
        }
        for(double & e : strategySum) e*=lambdaFactor(currentIteration);
    }

};

map<string,int> valorTrucoGivenCard;
map<string,int> valorTrucoAbs5GivenCard;
map<string,int> whoWinsRound;
map<string,double> trucoStateValues;
map<string,double> envidoStateValues;

class Game{
    public:
    vector<string> mesa;
    int gamePts1,gamePts2;
    int currentRound;
    int trucoValue;
    bool hasToRespond;
    bool canBetP1;
    bool canBetP2;
    int isP1TrucoTurn;
    int isP1Turn;
    int roundWinsP1;
    int roundWinsP2;
    vector<string> handP1;
    vector<bool> handP1Used;
    vector<string> handP2;
    vector<bool> handP2Used;
    bool terminal;
    string infoStateIdP1;
    string infoStateIdP2;
    Game(vector<string> & cards, int pts1, int pts2){
        gamePts1=pts1;
        gamePts2=pts2;
        mesa = {"","","","","",""};
        handP1Used = {false,false,false};
        handP2Used = {false,false,false};
        currentRound=0;
        trucoValue=1;
        canBetP1=true;
        canBetP2=true;
        hasToRespond=false;
        isP1TrucoTurn=true;
        isP1Turn=true;
        P1StartsInTruco=true;
        roundWinsP1=0;
        roundWinsP2=0;
        terminal=false;
        for (int i = 0; i < 3; i++)
        {
            handP1.push_back(cards[i]);
            handP2.push_back(cards[i+3]);
        }
        
        utils.formatHandTrucoValue(handP1);
        utils.formatHandTrucoValue(handP2);

        //infoStates R1
        infoStateIdP1="1|"+ptsToString(pts1)+"-"+ptsToString(pts2)+"|P1-R1|";
        infoStateIdP1+=getBucketsAbs5Str(handP1)+"[X]|";
        infoStateIdP2="1|"+ptsToString(pts1)+"-"+ptsToString(pts2)+"|P2-R1|";
        infoStateIdP2+=getBucketsAbs5Str(handP2)+"[X][M][P]|";
        history="";
        whoWonHistory="";
        //idxs
        idxR1ThrownCard=24;
        idxR1AmountBetter=27;
        idxR1AmountParda=30;
        idxR2FThrownCard=16;
        idxR2SThrownCard=22;
        idxR2SAmountBetter=25; 
        idxR2SAmountParda=28; 
        idxR3thrownCardStart=20;
    }

    void throwCard(int idxCard){//assumes that it is valid, 0-indexed
        if(currentRound==0){
            if(isP1TrucoTurn){
                mesa[0]=handP1[idxCard];
                //stateP2
                int c=0,p=0;
                for(auto card : handP2) if(utils.firstWinsTruco(card, handP1[idxCard])) c++;
                for(auto card : handP2) if(utils.valorTruco(card)==utils.valorTruco(handP1[idxCard])) p++;
                infoStateIdP2[idxR1AmountBetter]='0'+c;
                infoStateIdP2[idxR1AmountParda]='0'+p;
                infoStateIdP2[idxR1ThrownCard]='0'+valorTrucoAbs5GivenCard[handP1[idxCard]];
                //stateP1
                infoStateIdP1[idxR1ThrownCard]='0'+idxCard;
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
                    infoStateIdP1[idxR2FThrownCard]='0'+idxCard;
                    //stateP2
                    int c=0,p=0;
                    for(auto card : handP2) if(utils.firstWinsTruco(card, handP1[idxCard])) c++;
                    for(auto card : handP2) if(utils.valorTruco(card)==utils.valorTruco(handP1[idxCard])) p++;
                    infoStateIdP2[idxR2SAmountBetter]='0'+c;
                    infoStateIdP2[idxR2SAmountParda]='0'+p;
                    infoStateIdP2[idxR2SThrownCard]='0'+valorTrucoAbs5GivenCard[handP1[idxCard]];
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
                    infoStateIdP2[idxR2FThrownCard]='0'+idxCard;
                    //stateP2
                    int c=0,p=0;
                    for(auto card : handP1) if(utils.firstWinsTruco(card, handP2[idxCard])) c++;
                    for(auto card : handP1) if(utils.valorTruco(card)==utils.valorTruco(handP2[idxCard])) p++;
                    infoStateIdP1[idxR2SAmountBetter]='0'+c;
                    infoStateIdP1[idxR2SAmountParda]='0'+p;
                    infoStateIdP1[idxR2SThrownCard]='0'+valorTrucoAbs5GivenCard[handP2[idxCard]];
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
                    infoStateIdP1[idxR3thrownCardStart]=ptsToString(valorTrucoGivenCard[handP1[idxCard]])[0];
                    infoStateIdP1[idxR3thrownCardStart+1]=ptsToString(valorTrucoGivenCard[handP1[idxCard]])[1];
                    infoStateIdP2[idxR3thrownCardStart]=ptsToString(valorTrucoGivenCard[handP1[idxCard]])[0];
                    infoStateIdP2[idxR3thrownCardStart+1]=ptsToString(valorTrucoGivenCard[handP1[idxCard]])[1];
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
                    infoStateIdP1[idxR3thrownCardStart]=ptsToString(valorTrucoGivenCard[handP2[idxCard]])[0];
                    infoStateIdP1[idxR3thrownCardStart+1]=ptsToString(valorTrucoGivenCard[handP2[idxCard]])[1];
                    infoStateIdP2[idxR3thrownCardStart]=ptsToString(valorTrucoGivenCard[handP2[idxCard]])[0];
                    infoStateIdP2[idxR3thrownCardStart+1]=ptsToString(valorTrucoGivenCard[handP2[idxCard]])[1];
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
                gamePts1+=trucoValue;
            }else if(whoWinsRound[whoWonHistory]==2){
                terminal=true;
                gamePts2+=trucoValue;
            }else{
                currentRound++;
                if(currentRound==1){
                    if(P1StartsInTruco){
                        infoStateIdP1="1|"+ptsToString(gamePts1)+"-"+ptsToString(gamePts2)+"|P1-R2F|";
                        infoStateIdP1+="[X]"+getBucketsAbs14Str(handP1)+"|";
                        infoStateIdP2="1|"+ptsToString(gamePts1)+"-"+ptsToString(gamePts2)+"|P2-R2S|";
                        infoStateIdP2+=getBucketsAbs5Str(handP2)+"[X][M][P]|";
                    }else{
                        infoStateIdP1="1|"+ptsToString(gamePts1)+"-"+ptsToString(gamePts2)+"|P1-R2S|";
                        infoStateIdP1+=getBucketsAbs5Str(handP1)+"[X][M][P]|";
                        infoStateIdP2="1|"+ptsToString(gamePts1)+"-"+ptsToString(gamePts2)+"|P2-R2F|";
                        infoStateIdP2+="[X]"+getBucketsAbs14Str(handP2)+"|";
                    }
                }else if(currentRound==2){
                    if(P1StartsInTruco){
                        infoStateIdP1="1|"+ptsToString(gamePts1)+"-"+ptsToString(gamePts2)+"|P1-R3F|";
                        infoStateIdP1+=getBucketsAbs14Str(handP1)+"[XX]|";
                        infoStateIdP2="1|"+ptsToString(gamePts1)+"-"+ptsToString(gamePts2)+"|P2-R3S|";
                        infoStateIdP2+=getBucketsAbs14Str(handP2)+"[XX]|";
                    }else{
                        infoStateIdP1="1|"+ptsToString(gamePts1)+"-"+ptsToString(gamePts2)+"|P1-R3S|";
                        infoStateIdP1+=getBucketsAbs14Str(handP1)+"[XX]|";
                        infoStateIdP2="1|"+ptsToString(gamePts1)+"-"+ptsToString(gamePts2)+"|P2-R3F|";
                        infoStateIdP2+=getBucketsAbs14Str(handP2)+"[XX]|";
                    }
                }
            }
            
        }
    }
    void bet(){//asume que se puede
        if(isP1Turn){
            if(hasToRespond) trucoValue++;
            hasToRespond=true;
            isP1Turn=false;
            canBetP1=false;
            canBetP2=trucoValue<3;
        }else{
            if(hasToRespond) trucoValue++;
            hasToRespond=true;
            isP1Turn=true;
            canBetP2=false;
            canBetP1=trucoValue<3;
        }
        history+="(b)";
    }
    void quiero(){//asume que es valido
        isP1Turn=isP1TrucoTurn;
        hasToRespond=false;
        trucoValue++;
        history+="(q)";
    }
    void reject(){//asume que es valido
        terminal=true;
        if(isP1Turn) gamePts2+=trucoValue;
        else gamePts1+=trucoValue;
    }
    
    string getState() const{
        if(isP1Turn) return infoStateIdP1+"["+whoWonHistory+"]"+history;
        else return infoStateIdP2+"["+whoWonHistory+"]"+history;
    }
    private:
    int idxR1AmountBetter; 
    int idxR1AmountParda; 
    int idxR1ThrownCard;
    int idxR2SAmountBetter; 
    int idxR2SAmountParda; 
    int idxR2SThrownCard;
    int idxR2FThrownCard;
    int idxR3thrownCardStart;


    bool P1StartsInTruco;
    string history;
    string whoWonHistory;
    
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
};

map<string, InfoState> infoStates;
class Truco{
    public:
    double expectedGameValue;
    Truco(){}

    void train(int Niterations, int pts1, int pts2){
        string path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/chanceSamples/";
        string fileName = path+"samplesTruco"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)+".txt";
        ifstream FileSamples(fileName);
        expectedGameValue=0;
        for (int i = 1; i <= Niterations; i++)
        {
            if(i%100==0) cout<<i<<" "<<infoStates.size()<<endl;
            vector<string> cards(6);
            for (int i = 0; i < 6; i++) FileSamples>>cards[i];
            Game game(cards, pts1, pts2);

            expectedGameValue+=cfr(game, 1, 1);
            for(auto & e : infoStates)e.second.updateStrategy();
            for(auto & e : infoStates)e.second.discount(i);
        }
        expectedGameValue/=Niterations;
        FileSamples.close();
    }

    double cfr(Game const & game, double pr1, double pr2){
        
        if(game.terminal){
            if(game.gamePts1>=30) return 50;
            else if(game.gamePts2>=30) return -50;
            if(envidoStateValues.find("e"+createIdOfEnvidoPointsBothPlayer(game.gamePts2,game.gamePts1))==envidoStateValues.end()){
                while(true) cout<<"Envido init state not found yet"<<endl;
            }else{
                return -1*envidoStateValues["e"+createIdOfEnvidoPointsBothPlayer(game.gamePts2,game.gamePts1)];
            }
        }
        
        vector<pair<string, Game>> actionsStorage;
        {//Actions
        if(game.hasToRespond){
            {
                if(game.isP1Turn){
                {// quiero
                    Game newGame = game;
                    newGame.quiero();
                    actionsStorage.push_back({"(q)", newGame});
                }
                {// rebet
                    if(game.isP1TrucoTurn==false && game.canBetP1){
                        Game newGame = game;
                        newGame.bet();
                        actionsStorage.push_back({"(b)", newGame});
                    }
                }
                {// no quiero
                    Game newGame = game;
                    newGame.reject();
                    actionsStorage.push_back({"(nq)", newGame});
                }
                }
                else{//P2
                {// quiero
                    Game newGame = game;
                    newGame.quiero();
                    actionsStorage.push_back({"(q)", newGame});
                }
                {// rebet
                    if((game.isP1TrucoTurn==true) && game.canBetP2){
                        Game newGame = game;
                        newGame.bet();
                        actionsStorage.push_back({"(b)", newGame});
                    }
                }
                {// no quiero
                    Game newGame = game;
                    newGame.reject();
                    actionsStorage.push_back({"(nq)", newGame});
                }
                }
            }
        }
        else{//!hasToRespond
            {//bet
                if(game.isP1Turn && game.canBetP1){//P1
                    Game newGame = game;
                    newGame.bet();
                    actionsStorage.push_back({"(b)", newGame});
                }else if(!game.isP1Turn && game.canBetP2){//P2
                    Game newGame = game;
                    newGame.bet();
                    actionsStorage.push_back({"(b)", newGame});
                }
            }
            {//tirar carta
                if(game.isP1Turn){//P1
                    if(game.handP1.size()>=1){//mayor
                        Game newGame = game;
                        newGame.throwCard(0);
                        actionsStorage.push_back({"(T1)", newGame});
                    }
                    if(game.handP1.size()>=2){//media
                        Game newGame = game;
                        newGame.throwCard(1);
                        actionsStorage.push_back({"(T2)", newGame});
                    }
                    if(game.handP1.size()>=3){//menor
                        Game newGame = game;
                        newGame.throwCard(2);
                        actionsStorage.push_back({"(T3)", newGame});
                    }
                }else{ //P2
                    if(game.handP2.size()>=1){//mayor
                        Game newGame = game;
                        newGame.throwCard(0);
                        actionsStorage.push_back({"(T1)", newGame});
                    }
                    if(game.handP2.size()>=2){//media
                        Game newGame = game;
                        newGame.throwCard(1);
                        actionsStorage.push_back({"(T2)", newGame});
                    }
                    if(game.handP2.size()>=3){//menor
                        Game newGame = game;
                        newGame.throwCard(2);
                        actionsStorage.push_back({"(T3)", newGame});
                    }
                }
            }
        }
        }
        if(actionsStorage.size()==1){ //redundant state
            return cfr(actionsStorage[0].second, pr1, pr2);
        }else{
            InfoState & infoState = infoStates[game.getState()];
        
            if(!infoState.seen){
                infoState.seen=true;
                infoState.id=(game.getState());
                for(auto action : actionsStorage){
                    infoState.addAction(action.first);
                }
                infoState.initialize();
            }else{//debug
                if(infoState.cantActions!=actionsStorage.size()){
                    cout<<"Error infostate con diferentes acciones posibles"<<endl;
                }
            }

            vector<double> actionUtils (infoState.cantActions,0);

            for (int act = 0; act < infoState.cantActions; act++)
            {   
                if(game.isP1Turn){
                    actionUtils[act] = cfr(actionsStorage[act].second, pr1*infoState.strategy[act], pr2);
                } else{
                    actionUtils[act] = -1*cfr(actionsStorage[act].second, pr1, pr2*infoState.strategy[act]);
                }
            }
            double util=0;
            for (int i = 0; i < infoState.cantActions; i++)
            {
                util+=actionUtils[i]*infoState.strategy[i];
            }
            vector<double> regrets;
            for (int i = 0; i < infoState.cantActions; i++)
            {
                regrets.push_back(actionUtils[i]-util);
            }
            if(game.isP1Turn){
                infoState.reachPr+=pr1;
                for (int i = 0; i < infoState.cantActions; i++)
                {
                    infoState.regretSum[i]+=pr2*regrets[i];
                }
            }else{
                infoState.reachPr+=pr2;
                for (int i = 0; i < infoState.cantActions; i++)
                {
                    infoState.regretSum[i]+=pr1*regrets[i];
                }
            }
            
            return game.isP1Turn ? util : -util;
        }
    } 
};

int main(){
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
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
    //get initStatesValues
    string path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/solvedStrategies/initStateValues/";
    string fileNameValues = path+"initStateValues.txt";
    ifstream FileInValues(fileNameValues);
    string state;
    double value;
    while(FileInValues>>state){
        FileInValues>>value;
        if(state[0]=='t') trucoStateValues[state]=value;
        else if(state[0]=='e') envidoStateValues[state]=value;
    }
    FileInValues.close();
    
    for (int pts1 = 15; pts1 < 30; pts1++)
    {
        for (int pts2 = 15; pts2 < 30; pts2++)
        {
            //check if can solve
            if(trucoStateValues.find("t"+createIdOfEnvidoPointsBothPlayer(pts1,pts2))!=trucoStateValues.end()) continue;
            bool can=true;
            for (int i = 1; i < 5; i++)
            {
                if(pts1+i<30 && envidoStateValues.find("e"+createIdOfEnvidoPointsBothPlayer(pts1+i,pts2))==envidoStateValues.end()) can=false;
                if(pts2+i<30 && envidoStateValues.find("e"+createIdOfEnvidoPointsBothPlayer(pts1,pts2+i))==envidoStateValues.end()) can=false;
            }
            if(!can) continue;
               
            cout<<"Current: "<<pts1<<"-"<<pts2<<endl;
            //clear previous data
            infoStates.clear();

            //start cfr
            Truco agent;
            agent.train(1e4, pts1, pts2);

            //store state value
            trucoStateValues["t"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)]=agent.expectedGameValue;
            //output results
            string path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/solvedStrategies/trucoStrategies/";
            string fileNameStrategy = path+"trucoStrategies"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)+".txt";
            ofstream FileOut(fileNameStrategy);
            FileOut<<agent.expectedGameValue<<endl;
            for(auto e : infoStates){
                FileOut<<e.second.id<<" "<<e.second.cantActions<<endl;
                vector<double> strategy = e.second.get_average_strategy();
                for (int act = 0; act < e.second.cantActions; act++)
                {
                    FileOut<<e.second.actions[act]<<" "<<strategy[act]<<endl;
                }
            }
            FileOut.close();

        }
    }
    //output statesValues
    path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/solvedStrategies/initStateValues/";
    fileNameValues = path+"initStateValues.txt";
    ofstream FileOutValues(fileNameValues);
    for(auto state : trucoStateValues){
        FileOutValues<<state.first<<" "<<state.second<<endl;
    }
    for(auto state : envidoStateValues){
        FileOutValues<<state.first<<" "<<state.second<<endl;
    }
    FileOutValues.close();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time computation = " << std::chrono::duration_cast<std::chrono::minutes>(end - begin).count() << " minutes" << std::endl;

    ShellExecute(NULL, "open", "C:\\Users\\Usuario\\Desktop\\PC\\Yo\\QUE HACER\\PROGRAMAR\\Proyectos\\TrucoSolver\\TrucoWebAgent\\Out\\Build\\solvers\\Release\\envidoSolver.exe", NULL, NULL, SW_SHOWDEFAULT);

}