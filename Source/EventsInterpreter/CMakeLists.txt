cmake_minimum_required(VERSION 3.27)

project(Events_Interpreter C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#eventsInterpreter
add_library(eventsInterpreter STATIC eventsInterpreter.cpp eventsInterpreter.hpp)

