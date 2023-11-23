#include <gtest/gtest.h>
#include "../../source/minimumHoleCardPredictor/minimumHoleCardPredictor.hpp"

MinimumHoleCardPredictor predictor;

TEST(minimumHoleCardPredictorTest, getMinimumPosibleHoleCardTrucoValue){
    vector<string> playerHand = {"07e", "11c", "04e"};
    vector<string> opponentKnownCards = {};

    int minPossible = predictor.getMinimumPosibleHoleCardTrucoValue(playerHand, opponentKnownCards);
    EXPECT_EQ(14, minPossible);
}