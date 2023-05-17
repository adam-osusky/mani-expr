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
	auto & x = expr.create_variable(-10.5, "x");
	auto & z = expr.create_variable(-3.25, "z");

	auto r = (x*x) / (z-x) + 2.0;
	expr.set_expr(std::move(r));
	expr.differentiate();
	assert(are_equal(x.derivative, -0.799048662185669));
	assert(are_equal(z.derivative, -2.0975029468536377));

	std::cout << "Test 01 OK" << std::endl;
}

void test_02() {
	using T = long double;
	
	Expression<T> expr;
	auto & x = expr.create_variable(-10.5, "x");
	auto & y = expr.create_variable(0.47, "y");
	auto & z = expr.create_variable(-3.25, "z");
	
	auto r = ((-2.45L * x + 545.6L * y) * (-26.6L * x + 54.1L * y) * (219.56L * x + 5.16L * y) + (-44.495L * z - 14.156L) * (10.0L * x + y) - (55.35L* x + y) * (3.0L * x + y)) / (x + y + z);
	expr.set_expr(std::move(r));
	
	expr.differentiate();
	assert(are_equal(x.derivative, -1729956.9685693283L));
	assert(are_equal(y.derivative, 32568475.406540617L));
	assert(are_equal(z.derivative, 1122599.8577188218L));
	
	expr.differentiate();
	assert(are_equal(x.derivative, -1729956.9685693283L));
	assert(are_equal(y.derivative, 32568475.406540617L));
	assert(are_equal(z.derivative, 1122599.8577188218L));
	
	std::cout << "Test 02 OK" << std::endl;
}

void test_03() {
	Expression<double> expr;
	auto& x = expr.create_variable(6515.144, "x");
	auto& y = expr.create_variable(-4651.156, "y");
	
	expr.set_expr(((x * x - y) / (x + y)) / ((x - y) * (x + y)));
	expr.differentiate();
	
	assert(are_equal(x.derivative, -9.361829541361019e-07));
	assert(are_equal(y.derivative, -1.0760848082082438e-06));
	
	std::cout << "Test 03 OK" << std::endl;
}

int main() {
	test_01();
	test_02();
	test_03();
	
	std::cout << "All tests passed!" << std::endl;
	
	return 0;
}