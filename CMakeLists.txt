project(ImportExternalProject)
cmake_minimum_required(VERSION 3.17)
project(TOPM)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(cmake/isl.cmake)

add_executable(TOPM main.cpp)

if(NOT ISL_LIB)
    add_dependencies(TOPM ISL)
endif()
target_link_libraries(TOPM ${ISL_LIB})
target_include_directories(TOPM PRIVATE ${ISL_INCLUDE_DIR})
