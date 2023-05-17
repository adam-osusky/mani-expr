//
// Created by adas on 5/17/23.
//
#include <iostream>
#include <cassert>
#include <cmath>
#include "../src/Expression.hpp"

template<typename T>
bool are_equal(T a, T b, T tolerance = 1e-6) {
	return std::abs(a - b) < tolerance;
}

void test_01() {
	using T = double;
	
	Expression<T> expr;
	Var<T>& x = expr.create_variable(2.0, "x");
	Var<T>& y = expr.create_variable(3.0, "y");
	
	auto z = x + y;
	expr.set_expr(std::move(z));
	assert(are_equal<T>(expr.evaluate(), 5.0));
	
	x.value = 4.0;
	y.value = 5.0;
	assert(are_equal<T>(expr.evaluate(), 9.0));
	
	auto w = expr.get_expr() * x;
	expr.set_expr(std::move(w));
	assert(are_equal<T>(expr.evaluate(), 36.0));
	
	std::cout << "Test 01 OK" << std::endl;
}

void test_02() {
	using T = double;
	
	Expression<T> expr;
	Var<T>& x = expr.create_variable(2.0, "x");
	Var<T>& y = expr.create_variable(3.0, "y");
	Var<T>& z = expr.create_variable(4.0, "z");
	
	auto r = (x + 10.0) / (z - y) * x + 2.5;
	expr.set_expr(std::move(r));
	assert(are_equal<T>(expr.evaluate(), 26.5));
	
	r = x/y * 5.0 + z/2.5;
	expr.set_expr(std::move(r));
	assert(are_equal<T>(expr.evaluate(), 4.933333333333334));
	
	std::cout << "Test 02 OK" << std::endl;
}

void test_03() {
	using T = long double;
	
	Expression<T> expr;
	Var<T>& x = expr.create_variable(4.519, "x");
	Var<T>& y = expr.create_variable(-3.141941, "y");
	Var<T>& z = expr.create_variable(-0.69, "z");
	
	auto r = (x + z) / (z - y) * x - z*x*y / (x / z);
	expr.set_expr(std::move(r));
	assert(are_equal<T>(expr.evaluate(), 8.552838697650843));
	
	r = x/y;
	expr.set_expr(std::move(r));
	assert(are_equal<T>(expr.evaluate(), -1.4382828958277702));
	
	std::cout << "Test 03 OK" << std::endl;
}

int main() {
	test_01();
	test_02();
	test_03();
	
	std::cout << "All tests passed!" << std::endl;
	
	return 0;
}