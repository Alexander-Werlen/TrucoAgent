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

        //infoStates
        infoStateIdP1="1|"+ptsToString(pts1)+"-"+ptsToString(pts2)+"|";
        infoStateIdP1+=getBucketsStr(handP1)+"[X][P][LLL][GGG][1][t][f][0]";
        infoStateIdP2="1|"+ptsToString(pts1)+"-"+ptsToString(pts2)+"|";
        infoStateIdP2+=getBucketsStr(handP2)+"[X][P][LLL][GGG][1][t][f][1]";
        //idxs
        idxAmountBetter=18;
        idxAmountParda=21;
        idxFirstColThrow=24;
        idxFirstWin=29;
        idxTrucoValue=34;
        idxCanBet=37;
        idxHasToRespond=40;
    }

    void throwCard(int idxCard){//assumes that it is valid
        if(isP1TrucoTurn){
            mesa[2*currentRound]=handP1[idxCard];
            handP1Used[idxCard]=true;
            isP1TrucoTurn=false;
            isP1Turn=false;
            if(mesa[2*currentRound+1]==""){
                int c=0,p=0;
                for(auto card : handP2) if(utils.firstWinsTruco(card, handP1[idxCard])) c++;
                for(auto card : handP2) if(utils.valorTruco(card)==utils.valorTruco(handP1[idxCard])) p++;
                infoStateIdP2[idxAmountBetter]='0'+c;
                infoStateIdP2[idxAmountParda]='0'+p;
            }
            infoStateIdP1[idxFirstColThrow+idxCard]='U';
        }else{
            mesa[2*currentRound+1]=handP2[idxCard];
            handP2Used[idxCard]=true;
            isP1TrucoTurn=true;
            isP1Turn=true;
            if(mesa[2*currentRound]==""){
                int c=0,p=0;
                for(auto card : handP1) if(utils.firstWinsTruco(card, handP2[idxCard])) c++;
                for(auto card : handP1) if(utils.valorTruco(card)==utils.valorTruco(handP2[idxCard])) p++;
                infoStateIdP1[idxAmountBetter]='0'+c;
                infoStateIdP1[idxAmountParda]='0'+p;
            }
            infoStateIdP2[idxFirstColThrow+idxCard]='U';
        }
        if(mesa[2*currentRound]!="" && mesa[2*currentRound+1]!=""){
            infoStateIdP1[idxAmountBetter]='X';
            infoStateIdP1[idxAmountParda]='P';
            infoStateIdP2[idxAmountBetter]='X';
            infoStateIdP2[idxAmountParda]='P';
            if(utils.firstWinsTruco(mesa[2*currentRound], mesa[2*currentRound+1])){
                roundWinsP1++;
                infoStateIdP1[idxFirstWin+currentRound]='0';
                infoStateIdP2[idxFirstWin+currentRound]='0';
                isP1TrucoTurn=true;
                isP1Turn=true;
            }else if(utils.firstWinsTruco(mesa[2*currentRound+1], mesa[2*currentRound])){
                roundWinsP2++;
                infoStateIdP1[idxFirstWin+currentRound]='1';
                infoStateIdP2[idxFirstWin+currentRound]='1';
                isP1TrucoTurn=false;
                isP1Turn=false;
            }else{
                roundWinsP1++;
                roundWinsP2++;
                infoStateIdP1[idxFirstWin+currentRound]='E';
                infoStateIdP2[idxFirstWin+currentRound]='E';
                isP1TrucoTurn=true;
                isP1Turn=true;
            }

            if(roundWinsP1>2){//dos pardas
                terminal=true;
                gamePts1+=trucoValue;
            }else if(roundWinsP2>2){
                terminal=true;
                gamePts2+=trucoValue;
            }
            else if(roundWinsP1>1 && roundWinsP2<2){
                terminal=true;
                gamePts1+=trucoValue;
            }else if(roundWinsP2>1 && roundWinsP1<2){
                terminal=true;
                gamePts2+=trucoValue;
            }else if(currentRound==2){//ultima es parda
                terminal=true;
                if(roundWinsP1>=roundWinsP2) gamePts1+=trucoValue;
                else gamePts2+=trucoValue;
            }
            currentRound++;
        }
    }
    void bet(){//asume que se puede
        if(isP1Turn){
            if(hasToRespond) trucoValue++;
            hasToRespond=true;
            isP1Turn=false;
            canBetP1=false;
            canBetP2=trucoValue<3;
            infoStateIdP1[idxHasToRespond]='f';
            infoStateIdP2[idxHasToRespond]='t';
        }else{
            if(hasToRespond) trucoValue++;
            hasToRespond=true;
            isP1Turn=true;
            canBetP2=false;
            canBetP1=trucoValue<3;
            infoStateIdP2[idxHasToRespond]='f';
            infoStateIdP1[idxHasToRespond]='t';
        }
        infoStateIdP1[idxCanBet]=canBetP1?'t':'f';
        infoStateIdP2[idxCanBet]=canBetP2?'t':'f';
        infoStateIdP1[idxTrucoValue]='0'+trucoValue;
        infoStateIdP2[idxTrucoValue]='0'+trucoValue;
    }
    void quiero(){//asume que es valido
        isP1Turn=isP1TrucoTurn;
        hasToRespond=false;
        trucoValue++;
        infoStateIdP2[idxHasToRespond]='f';
        infoStateIdP1[idxHasToRespond]='f';
        infoStateIdP1[idxTrucoValue]='0'+trucoValue;
        infoStateIdP2[idxTrucoValue]='0'+trucoValue;
    }
    void reject(){//asume que es valido
        terminal=true;
        if(isP1Turn) gamePts2+=trucoValue;
        else gamePts1+=trucoValue;
    }
    private:
    int idxAmountBetter; 
    int idxAmountParda; 
    int idxFirstColThrow; 
    int idxFirstWin; 
    int idxTrucoValue;
    int idxCanBet;
    int idxHasToRespond;
    string getBucketsStr(vector<string> & v){
        vector<string> b;
        for (int i = 0; i < 3; i++)
        {
            b.push_back("["+to_string(valorTrucoGivenCard[v[i]])+"]");
        }
        sort(b.begin(),b.end());
        return b[0]+b[1]+b[2];
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
        
        InfoState & infoState = infoStates[((game.isP1Turn) ? game.infoStateIdP1 : game.infoStateIdP2)];
        
        vector<pair<string, Game>> actionsStorage;
        {//Actions
        if(game.hasToRespond){
            {//quiero + acciones
                if(game.isP1Turn){
                {// rebet
                    Game newGame = game;
                    newGame.quiero();
                    if(newGame.isP1Turn && newGame.canBetP1){
                        newGame.bet();
                        actionsStorage.push_back({"(b)", newGame});
                    }
                }
                {//q+tirar 1 carta
                    Game newGameAux = game;
                    newGameAux.quiero();
                    if(newGameAux.isP1Turn){//P1
                        if(!newGameAux.handP1Used[0]){//mayor
                            Game newGame = newGameAux;
                            newGame.throwCard(0);
                            if(newGame.terminal | !newGame.isP1Turn) actionsStorage.push_back({"(q)(T1)", newGame});
                        }
                        if(!newGameAux.handP1Used[1]){//media
                            Game newGame = newGameAux;
                            newGame.throwCard(1);
                            if(newGame.terminal | !newGame.isP1Turn) actionsStorage.push_back({"(q)(T2)", newGame});
                        }
                        if(!newGameAux.handP1Used[2]){//menor
                            Game newGame = newGameAux;
                            newGame.throwCard(2);
                            if(newGame.terminal | !newGame.isP1Turn) actionsStorage.push_back({"(q)(T3)", newGame});
                        }
                    }
                }
                {//q+tirar 2 cartas
                    Game newGameAux = game;
                    newGameAux.quiero();
                    if(newGameAux.isP1Turn){
                        for (int i = 0; i < 3; i++){
                            for (int j = 0; j < 3; j++){
                                if(j==i)continue;
                                if(!game.handP1Used[i] && !game.handP1Used[j]){
                                    Game newGame = newGameAux;
                                    newGame.throwCard(i);
                                    if(newGame.isP1Turn && !newGame.terminal){
                                        newGame.throwCard(j);
                                        actionsStorage.push_back({"(q)(T"+to_string(1+i)+")"+"(T"+to_string(1+j)+")",newGame});
                                    }
                                }
                            }
                        }
                    }
                }
                }
                else{//P2
                {// rebet
                    Game newGame = game;
                    newGame.quiero();
                    if(!newGame.isP1Turn && newGame.canBetP2){
                        newGame.bet();
                        actionsStorage.push_back({"(b)", newGame});
                    }
                }
                {//q+tirar 1
                    Game newGameAux = game;
                    newGameAux.quiero();
                    if(!newGameAux.isP1Turn){
                        if(!game.handP2Used[0]){//mayor
                            Game newGame = newGameAux;
                            newGame.throwCard(0);
                            if(newGame.terminal || newGame.isP1Turn) actionsStorage.push_back({"(T1)", newGame});
                        }
                        if(!game.handP2Used[1]){//media
                            Game newGame = newGameAux;
                            newGame.throwCard(1);
                            if(newGame.terminal || newGame.isP1Turn) actionsStorage.push_back({"(T2)", newGame});
                        }
                        if(!game.handP2Used[2]){//menor
                            Game newGame = newGameAux;
                            newGame.throwCard(2);
                            if(newGame.terminal || newGame.isP1Turn) actionsStorage.push_back({"(T3)", newGame});
                        }
                    }
                }
                {//q+tirar 2 cartas
                    Game newGameAux = game;
                    newGameAux.quiero();
                    if(!newGameAux.isP1Turn){
                        for (int i = 0; i < 3; i++)
                        {
                            for (int j = 0; j < 3; j++)
                            {
                                if(j==i)continue;
                                if(!game.handP2Used[i] && !game.handP2Used[j]){
                                    Game newGame = game;
                                    newGame.throwCard(i);
                                    if(!newGame.isP1Turn && !newGame.terminal){
                                        newGame.throwCard(j);
                                        actionsStorage.push_back({"(T"+to_string(1+i)+")"+"(T"+to_string(1+j)+")",newGame});
                                    }
                                }
                            }
                        }
                    }
                }
                }
            }
            {//quiero
                Game newGame = game;
                newGame.quiero();
                if(game.isP1Turn!=newGame.isP1Turn) actionsStorage.push_back({"(q)", newGame});
            }
            {//no quiero
                Game newGame = game;
                newGame.reject();
                actionsStorage.push_back({"(nq)", newGame});
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
            {//tirar 1 carta
                if(game.isP1Turn){//P1
                    if(!game.handP1Used[0]){//mayor
                        Game newGame = game;
                        newGame.throwCard(0);
                        if(newGame.terminal | !newGame.isP1Turn) actionsStorage.push_back({"(T1)", newGame});
                    }
                    if(!game.handP1Used[1]){//media
                        Game newGame = game;
                        newGame.throwCard(1);
                        if(newGame.terminal | !newGame.isP1Turn) actionsStorage.push_back({"(T2)", newGame});
                    }
                    if(!game.handP1Used[2]){//menor
                        Game newGame = game;
                        newGame.throwCard(2);
                        if(newGame.terminal | !newGame.isP1Turn) actionsStorage.push_back({"(T3)", newGame});
                    }
                }else{
                    if(!game.handP2Used[0]){//mayor
                        Game newGame = game;
                        newGame.throwCard(0);
                        if(newGame.terminal || newGame.isP1Turn) actionsStorage.push_back({"(T1)", newGame});
                    }
                    if(!game.handP2Used[1]){//media
                        Game newGame = game;
                        newGame.throwCard(1);
                        if(newGame.terminal || newGame.isP1Turn) actionsStorage.push_back({"(T2)", newGame});
                    }
                    if(!game.handP2Used[2]){//menor
                        Game newGame = game;
                        newGame.throwCard(2);
                        if(newGame.terminal || newGame.isP1Turn) actionsStorage.push_back({"(T3)", newGame});
                    }
                }
            }
            {//tirar 2
                if(game.isP1Turn){
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            if(j==i)continue;
                            if(!game.handP1Used[i] && !game.handP1Used[j]){
                                Game newGame = game;
                                newGame.throwCard(i);
                                if(newGame.isP1Turn && !newGame.terminal){
                                    newGame.throwCard(j);
                                    actionsStorage.push_back({"(T"+to_string(1+i)+")"+"(T"+to_string(1+j)+")",newGame});
                                }
                            }
                        }
                    }
                }else{//P2
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            if(j==i)continue;
                            if(!game.handP2Used[i] && !game.handP2Used[j]){
                                Game newGame = game;
                                newGame.throwCard(i);
                                if(!newGame.isP1Turn && !newGame.terminal){
                                    newGame.throwCard(j);
                                    actionsStorage.push_back({"(T"+to_string(1+i)+")"+"(T"+to_string(1+j)+")",newGame});
                                }
                            }
                        }
                    }
                }
            }
        }
        }

        if(!infoState.seen){
            infoState.seen=true;
            infoState.id=(game.isP1Turn?game.infoStateIdP1:game.infoStateIdP2);
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
};

