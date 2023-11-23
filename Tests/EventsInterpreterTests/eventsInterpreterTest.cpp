#include <gtest/gtest.h>
#include "../../Source/EventsInterpreter/eventsInterpreter.hpp"

EventsInterpreter interpreter;
TEST(eventClasification, eventIsEnvidoAction){
    EXPECT_TRUE(interpreter.eventIsEnvidoAction("(envido)"));
    EXPECT_TRUE(interpreter.eventIsEnvidoAction("(real envido)"));
    EXPECT_TRUE(interpreter.eventIsEnvidoAction("(falta envido)"));

    EXPECT_FALSE(interpreter.eventIsEnvidoAction("envido"));
    EXPECT_FALSE(interpreter.eventIsEnvidoAction("real envido"));
    EXPECT_FALSE(interpreter.eventIsEnvidoAction("falta envido"));
    EXPECT_FALSE(interpreter.eventIsEnvidoAction("(ENVIDO)"));
    EXPECT_FALSE(interpreter.eventIsEnvidoAction("(REAL ENVIDO)"));
    EXPECT_FALSE(interpreter.eventIsEnvidoAction("(FALTA ENVIDO)"));

    EXPECT_FALSE(interpreter.eventIsEnvidoAction("(truco)"));
    EXPECT_FALSE(interpreter.eventIsEnvidoAction("(quiero)"));
}

TEST(eventClasification, eventIsTrucoAction){
    EXPECT_TRUE(interpreter.eventIsTrucoAction("(truco)"));
    EXPECT_TRUE(interpreter.eventIsTrucoAction("(retruco)"));
    EXPECT_TRUE(interpreter.eventIsTrucoAction("(vale cuatro)"));

    EXPECT_TRUE(interpreter.eventIsTrucoAction("(04b)"));
    EXPECT_TRUE(interpreter.eventIsTrucoAction("(12c)"));

    EXPECT_FALSE(interpreter.eventIsTrucoAction("(2c)"));
    EXPECT_FALSE(interpreter.eventIsTrucoAction("12c"));

    EXPECT_FALSE(interpreter.eventIsTrucoAction("truco"));
    EXPECT_FALSE(interpreter.eventIsTrucoAction("retruco"));
    EXPECT_FALSE(interpreter.eventIsTrucoAction("vale cuatro"));
    EXPECT_FALSE(interpreter.eventIsTrucoAction("(TRUCO)"));

    EXPECT_FALSE(interpreter.eventIsTrucoAction("(envido)"));
    EXPECT_FALSE(interpreter.eventIsTrucoAction("(quiero)"));
}

TEST(eventClasification, eventIsAcceptOrReject){
    EXPECT_TRUE(interpreter.eventIsAcceptOrReject("(quiero)"));
    EXPECT_TRUE(interpreter.eventIsAcceptOrReject("(no quiero)"));

    EXPECT_FALSE(interpreter.eventIsAcceptOrReject("quiero"));
    EXPECT_FALSE(interpreter.eventIsAcceptOrReject("no quiero"));
    EXPECT_FALSE(interpreter.eventIsAcceptOrReject("noquiero"));

    EXPECT_FALSE(interpreter.eventIsAcceptOrReject("(QUIERO)"));
    EXPECT_FALSE(interpreter.eventIsAcceptOrReject("(NO QUIERO)"));

    EXPECT_FALSE(interpreter.eventIsAcceptOrReject("(envido)"));
    EXPECT_FALSE(interpreter.eventIsAcceptOrReject("(truco)"));
}

TEST(eventClasification, eventIsEnvidoPointsInformation){
    EXPECT_TRUE(interpreter.eventIsEnvidoPointsInformation("(24-27)"));
    EXPECT_TRUE(interpreter.eventIsEnvidoPointsInformation("(24-27)"));
    EXPECT_TRUE(interpreter.eventIsEnvidoPointsInformation("(07-33)"));
    EXPECT_TRUE(interpreter.eventIsEnvidoPointsInformation("(31-no)"));

    EXPECT_FALSE(interpreter.eventIsEnvidoPointsInformation("07-33"));
    EXPECT_FALSE(interpreter.eventIsEnvidoPointsInformation("(07_33)"));
}


TEST(eventParsing, getEventsListGivenHistory){
    //initialState
    string historyEJ1 = "";
    vector<string> outEJ1 = interpreter.getEventsListGivenHistory(historyEJ1);
    EXPECT_EQ(0, outEJ1.size());

    //EJ2
    string historyEJ2 = "(envido)(real envido)(quiero)(27-no)(truco)(quiero)(03c)(retruco)(no quiero)";
    vector<string> expectedOutEJ2 = {"(envido)", "(real envido)", "(quiero)", "(27-no)", "(truco)", "(quiero)", "(03c)", "(retruco)", "(no quiero)"};

    vector<string> outEJ2 = interpreter.getEventsListGivenHistory(historyEJ2);
    ASSERT_EQ(expectedOutEJ2.size(), outEJ2.size());
    for (int i = 0; i < outEJ2.size(); i++) {
        EXPECT_EQ(expectedOutEJ2[i], outEJ2[i]);
    }
}