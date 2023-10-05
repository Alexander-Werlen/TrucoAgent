/* 
    Provides functionalities for handling NIC events.
*/

#pragma once
#include <string>
#include <set>
#include <vector>
#include <deque>
#include <stdexcept>

using namespace std;

class EventsInterpreter{
    public:
    EventsInterpreter();
    bool eventIsEnvidoAction(const string &);
    bool eventIsTrucoAction(const string &);
    bool eventIsAcceptOrReject(const string &);
    bool eventIsEnvidoPointsInformation(const string &);
    vector<string> getEventsListGivenHistory(const string &);
    private:
    set<string> envidoActionsSet;
    set<string> trucoActionsSet;
    string acceptActionString;
    string rejectActionString;
};