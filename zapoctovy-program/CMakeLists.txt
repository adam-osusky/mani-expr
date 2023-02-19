cmake_minimum_required(VERSION 3.23)
project(zapoctovy_program)

set(CMAKE_CXX_STANDARD 17)

add_executable(zapoctovy_program main.cpp src/expression.cpp src/expression.hpp src/OpNodes.cpp src/OpNodes.hpp)
