cmake_minimum_required(VERSION 3.23)
project(zapoctovy_program)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall)

add_executable(demo demo.cpp src/ExpressionNode.hpp src/Expression.hpp)

add_executable(creation_eval_test tests/creation_eval_test.cpp src/ExpressionNode.hpp src/Expression.hpp)
add_executable(differentiation_test tests/differentiation_test.cpp src/ExpressionNode.hpp src/Expression.hpp)
add_executable(simplify_test tests/simplify_test.cpp src/ExpressionNode.hpp src/Expression.hpp)
add_executable(complex_test tests/complex_test.cpp src/ExpressionNode.hpp src/Expression.hpp)
