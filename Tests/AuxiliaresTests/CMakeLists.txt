cmake_minimum_required(VERSION 3.27)

project(AuxiliaresTests C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


#deckTest
add_executable(deckTest deckTest.cpp)
target_link_libraries(deckTest PUBLIC
    gtest_main
    deck
)

add_test(
    NAME deckTest
    COMMAND deckTest
)

#cardUtilsTest
add_executable(cardUtilsTest cardUtilsTest.cpp)
target_link_libraries(cardUtilsTest PUBLIC
    gtest_main
    cardUtils
)

add_test(
    NAME cardUtilsTest
    COMMAND cardUtilsTest
)