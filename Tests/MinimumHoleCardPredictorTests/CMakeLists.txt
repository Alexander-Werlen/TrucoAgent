cmake_minimum_required(VERSION 3.27)

project(MinimumHoleCardPredictorTest C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#minimumHoleCardPredictorTest
add_executable(minimumHoleCardPredictorTest minimumHoleCardPredictorTest.cpp)
target_link_libraries(minimumHoleCardPredictorTest PUBLIC
    gtest_main
    minimumHoleCardPredictor
)

add_test(
    NAME minimumHoleCardPredictorTest
    COMMAND minimumHoleCardPredictorTest
)