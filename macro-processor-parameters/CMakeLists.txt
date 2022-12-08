cmake_minimum_required(VERSION 3.23)
project(macro_processor_parameters)

set(CMAKE_CXX_STANDARD 17)

add_executable(macro_processor_parameters main.cpp macro_processor.cpp macro_processor.hpp)
