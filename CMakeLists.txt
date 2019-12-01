cmake_minimum_required(VERSION 3.2)
project(AOC2019)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(day1 1/day1.cpp)
add_executable(day2 2/day2.cpp)
