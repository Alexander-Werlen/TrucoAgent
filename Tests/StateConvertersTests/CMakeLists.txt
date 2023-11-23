cmake_minimum_required(VERSION 3.27)

project(EventsInterpreterTest C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#envidoConverter_NIC_to_NAEI
add_executable(envidoConverter_NIC_to_NAEI_Test envidoConverter_NIC_to_NAEI_Test.cpp)
target_link_libraries(envidoConverter_NIC_to_NAEI_Test PUBLIC
    gtest_main
    envidoConverter_NIC_to_NAEI
)

add_test(
    NAME envidoConverter_NIC_to_NAEI_Test
    COMMAND envidoConverter_NIC_to_NAEI_Test
)