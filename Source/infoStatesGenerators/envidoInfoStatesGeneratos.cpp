/* 
    script that generates the NAEI envido info states and stores them in cache
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <stdexcept>
#include <cmath>

using namespace std;

class Node{
    public:
    string id;
    string infoStateId;
    bool isP1;
    int cantMoves;
    bool isTerminal;
    float valueTerminal;
    map<string, string> sigNodeGivenAction;

    Node(){}
    Node(string idIn, string infoStateIdIn, bool isP1In){
        isTerminal=false;
        cantMoves=0;
        id=idIn;
        infoStateId=infoStateIdIn;
        isP1=isP1In;
    }
    Node(string idIn, float value){
        isTerminal=true;
        valueTerminal=value;
    }

    void addAction(string act, string sigNode){
        sigNodeGivenAction[act] = sigNode;
        cantMoves++;
    }
};

class InfoState{
    public:
    string id;
    bool isP1;
    int cantActions;
    set<string> actions;

    InfoState(){}
    InfoState(string idIn, bool isP1In){
        isP1=isP1In;
        id=idIn;
        cantActions=0;
    }
    void addAction(string act){
        if(actions.insert(act).second) cantActions++;
    }
    
};

string ptsToString(int envidoPoints){
    //converts int to envidoPoints string format
    string envidoPointsStr="";
    envidoPointsStr+=('0'+envidoPoints/10);
    envidoPointsStr+=('0'+envidoPoints%10);

    return envidoPointsStr;
}

string createInitStructure(int ptsP1, int ptsP2){
    string initStructure="0 ";
    initStructure+=ptsToString(ptsP1);
    initStructure+="-";
    initStructure+=ptsToString(ptsP2);

    return initStructure;
}

string createIdOfEnvidoPointsSinglePlayer(int pts){
    return "["+ptsToString(pts)+"]";
}

string createIdOfEnvidoPointsBothPlayer(int ptsA, int ptsB){
    return "["+ptsToString(ptsA)+"-"+ptsToString(ptsB)+"]";
}

string getBetAction(int betSize){
    return "(b"+to_string(betSize)+")";
}

//auxiliar variables
map<int, vector<int>> posibleBets = { //no falta envido
    {0, {2,3}},
    {2, {4,5}},
    {4, {7}}
};

//global variables initialization
map<string, Node> gameNodes;
map<string, InfoState> infoSet;

class GameState{
    public:
    string initStructure;
    string history;
    string pointsP1Id;
    string pointsP2Id;
    string pointsId;
    string infoStatePrefixP1;
    string infoStatePrefixP2;
    string gameStatePrefix;
    int gamePointsP1;
    int gamePointsP2;
    int pointsP1;
    int pointsP2;
    int valueFaltaEnvido;
    int valueEnvido;
    int valueEnvidoBet;
    bool isP1Turn;
    bool isTerminal;
    bool hasToRespond;
    bool inFaltaEnvido;

    GameState(int ptsP1, int ptsP2, int envidoPointsP1, int envidoPointsP2){
        initStructure = createInitStructure(ptsP1, ptsP2);
        history = "";
        gamePointsP1=ptsP1;
        gamePointsP2=ptsP2;
        pointsP1=envidoPointsP1;
        pointsP2=envidoPointsP2;
        initIds();
        initPrefixes();
        valueEnvido=0;
        valueEnvidoBet=0;
        valueFaltaEnvido=30-max(gamePointsP1, gamePointsP2);
        isP1Turn=true;
        isTerminal=false;
        hasToRespond=false;
        inFaltaEnvido=false;
    }
    string getInfoStateIdP1() const{
        return infoStatePrefixP1+history;
    }
    string getInfoStateIdP2() const{
        return infoStatePrefixP2+history;
    }
    string getInfoStateId() const{
        return (isP1Turn ? infoStatePrefixP1 : infoStatePrefixP2) + history;
    }
    string getGameStateId() const{
        return gameStatePrefix+history;
    }
    float getEvaluationQuiero() const{
        float P1 = gamePointsP1;
        float P2 = gamePointsP2;

        if(pointsP1>=pointsP2) P1+=valueEnvido;
        else P2+=valueEnvido;

        if(P1>=30) return 50;
        if(P2>=30) return -50;

        float value = 50*atan((P1-P2)/7);

        value=max(-48.0f, value);
        value=min(48.0f, value);

        return value;
    }
    float getEvaluationReject() const{
        float P1 = gamePointsP1;
        float P2 = gamePointsP2;

        if(!isP1Turn) P1+=valueEnvido;
        else P2+=valueEnvido;

        if(P1>=30) return 50;
        if(P2>=30) return -50;

        float value = 50*atan((P1-P2)/7);

        value=max(-48.0f, value);
        value=min(48.0f, value);

        return value;
    }

    private:
    void initIds(){
        pointsP1Id = createIdOfEnvidoPointsSinglePlayer(pointsP1);
        pointsP2Id = createIdOfEnvidoPointsSinglePlayer(pointsP2);
        pointsId = createIdOfEnvidoPointsBothPlayer(pointsP1, pointsP2);
    }
    void initPrefixes(){
        infoStatePrefixP1 = initStructure + " " + pointsP1Id + " ";
        infoStatePrefixP2 = initStructure + " " + pointsP2Id + " ";
        gameStatePrefix = initStructure + " " + pointsId + " ";
    }

};

void recursiveSimulation(const GameState gameState){
    //get gameNode
    Node & node = gameNodes[gameState.getGameStateId()];
    InfoState & infoNode = infoSet[gameState.getInfoStateId()];

    if(node.isTerminal) throw runtime_error("In recursiveSimulation used gameState that was in terminal node"); //This shouldn´t happen

    if(gameState.hasToRespond){
        //only yes or no, rebetting on other side
        {//quiero
            GameState newGameState = gameState;
            newGameState.valueEnvido=newGameState.valueEnvidoBet;
            newGameState.history+="(q)";
            node.addAction("(q)", newGameState.getGameStateId());
            infoNode.addAction("(q)");

            gameNodes[newGameState.getGameStateId()] = Node(newGameState.getGameStateId(), newGameState.getEvaluationQuiero());
        }
        {//no quiero
            GameState newGameState = gameState;
            newGameState.history+="(nq)";
            node.addAction("(nq)", newGameState.getGameStateId());
            infoNode.addAction("(nq)");

            gameNodes[newGameState.getGameStateId()] = Node(newGameState.getGameStateId(), newGameState.getEvaluationReject());
        }
    }else{//no need to respond
        {//pass
            GameState newGameState = gameState;
            newGameState.isP1Turn=!gameState.isP1Turn;
            newGameState.history+="(p)";
            node.addAction("(p)", newGameState.getGameStateId());
            infoNode.addAction("(p)");

            if(gameState.isP1Turn){ //not terminal
                gameNodes[newGameState.getGameStateId()] = Node(newGameState.getGameStateId(), newGameState.getInfoStateId(), false);
                infoSet[newGameState.getInfoStateId()] = InfoState(newGameState.getInfoStateId(), false);
                newGameState.isP1Turn=false;

                recursiveSimulation(newGameState);
            }else{ //terminal
                gameNodes[newGameState.getGameStateId()] = Node(newGameState.getGameStateId(), 0);
            }
        }
    }
    {//bets
        if(!gameState.inFaltaEnvido){
        for(auto bet : posibleBets[gameState.valueEnvidoBet]){
            GameState newGameState = gameState;
            newGameState.valueEnvido=gameState.valueEnvidoBet;
            newGameState.valueEnvidoBet=bet;
            if(gameState.valueEnvidoBet==0) newGameState.valueEnvido=1;
            newGameState.isP1Turn=!gameState.isP1Turn;
            newGameState.hasToRespond=true;
            newGameState.history+=getBetAction(bet);
            node.addAction(getBetAction(bet), newGameState.getGameStateId());
            infoNode.addAction(getBetAction(bet));

            gameNodes[newGameState.getGameStateId()] = Node(newGameState.getGameStateId(), newGameState.getInfoStateId(), newGameState.isP1Turn);
            infoSet[newGameState.getInfoStateId()] = InfoState(newGameState.getInfoStateId(), newGameState.isP1Turn);

            recursiveSimulation(newGameState);
        }
        }
    }
    {//falta envido
        if(!gameState.inFaltaEnvido){
            GameState newGameState = gameState;
            newGameState.history+="(bF)";
            newGameState.valueEnvido=gameState.valueEnvidoBet;
            newGameState.valueEnvidoBet=gameState.valueFaltaEnvido;
            if(gameState.valueEnvidoBet==0) newGameState.valueEnvido=1;
            newGameState.isP1Turn=!gameState.isP1Turn;
            newGameState.inFaltaEnvido=true;
            newGameState.hasToRespond=true;
            node.addAction("(bF)", newGameState.getGameStateId());
            infoNode.addAction("(bF)");

            gameNodes[newGameState.getGameStateId()] = Node(newGameState.getGameStateId(), newGameState.getInfoStateId(), newGameState.isP1Turn);
            infoSet[newGameState.getInfoStateId()] = InfoState(newGameState.getInfoStateId(), newGameState.isP1Turn);

            recursiveSimulation(newGameState);
        }
    }
}

void simulateEnvido(int ptsP1, int ptsP2, int envidoPointsPA, int envidoPointsPB){

    GameState gameState(ptsP1, ptsP2, envidoPointsPA, envidoPointsPB);

    gameNodes[gameState.getGameStateId()] = Node(gameState.getGameStateId(), gameState.getInfoStateIdP1(), true);
    infoSet[gameState.getInfoStateIdP1()] = InfoState(gameState.getInfoStateIdP1(), true);

    recursiveSimulation(gameState);
}

int main(){

    vector<int> possibleEnvidoPoints = {0,1,2,3,4,5,6,7,20,21,22,23,24,25,26,27,28,29,30,31,32,33};

    //processing all initial envido positions
    for (int ptsP1 = 0; ptsP1 < 30; ptsP1++)
    {
        for (int ptsP2 = 0; ptsP2 < 30; ptsP2++)
        {
            //clearing past data of global variables
            gameNodes.clear();
            infoSet.clear();

            //use all possibleEnvidoPoints;
            for(int envidoPointsPA : possibleEnvidoPoints){
                for(int envidoPointsPB : possibleEnvidoPoints){
                    //simular para conseguir game/info states
                    simulateEnvido(ptsP1, ptsP2, envidoPointsPA, envidoPointsPB);
                }
            }
            //storing game nodes data
            /* game node format 
                <stateId> <isTerminal>
                (if terminal: ) <value>
                (else: ) <infoStateId> <isP1> <cantMoves>
                    (for i : cantMoves) <action> <sigState>
            */
            string path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/envidoAbstractionStates/gameNodesData/";
            string fileNameGameNodes = path+"gameNodesEnvido"+createIdOfEnvidoPointsBothPlayer(ptsP1,ptsP2)+".txt";
            ofstream outFile(fileNameGameNodes);

            outFile<<gameNodes.size()<<endl;
            for(auto node : gameNodes){
                outFile<<node.first<<" "<<node.second.isTerminal<<endl;
                if(node.second.isTerminal){
                    outFile<<node.second.valueTerminal<<endl;
                }else{
                    outFile<<node.second.infoStateId<<" "<<node.second.isP1<<" "<<node.second.cantMoves<<endl;
                    for(auto actions : node.second.sigNodeGivenAction){
                        outFile<<actions.first<<" "<<actions.second<<endl;
                    }
                }
                outFile<<endl;
            }
            outFile.close();

            //storing info states data
            /* game node format 
                <infoStateId> <cantMoves> (for i : cantMoves) <action>
            */
            path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/envidoAbstractionStates/infoStatesData/";
            string fileNameInfoStates = path+"infoStatesEnvido"+createIdOfEnvidoPointsBothPlayer(ptsP1,ptsP2)+".txt";
            ofstream outFileInfoStates(fileNameInfoStates);

            outFileInfoStates<<infoSet.size()<<endl;
            for(auto state : infoSet){
                outFileInfoStates<<state.second.id<<" "<<state.second.cantActions<<" ";
                for(auto action : state.second.actions) outFileInfoStates<<action<<" ";
                outFileInfoStates<<endl;
            }
            outFileInfoStates.close();

        }
    }
    

}