int main(){
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    {
        valorTrucoGivenCard["01e"] = 1;  
        valorTrucoGivenCard["01b"] = 1;  
        valorTrucoGivenCard["07e"] = 1;  
        valorTrucoGivenCard["07o"] = 1;  

        valorTrucoGivenCard["03b"] = 2;  
        valorTrucoGivenCard["03c"] = 2;  
        valorTrucoGivenCard["03e"] = 2;  
        valorTrucoGivenCard["03o"] = 2;  

        valorTrucoGivenCard["02b"] = 2;  
        valorTrucoGivenCard["02e"] = 2;  
        valorTrucoGivenCard["02c"] = 2;  
        valorTrucoGivenCard["02o"] = 2;  

        valorTrucoGivenCard["01c"] = 3;  
        valorTrucoGivenCard["01o"] = 3;  

        valorTrucoGivenCard["12c"] = 3; 
        valorTrucoGivenCard["12e"] = 3; 
        valorTrucoGivenCard["12o"] = 3; 
        valorTrucoGivenCard["12b"] = 3; 

        valorTrucoGivenCard["11b"] = 3;  
        valorTrucoGivenCard["11o"] = 3;  
        valorTrucoGivenCard["11e"] = 3;  
        valorTrucoGivenCard["11c"] = 3;  

        valorTrucoGivenCard["10b"] = 4;  
        valorTrucoGivenCard["10o"] = 4;  
        valorTrucoGivenCard["10c"] = 4;  
        valorTrucoGivenCard["10e"] = 4;  

        valorTrucoGivenCard["07c"] = 4;  
        valorTrucoGivenCard["07b"] = 4;  

        valorTrucoGivenCard["06c"] = 5;  
        valorTrucoGivenCard["06e"] = 5;  
        valorTrucoGivenCard["06b"] = 5;  
        valorTrucoGivenCard["06o"] = 5;  

        valorTrucoGivenCard["05b"] = 5;  
        valorTrucoGivenCard["05o"] = 5;  
        valorTrucoGivenCard["05e"] = 5;  
        valorTrucoGivenCard["05c"] = 5;  

        valorTrucoGivenCard["04b"] = 5;  
        valorTrucoGivenCard["04c"] = 5;  
        valorTrucoGivenCard["04e"] = 5;  
        valorTrucoGivenCard["04o"] = 5;  
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
    
    for (int pts1 = 0; pts1 < 30; pts1++)
    {
        for (int pts2 = 0; pts2 < 30; pts2++)
        {
            if(pts1<15 || pts2<15) continue;
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
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time computation = " << std::chrono::duration_cast<std::chrono::minutes>(end - begin).count() << " minutes" << std::endl;
    system("pause");
}