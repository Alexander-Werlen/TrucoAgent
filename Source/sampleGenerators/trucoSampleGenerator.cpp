#include "../auxiliares/cardUtils.hpp"
#include "../auxiliares/deck.hpp"
#include <vector>
#include <set>
#include <string>
#include <random>
#include <iterator>
#include <fstream>

using namespace std;

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

Deck deck;
cardUtils utils;
string getRandomStart(){
    vector<string> cards1;
    vector<string> cards2;
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

    string sample = "";
    for(string card : cards1) sample+=card+" ";
    for(string card : cards2) sample+=card+" ";
    return sample;
}

int main(){
    int it = 1e5+500; //limite de samples
    for (int pts1 = 0; pts1 < 30; pts1++)
    {
        for (int pts2 = 0; pts2 < 30; pts2++)
        {
            string path = "C:/Users/Usuario/Desktop/PC/Yo/QUE HACER/PROGRAMAR/Proyectos/TrucoSolver/TrucoWebAgent/cachedData/chanceSamples/";
            string fileName = path+"samplesTruco"+createIdOfEnvidoPointsBothPlayer(pts1,pts2)+".txt";
            ofstream File(fileName);
            for (int i = 0; i < it; i++)
            {
                string sample = getRandomStart();
                File<<sample<<endl;
            }
            File.close();
        }
    }
}