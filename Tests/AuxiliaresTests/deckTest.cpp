#include <gtest/gtest.h>
#include "../../source/auxiliares/deck.hpp"

Deck deck;
TEST(DeckFunctionalities, cardIsInDeck){
    EXPECT_TRUE(deck.cardIsInDeck("01e"));
    EXPECT_FALSE(deck.cardIsInDeck("1e"));

    EXPECT_TRUE(deck.cardIsInDeck("05e"));
    EXPECT_FALSE(deck.cardIsInDeck("5e"));

    EXPECT_TRUE(deck.cardIsInDeck("12e"));
    EXPECT_FALSE(deck.cardIsInDeck("012e"));

    EXPECT_FALSE(deck.cardIsInDeck("00e"));
    EXPECT_FALSE(deck.cardIsInDeck("08e"));
    EXPECT_FALSE(deck.cardIsInDeck("09e"));
    EXPECT_FALSE(deck.cardIsInDeck("13e"));

    EXPECT_TRUE(deck.cardIsInDeck("05b"));
    EXPECT_TRUE(deck.cardIsInDeck("05c"));
    EXPECT_TRUE(deck.cardIsInDeck("05e"));
    EXPECT_TRUE(deck.cardIsInDeck("05o"));
    EXPECT_TRUE(deck.cardIsInDeck("10b"));
    EXPECT_TRUE(deck.cardIsInDeck("10c"));
    EXPECT_TRUE(deck.cardIsInDeck("10e"));
    EXPECT_TRUE(deck.cardIsInDeck("10o"));
    EXPECT_TRUE(deck.cardIsInDeck("12b"));
    EXPECT_TRUE(deck.cardIsInDeck("12c"));
    EXPECT_TRUE(deck.cardIsInDeck("12e"));
    EXPECT_TRUE(deck.cardIsInDeck("12o"));

    EXPECT_FALSE(deck.cardIsInDeck("b"));
    EXPECT_FALSE(deck.cardIsInDeck("c"));
    EXPECT_FALSE(deck.cardIsInDeck("e"));
    EXPECT_FALSE(deck.cardIsInDeck("o"));

    EXPECT_FALSE(deck.cardIsInDeck("01f"));
    EXPECT_FALSE(deck.cardIsInDeck("01d"));
    EXPECT_FALSE(deck.cardIsInDeck("01a"));
    EXPECT_FALSE(deck.cardIsInDeck("01m"));
}