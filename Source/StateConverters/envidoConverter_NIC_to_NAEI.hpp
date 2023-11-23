/* 
    Converts envido NIC to NAEI
*/

#pragma once
#include "..\eventsInterpreter\eventsInterpreter.hpp"
#include <string>

class EnvidoStateConverter{
    public:
    EnvidoStateConverter();
    string convert_NIC_to_NAEI_envido(const string &);

    private:
    EventsInterpreter eventsInterpreter;

    struct NIC_elements_indices_struct{
        int whoPlaysId;
        int initialPointsStart;
        int P1CardsStart;
        int P2CardsStart;
        int historyStart;
    };
    NIC_elements_indices_struct NIC_elements_indices;

    struct NIC_elements_lengths_struct{
        int initialPointsLength;
        int P1CardsLength;
        int P2CardsLength;
    };
    NIC_elements_lengths_struct NIC_elements_lengths;

    string trucoActionStrIdentifier;

    bool getWhoPlays(const string &);
    string getWhoPlaysId(const string &);
    string getHandP1(const string &);
    string getHandP2(const string &);
    string getInitialPoints(const string &);
    string getHistory(const string &);
    string convert_history_from_NIC_to_NAEI(const string &, bool &);

};
