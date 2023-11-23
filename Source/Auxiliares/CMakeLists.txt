cmake_minimum_required(VERSION 3.27)

project(Auxiliares C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#deck
add_library(deck STATIC deck.cpp deck.hpp)

#cardUtils
add_library(cardUtils STATIC cardUtils.cpp cardUtils.hpp)

target_link_libraries(cardUtils PUBLIC deck)
