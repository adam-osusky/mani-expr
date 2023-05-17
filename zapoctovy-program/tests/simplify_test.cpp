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
	auto &x = expr.create_variable(-10.5, "x");
	T a = 3;
	T b = 4;
	T one = 1;
	T zero = 0;
	
	{
		expr.set_expr(std::make_unique<AddNode<T>>(std::make_unique<ConstantNode<T>>(a),
												   std::make_unique<ConstantNode<T>>(b)));
		expr.simplify();
		std::stringstream ss;
		ss << a + b;
		assert(expr.to_string() == ss.str());
	}
	
	{
		expr.set_expr(std::make_unique<MultNode<T>>(std::make_unique<ConstantNode<T>>(a),
													std::make_unique<ConstantNode<T>>(b)));
		expr.simplify();
		std::stringstream ss;
		ss << a * b;
		assert(expr.to_string() == ss.str());
	}
	
	{
		expr.set_expr(std::make_unique<AddNode<T>>(std::make_unique<ConstantNode<T>>(a),
												   std::make_unique<ConstantNode<T>>(b), true));
		expr.simplify();
		std::stringstream ss;
		ss << a - b;
		assert(expr.to_string() == ss.str());
	}
	
	{
		expr.set_expr(x / one);
		expr.simplify();
		assert(expr.to_string() == "x");
		
		expr.set_expr(x + zero);
		expr.simplify();
		assert(expr.to_string() == "x");
		
		expr.set_expr(x - zero);
		expr.simplify();
		assert(expr.to_string() == "x");
		
		expr.set_expr(zero + x);
		expr.simplify();
		assert(expr.to_string() == "x");
		
		expr.set_expr(zero - x);
		expr.simplify();
		assert(expr.to_string() == "-1 * x");
		
		expr.set_expr(zero * x);
		expr.simplify();
		assert(expr.to_string() == "0");
		
		expr.set_expr(x * zero);
		expr.simplify();
		assert(expr.to_string() == "0");
		
		expr.set_expr(one * x);
		expr.simplify();
		assert(expr.to_string() == "x");
		
		expr.set_expr(x * one);
		expr.simplify();
		assert(expr.to_string() == "x");
	}
	
	std::cout << "Test 01 OK" << std::endl;
}

void test_02() {
	using T = double;
	
	Expression<T> expr;
	auto &x = expr.create_variable(-10.5, "x");
	auto &y = expr.create_variable(0.47, "y");
	T a = 3;
	T b = 4;
	
	expr.set_expr(x*a + x*b);
	expr.factorize();
	assert(expr.to_string() == "(3 + 4) * x");
	
	expr.set_expr(x*a + y*b);
	expr.factorize();
	assert(expr.to_string() == "(x * 3 + y * 4)");
	
	expr.set_expr(x*a + b*x);
	expr.factorize();
	assert(expr.to_string() == "(3 + 4) * x");
	
	expr.set_expr(a*x + b*x);
	expr.factorize();
	assert(expr.to_string() == "(3 + 4) * x");
	
	expr.set_expr(a*x + x*b);
	expr.factorize();
	assert(expr.to_string() == "(3 + 4) * x");
	
	expr.set_expr(a*x + x);
	expr.factorize();
	assert(expr.to_string() == "(3 + 1) * x");
	
	expr.set_expr(x*a + x);
	expr.factorize();
	assert(expr.to_string() == "(3 + 1) * x");
	
	
	expr.set_expr(x + a*x);
	expr.factorize();
	assert(expr.to_string() == "(3 + 1) * x");
	
	expr.set_expr(x + x*a);
	expr.factorize();
	assert(expr.to_string() == "(3 + 1) * x");
	
	expr.set_expr(x + x);
	expr.factorize();
	assert(expr.to_string() == "2 * x");
	
	
	
	expr.set_expr(x*a - x*b);
	expr.factorize();
	assert(expr.to_string() == "(3 - 4) * x");
	
	expr.set_expr(x*a - y*b);
	expr.factorize();
	assert(expr.to_string() == "(x * 3 - y * 4)");
	
	expr.set_expr(x*a - b*x);
	expr.factorize();
	assert(expr.to_string() == "(3 - 4) * x");
	
	expr.set_expr(a*x - b*x);
	expr.factorize();
	assert(expr.to_string() == "(3 - 4) * x");
	
	expr.set_expr(a*x - x*b);
	expr.factorize();
	assert(expr.to_string() == "(3 - 4) * x");
	
	expr.set_expr(a*x - x);
	expr.factorize();
	assert(expr.to_string() == "(3 - 1) * x");
	
	expr.set_expr(x*a - x);
	expr.factorize();
	assert(expr.to_string() == "(3 - 1) * x");
	
	
	expr.set_expr(x - a*x);
	expr.factorize();
	assert(expr.to_string() == "(3 - 1) * x");
	
	expr.set_expr(x - x*a);
	expr.factorize();
	assert(expr.to_string() == "(3 - 1) * x");
	
	expr.set_expr(x - x);
	expr.factorize();
	assert(expr.to_string() == "0");

	
	std::cout << "Test 02 OK" << std::endl;
}

void test_03() {
	using T = double;
	
	Expression<T> expr;
	auto & x = expr.create_variable(-10.5, "x");
	auto & y = expr.create_variable(0.47, "y");
	auto & z = expr.create_variable(-3.25, "z");
	
	
	expr.set_expr(12.0*x - x*6.0 - 5.0*x +4.0*x);
	expr.normalize();
	assert(expr.to_string() == "5 * x");
	
	expr.set_expr((x+x)/1.0 + 10.0);
	expr.normalize();
	assert(expr.to_string() == "(2 * x + 10)");
	
	expr.set_expr( (5.0*x + 4.25*x) * (x*3.2 - 4.2 *x) );
	expr.normalize();
	assert(expr.to_string() == "9.25 * x * -1 * x");
	
	expr.set_expr( (5.0*x + 4.25*x) + (x*3.2 - 4.2 *x) );
	expr.normalize();
	assert(expr.to_string() == "8.25 * x");
	
	expr.set_expr( (5.0*x + 4.25*x) + (x*3.2 + 4.2 *x)/1.0 + y*0.0 + 0.0 + (x + x) * (x - x) - x + y);
	expr.normalize();
	assert(expr.to_string() == "(15.65 * x + y)");
	
	std::cout << "Test 03 OK" << std::endl;
}

int main() {
	test_01();
	test_02();
	test_03();
	
	std::cout << "All tests passed!" << std::endl;
	
	return 0;
}
