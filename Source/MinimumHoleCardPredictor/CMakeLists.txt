cmake_minimum_required(VERSION 3.27)

project(MinimumHoleCardPredictor C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


#minimumHoleCardPredictor
add_library(minimumHoleCardPredictor STATIC minimumHoleCardPredictor.cpp minimumHoleCardPredictor.hpp)

target_link_libraries(minimumHoleCardPredictor PRIVATE deck cardUtils)