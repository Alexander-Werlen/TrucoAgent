#include "./envidoConverter_NIC_to_NAEI.hpp"

EnvidoStateConverter::EnvidoStateConverter(){
    //NIC_elements_indices init
    NIC_elements_indices.whoPlaysId=0;
    NIC_elements_indices.initialPointsStart=2;
    NIC_elements_indices.P1CardsStart=8;
    NIC_elements_indices.P2CardsStart=8+5*3;
    NIC_elements_indices.historyStart=8+5*6+1;
    
    //NIC_elements_lengths init
    NIC_elements_lengths.initialPointsLength=5;
    NIC_elements_lengths.P1CardsLength=5*3;
    NIC_elements_lengths.P2CardsLength=5*3;

    //Str identifier for truco action
    trucoActionStrIdentifier="(truco action)";

    //Init eventInterpreter
    EventsInterpreter eventsInterpreter;
    
};

bool EnvidoStateConverter::getWhoPlays(const string & nicState){
    return nicState[NIC_elements_indices.whoPlaysId] == '0';
};
string EnvidoStateConverter::getWhoPlaysId(const string & nicState){
    string whoPlaysId="";
    whoPlaysId+=nicState[NIC_elements_indices.whoPlaysId];
    return whoPlaysId;
};
string EnvidoStateConverter::getHandP1(const string & nicState){
    return nicState.substr(NIC_elements_indices.P1CardsStart, NIC_elements_lengths.P1CardsLength);
};
string EnvidoStateConverter::getHandP2(const string & nicState){
    return nicState.substr(NIC_elements_indices.P2CardsStart, NIC_elements_lengths.P2CardsLength);
};
string EnvidoStateConverter::getHistory(const string & nicState){
    return nicState.substr(NIC_elements_indices.historyStart);
};
string EnvidoStateConverter::getInitialPoints(const string & nicState){
    return nicState.substr(NIC_elements_indices.initialPointsStart,NIC_elements_lengths.initialPointsLength);
};
string EnvidoStateConverter::convert_history_from_NIC_to_NAEI(const string & historyNIC, bool & isTerminalFlagOut){
    vector<string> eventList = eventsInterpreter.getEventsListGivenHistory(historyNIC);
    isTerminalFlagOut=false;
    int trucoActionsCounter=0;
    string historyNAEI = "";
    for(string event : eventList){
        if(eventsInterpreter.eventIsEnvidoAction(event)) historyNAEI+=event;
        else if(eventsInterpreter.eventIsTrucoAction(event)) {historyNAEI+=trucoActionStrIdentifier;trucoActionsCounter++;}
        else if(eventsInterpreter.eventIsAcceptOrReject(event)) {historyNAEI+=event; isTerminalFlagOut=true;}
        else break;
        
        if(trucoActionsCounter==2){
            isTerminalFlagOut=true;
            break;
        }
    }
    return historyNAEI;
};


string EnvidoStateConverter::convert_NIC_to_NAEI_envido(const string & nicState){
    //get history and checking if is terminal node of envido
    string historyNIC = getHistory(nicState);
    bool isTerminalFlagOut;
    string historyNAEI = convert_history_from_NIC_to_NAEI(historyNIC, isTerminalFlagOut);

    bool isP1Turn = getWhoPlays(nicState);

    string whoPlaysId = getWhoPlaysId(nicState);
    if(isTerminalFlagOut) whoPlaysId = "2";

    string naeiInitialPoints = getInitialPoints(nicState);

    string naeiHand;
    if(isP1Turn | isTerminalFlagOut) naeiHand = getHandP1(nicState);
    else naeiHand = getHandP2(nicState);

    return (whoPlaysId+" "+naeiInitialPoints+" "+naeiHand+" "+historyNAEI);
}
