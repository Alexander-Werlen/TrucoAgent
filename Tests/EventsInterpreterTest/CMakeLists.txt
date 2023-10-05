cmake_minimum_required(VERSION 3.27)

project(EventsInterpreterTest C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


#eventsInterpreterTest
add_executable(eventsInterpreterTest eventsInterpreterTest.cpp)
target_link_libraries(eventsInterpreterTest PUBLIC
    gtest_main
    eventsInterpreter
)

add_test(
    NAME eventsInterpreterTest
    COMMAND eventsInterpreterTest
)