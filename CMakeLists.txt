project(ImportExternalProject)
cmake_minimum_required(VERSION 3.17)
project(TOPM)

set(CMAKE_CXX_STANDARD 14)

# Find gmp
find_path(GMP_INCLUDE_DIR NAME gmp.h HINTS /usr /usr/local PATH_SUFFIXES include include/x86_64-linux-gnu)
find_library(GMP_LIBRARY NAMES gmp HINTS /usr /usr/local PATH_SUFFIXES lib lib64 lib/x86_64-linux-gnu)
if(NOT GMP_INCLUDE_DIR)
    message(FATAL_ERROR "Please export CMAKE_INCLUDE_PATH to directory where gmp.h locates at.")
endif()
if(NOT GMP_LIBRARY)
    message(FATAL_ERROR "Please export CMAKE_LIBRARY_PATH to directory where libgmp.so locates at.")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(cmake/isl.cmake)

add_executable(TOPM main.cpp)
if(EXISTS "${ISL_LIB}")
    message(STATUS "${ISL_LIB} is exists")
else()
    message(STATUS "need to build ${ISL_LIB}")
    add_dependencies(TOPM ISL)
endif()
target_link_libraries(TOPM ${ISL_LIB} ${GMP_LIBRARY})
target_include_directories(TOPM PRIVATE ${ISL_INCLUDE_DIR} ${GMP_INCLUDE_DIR})
