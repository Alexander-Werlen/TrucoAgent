/* 
    Given your known hand and the cards you know from the opponent hand plus their envido points, lets you know the minimum possible truco value that they have in their hole card.
*/

#pragma once

#include "../Auxiliares/cardUtils.hpp"
#include "../Auxiliares/deck.hpp"

#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <stdexcept>

#include <iostream> //debugging

using namespace std;

class MinimumHoleCardPredictor{
    public:
    MinimumHoleCardPredictor();
    int getMinimumPosibleHoleCardTrucoValue(vector<string>, vector<string>, int = -1);

    private:
    Deck deck;
    cardUtils cardUtils;
    map<string,int> minimumHoleCardGivenData;
};