cmake_minimum_required(VERSION 3.27)

project(envidoConverter_NIC_to_NAEI C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#eventsInterpreter
add_library(envidoConverter_NIC_to_NAEI STATIC envidoConverter_NIC_to_NAEI.cpp envidoConverter_NIC_to_NAEI.hpp)

target_link_libraries(envidoConverter_NIC_to_NAEI PRIVATE eventsInterpreter)
