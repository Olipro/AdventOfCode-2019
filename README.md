# Advent of Code 2019

This is a repository containing my solutions to AoC 2019.

All of them are in C++17 and are built as individual executables.
Only the standard library is used so it should theoretically build on
any platform with a modern-enough compiler.

## Compiling

CMake is easy enough, but you could alternatively build manually using
something along the lines of:

`clang++ -std=c++17 -O3 -I. -o day5 5/day5.cpp` - Obviously this would
build day 5. Substitute numbers as you wish if you don't want to use
CMake.

## Usage

All programs take at least one argument. The first argument is always
intended to be a file. I always **strip the trailing newline** from any
input files (should they have it) - I cannot guarantee correct parsing
behaviour if you do not.

On days where the input is not given a a file, enter a dummy value for
the first argument. subsequent arguments should be whatever is provided.