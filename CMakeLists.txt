cmake_minimum_required(VERSION 3.17)
project(TOPM)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(isl)
include_directories(isl/include)

add_executable(TOPM main.cpp isl_dump.cpp test_expr_compute.cpp akg_code.cpp akg_code.h)
target_link_libraries(TOPM isl)