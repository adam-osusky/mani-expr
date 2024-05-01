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
	auto & x = expr.create_variable(2.0, "x");
	auto & y = expr.create_variable(3.0, "y");
	
	expr.differentiate(); // trying to differentiate without initializes expression
	
	expr.set_expr( (5.0*x + 4.25*x) + (x*3.2 + 4.2 *x)/1.0 + y*0.0 + 0.0 + (x + x) * (x - x) - x + y );
	expr.differentiate();
	
	auto dx = x.derivative;
	auto dy = y.derivative;
	
	expr.normalize();
	expr.differentiate();
	
	assert(are_equal(x.derivative, dx));
	assert(are_equal(y.derivative, dy));
	
	std::cout << "Test 01 OK" << std::endl;
}

void test_02() {
	using T = double;
	bool ok = true;
	
	Expression<T> expr1;
	Expression<T> expr2;
	Expression<T> expr3;
	auto & x = expr1.create_variable(2.0, "x");
	auto & y = expr2.create_variable(3.0, "y");
	auto & z = expr3.create_variable(3.0, "y");
	
	// not initializes should not throw sigseg
	expr1.normalize();
	expr1.factorize();
	expr1.simplify();
	expr1.evaluate();
	
	expr2.set_expr(12.0*x - x*6.0 - 5.0*x +4.0*x);
	expr2.normalize();
	expr2.differentiate();
	assert(are_equal(x.derivative, 5.0));
	
	try {
		expr3.get_expr();
	} catch (const std::runtime_error& e) {
	} catch (const std::exception& e) {
		std::cerr << "Caught an exception: " << e.what() << std::endl;
		ok = false;
	} catch (...) {
		std::cerr << "Caught an unknown exception" << std::endl;
		ok = false;
	}
	
	expr3.set_expr( expr2.get_expr());
	expr3.differentiate();
	
	if (ok) {
		std::cout << "Test 02 OK" << std::endl;
	} else {
		std::cout << "Test 02 NOT OK" << std::endl;
	}
}

int main() {
	test_01();
	test_02();
	
	std::cout << "All tests passed!" << std::endl;
	
	return 0;
}