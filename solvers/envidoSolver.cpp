#include <map>
#include <vector>
#include <set>
#include <random>
#include <fstream>
#include <iostream>
using namespace std;

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

class Node{
    public:
    string id;
    bool isP1;
    bool isTerminal;
    double nodeValue;
    string infoStateId;
    int cantMoves;
    map<string, string> sigNodeOfAction;
    Node(){}
};

class InfoState{
    public:
    //basic
    string id;
    int cantActions;
    vector<string> actions;
    //cfr
    double reachPr;
    vector<double> strategy;
    vector<double> regretSum;
    vector<double> strategySum;

    InfoState(){}
    void addAction(string act){
        actions.push_back(act);
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
int gamePointsP1;
int gamePointsP2;
map<string, Node> gameNodes;
map<string, InfoState> infoStates;
map<string,double> trucoStateValues;
map<string,double> envidoStateValues;
class Envido{
    public:
    double expectedGameValue;
    Envido(){}

    void train(int Niterations, int pts1, int pts2){
        gamePointsP1=pts1;
        gamePointsP2=pts2;
        string path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/chanceSamples/";
        string fileName = path+"samplesEnvido"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)+".txt";
        ifstream FileSamples(fileName);
        expectedGameValue=0;
        for (int i = 1; i <= Niterations; i++)
        {
            string chanceSample;
            FileSamples>>chanceSample;
            string startingPos="0|"+ptsToString(pts1)+"-"+ptsToString(pts2)+"|"+chanceSample+"|";
            expectedGameValue+=cfr(startingPos, 1, 1);
            for(auto & e : infoStates)e.second.updateStrategy();
            for(auto & e : infoStates)e.second.discount(i);
        }
        expectedGameValue/=Niterations;
        FileSamples.close();
    }

    double cfr(string nodeId, double pr1, double pr2){
        if(gameNodes.find(nodeId)==gameNodes.end()) cout<<"ERROR";
        Node & node = gameNodes[nodeId];

        if(node.isTerminal){
            if(node.nodeValue>=0){
                if(gamePointsP1+node.nodeValue>=30) return 50;
                if(trucoStateValues.find("t"+createIdOfEnvidoPointsBothPlayer(gamePointsP1+node.nodeValue,gamePointsP2))==trucoStateValues.end()){
                    while(true) cout<<"Truco init state not found yet"<<endl;
                }else{
                    return trucoStateValues["t"+createIdOfEnvidoPointsBothPlayer(gamePointsP1+node.nodeValue,gamePointsP2)];
                }
            }else{
                if(gamePointsP2-node.nodeValue>=30) return -50;
                if(trucoStateValues.find("t"+createIdOfEnvidoPointsBothPlayer(gamePointsP1,gamePointsP2-node.nodeValue))==trucoStateValues.end()){
                    while(true) cout<<"Truco init state not found yet"<<endl;
                }else{
                    return trucoStateValues["t"+createIdOfEnvidoPointsBothPlayer(gamePointsP1,gamePointsP2-node.nodeValue)];
                }
            }
        }

        InfoState & infoState = infoStates[node.infoStateId];
        vector<double> actionUtils (infoState.cantActions,0);
        string nextNodeId;

        for (int act = 0; act < infoState.cantActions; act++)
        {   
            nextNodeId = node.sigNodeOfAction[infoState.actions[act]];
            if(node.isP1){
                actionUtils[act] = cfr(nextNodeId, pr1*infoState.strategy[act], pr2);
            } else{
                actionUtils[act] = -1*cfr(nextNodeId, pr1, pr2*infoState.strategy[act]);
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
        if(node.isP1){
            infoState.reachPr+=pr1;
            for (int i = 0; i < infoState.cantActions; i++)
            {
                infoState.regretSum[i]+=pr2*regrets[i];
            }
        } else{
            infoState.reachPr+=pr2;
            for (int i = 0; i < infoState.cantActions; i++)
            {
                infoState.regretSum[i]+=pr1*regrets[i];
            }
        }
        
        return node.isP1 ? util : -util;
    }
};

int main(){
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
            if(envidoStateValues.find("e"+createIdOfEnvidoPointsBothPlayer(pts1,pts2))!=envidoStateValues.end()) continue;
            bool can=true;
            for (int i = 0; i < 30; i++)
            {
                if(pts1+i<30 && trucoStateValues.find("t"+createIdOfEnvidoPointsBothPlayer(pts1+i,pts2))==trucoStateValues.end()) can=false;
                if(pts2+i<30 && trucoStateValues.find("t"+createIdOfEnvidoPointsBothPlayer(pts1,pts2+i))==trucoStateValues.end()) can=false;
            }
            if(!can) continue;

            cout<<"Current: "<<pts1<<"-"<<pts2<<endl;
            //clear previous data
            gameNodes.clear();
            infoStates.clear();

            //read game nodes
            string path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/envidoAbstractionStates/gameNodesData/";
            string fileNameGameNodes = path+"gameNodesEnvido"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)+".txt";
            ifstream File(fileNameGameNodes);
            int n,k;
            string s, s2;

            File>>n;
            for (int i = 0; i < n; i++)
            {
                File>>s;
                gameNodes[s] = Node();
                Node & node = gameNodes[s];
                node.id=s;

                File>>node.isTerminal;
                if(node.isTerminal){
                    File>>node.nodeValue; //check not using id when terminal
                }else{
                    File>>node.infoStateId;
                    File>>node.isP1;
                    File>>node.cantMoves;
                    for (int j = 0; j < node.cantMoves; j++)
                    {
                        File>>s>>s2;
                        node.sigNodeOfAction[s]=s2;
                    }
                }
            }
            File.close();

            //read info states
            path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/envidoAbstractionStates/infoStatesData/";
            string fileNameInfoStates = path+"infoStatesEnvido"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)+".txt";
            ifstream FileInfoStates(fileNameInfoStates);

            FileInfoStates>>k;
            for (int i = 0; i < k; i++)
            {
                FileInfoStates>>s;
                infoStates[s] = InfoState();
                InfoState & state = infoStates[s];
                state.id=s;
                FileInfoStates>>state.cantActions;
                for (int j = 0; j < state.cantActions; j++)
                {
                    FileInfoStates>>s;
                    state.addAction(s);
                }
                state.initialize();
            }
            FileInfoStates.close();

            //start cfr
            Envido agent;
            agent.train(1e4, pts1, pts2);

            //store state value
            envidoStateValues["e"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)]=agent.expectedGameValue;

            //output results
            path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/solvedStrategies/envidoStrategies/";
            string fileNameStrategy = path+"strategyEnvido"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)+".txt";
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

}