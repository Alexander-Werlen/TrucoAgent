cmake_minimum_required(VERSION 3.27)

project(Playing C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

add_executable(playAgent playAgent.cpp)

target_link_libraries(playAgent PUBLIC deck cardUtils)
