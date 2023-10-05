#include "eventsInterpreter.hpp"

EventsInterpreter::EventsInterpreter(){
    envidoActionsSet = {"(envido)", "(real envido)", "(falta envido)"};
    trucoActionsSet = {
        "(truco)", "(retruco)", "(vale cuatro)", 
        "(01b)", "(02b)", "(03b)", "(04b)", "(05b)", "(06b)", "(07b)", "(10b)", "(11b)", "(12b)",
        "(01c)", "(02c)", "(03c)", "(04c)", "(05c)", "(06c)", "(07c)", "(10c)", "(11c)", "(12c)",
        "(01e)", "(02e)", "(03e)", "(04e)", "(05e)", "(06e)", "(07e)", "(10e)", "(11e)", "(12e)",
        "(01o)", "(02o)", "(03o)", "(04o)", "(05o)", "(06o)", "(07o)", "(10o)", "(11o)", "(12o)"
    };
    acceptActionString = "(quiero)";
    rejectActionString = "(no quiero)";
}

bool EventsInterpreter::eventIsEnvidoAction(const string & event){
    return envidoActionsSet.count(event);
};
bool EventsInterpreter::eventIsTrucoAction(const string & event){
    return trucoActionsSet.count(event);
};
bool EventsInterpreter::eventIsAcceptOrReject(const string & event){
    return (event==acceptActionString | event==rejectActionString);
};
bool EventsInterpreter::eventIsEnvidoPointsInformation(const string & event){
    //only checks structure, doesn´t check that int values are valid.
    if(event.length()!=7) return false;
    if(event.front()!='(' | event.back()!=')' | event[3]!='-') return false;
    return  true;
};

vector<string> EventsInterpreter::getEventsListGivenHistory(const string & history){
    vector<string> eventList;
    deque<char> historyChars = {history.begin(), history.end()}; //puts history in deque in chars
    string event;
    while(historyChars.size()>0){
        event="";
        if(historyChars.front()!='(') throw invalid_argument("history doesn't comply with NIC");
        event+=historyChars.front();
        historyChars.pop_front();
        while(historyChars.size()>0 & historyChars.front()!=')'){
            event+=historyChars.front();
            historyChars.pop_front();
        }
        if(historyChars.size()==0 | historyChars.front()!=')') throw invalid_argument("history doesn't comply with NIC");
        event+=historyChars.front();
        historyChars.pop_front();

        eventList.push_back(event);
    }
    return eventList;
};