cmake_minimum_required(VERSION 3.27)

project(Auxiliares C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(handToEnvidoPointsConverter STATIC handToEnvidoPointsConverter.cpp handToEnvidoPointsConverter.hpp)
