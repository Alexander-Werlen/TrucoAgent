cmake_minimum_required(VERSION 3.27)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O2")

project(TrucoSolver C CXX)

enable_testing()
include(CTest)

add_subdirectory(source)

add_subdirectory(external)

add_subdirectory(solvers)

add_subdirectory(playing)

