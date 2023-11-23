cmake_minimum_required(VERSION 3.27)

project(TrucoSolver C CXX)

enable_testing()
include(CTest)

add_subdirectory(tests)

add_subdirectory(source)

add_subdirectory(external)
