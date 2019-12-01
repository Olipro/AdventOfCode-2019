cmake_minimum_required(VERSION 3.2)
project(AOC2019)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(day1 1/day1.cpp)
add_executable(day2 2/day2.cpp)
add_executable(day3 3/day3.cpp)
add_executable(day4 4/day4.cpp)
add_executable(day5 5/day5.cpp)
add_executable(day6 6/day6.cpp)
add_executable(day7 7/day7.cpp)
