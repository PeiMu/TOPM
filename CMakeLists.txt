cmake_minimum_required(VERSION 3.17)
project(TOPM)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(isl)

add_executable(TOPM main.cpp)