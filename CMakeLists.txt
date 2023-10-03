cmake_minimum_required(VERSION 3.27)

project(TrucoSolver C CXX)

enable_testing()
include(CTest)

set(ENV{SOURCE_FOLDER_PATH} ${CMAKE_SOURCE_DIR}/Source)
set(ENV{EXTERNAL_FOLDER_PATH} ${CMAKE_SOURCE_DIR}/External)
set(ENV{TESTS_FOLDER_PATH} ${CMAKE_SOURCE_DIR}/Tests)


add_subdirectory(Tests)

add_subdirectory(Source)

add_subdirectory(External)
