cmake_minimum_required(VERSION 3.27)

project(SampleGenerator C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

add_executable(envidoSampleGenerator envidoSampleGenerator.cpp)
add_executable(trucoSampleGenerator trucoSampleGenerator.cpp)

target_link_libraries(envidoSampleGenerator PUBLIC deck cardUtils)
target_link_libraries(trucoSampleGenerator PUBLIC deck cardUtils)
