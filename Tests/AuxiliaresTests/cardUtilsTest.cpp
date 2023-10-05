#include <gtest/gtest.h>
#include "../../Source/Auxiliares/cardUtils.hpp"

cardUtils utils;

TEST(Accessing, palo){
    EXPECT_EQ('b', utils.palo("01b"));
    EXPECT_EQ('c', utils.palo("01c"));
    EXPECT_EQ('e', utils.palo("01e"));
    EXPECT_EQ('o', utils.palo("01o"));

    EXPECT_EQ('b', utils.palo("11b"));
    EXPECT_EQ('c', utils.palo("11c"));
    EXPECT_EQ('e', utils.palo("11e"));
    EXPECT_EQ('o', utils.palo("11o"));
}

TEST(Accessing, valorEnvido){
    EXPECT_EQ(1, utils.valorEnvido("01b"));
    EXPECT_EQ(1, utils.valorEnvido("01e"));
    EXPECT_EQ(1, utils.valorEnvido("01c"));
    EXPECT_EQ(1, utils.valorEnvido("01o"));

    EXPECT_EQ(1, utils.valorEnvido("01c"));
    EXPECT_EQ(2, utils.valorEnvido("02c"));
    EXPECT_EQ(4, utils.valorEnvido("04c"));
    EXPECT_EQ(5, utils.valorEnvido("05c"));
    EXPECT_EQ(7, utils.valorEnvido("07c"));

    EXPECT_EQ(0, utils.valorEnvido("10b"));
    EXPECT_EQ(0, utils.valorEnvido("11c"));
    EXPECT_EQ(0, utils.valorEnvido("12e"));
}

TEST(Computing, cardIsValid){
    EXPECT_TRUE(utils.cardIsValid("01b"));
    EXPECT_TRUE(utils.cardIsValid("01c"));
    EXPECT_TRUE(utils.cardIsValid("11e"));

    EXPECT_FALSE(utils.cardIsValid("5b"));
    EXPECT_FALSE(utils.cardIsValid("1c"));
    EXPECT_FALSE(utils.cardIsValid("11"));
}

TEST(Computing, envidoPointsGivenTwoCards){
    EXPECT_EQ(2, utils.envidoPointsGivenTwoCards("01e","02c"));
    EXPECT_EQ(7, utils.envidoPointsGivenTwoCards("07c","03b"));
    EXPECT_EQ(3, utils.envidoPointsGivenTwoCards("11c","03b"));
    EXPECT_EQ(0, utils.envidoPointsGivenTwoCards("11c","12b"));

    EXPECT_EQ(26, utils.envidoPointsGivenTwoCards("04c","02c"));
    EXPECT_EQ(29, utils.envidoPointsGivenTwoCards("06e","03e"));
    EXPECT_EQ(33, utils.envidoPointsGivenTwoCards("06b","07b"));

    EXPECT_EQ(23, utils.envidoPointsGivenTwoCards("03e","10e"));
    EXPECT_EQ(26, utils.envidoPointsGivenTwoCards("10c","06c"));
    EXPECT_EQ(21, utils.envidoPointsGivenTwoCards("12b","01b"));

    EXPECT_EQ(20, utils.envidoPointsGivenTwoCards("10c","11c"));
    EXPECT_EQ(20, utils.envidoPointsGivenTwoCards("11e","12e"));
    EXPECT_EQ(20, utils.envidoPointsGivenTwoCards("10b","12b"));
}

TEST(Computing, envidoPointsOfHand){
    EXPECT_EQ(0, utils.envidoPointsOfHand({"12c","11e","10b"}));
    EXPECT_EQ(3, utils.envidoPointsOfHand({"01c","01e","03b"}));
    EXPECT_EQ(7, utils.envidoPointsOfHand({"02c","07e","05b"}));
    EXPECT_EQ(20, utils.envidoPointsOfHand({"12c","11e","10c"}));
    EXPECT_EQ(22, utils.envidoPointsOfHand({"12c","05e","02c"}));
    EXPECT_EQ(27, utils.envidoPointsOfHand({"12c","05b","07c"}));
    EXPECT_EQ(29, utils.envidoPointsOfHand({"12c","02c","07c"}));
}
