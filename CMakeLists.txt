cmake_minimum_required(VERSION 3.8)
project(TextFlow)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp TextFlow_Tests.cpp TextFlow.hpp Surrogate.cpp)
add_executable(TextFlow ${SOURCE_FILES})