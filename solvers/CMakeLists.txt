cmake_minimum_required(VERSION 3.27)

project(Solvers C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

add_executable(trucoSolver trucoSolver.cpp)
add_executable(envidoSolver envidoSolver.cpp)

target_link_libraries(trucoSolver PUBLIC deck cardUtils)
