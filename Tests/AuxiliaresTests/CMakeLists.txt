cmake_minimum_required(VERSION 3.27)

project(AuxiliaresTests C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


#handToEnvidoPointsConverterTest
add_executable(handToEnvidoPointsConverterTest handToEnvidoPointsConverterTest.cpp)
target_link_libraries(handToEnvidoPointsConverterTest PUBLIC
    gtest_main
    handToEnvidoPointsConverter
)

add_test(
    NAME handToEnvidoPointsConverterTest
    COMMAND handToEnvidoPointsConverterTest
)