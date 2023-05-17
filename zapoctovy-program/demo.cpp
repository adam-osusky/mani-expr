#include <iostream>
#include "src/Expression.hpp"

void optimization() {
	using T = double;
	
	auto expr = Expression<T>();
	auto & x = expr.create_variable(4.0, "x");
	auto & y = expr.create_variable(3.0, "y");
	
	expr.set_expr(x*x + y*y);
	
	for (std::size_t i = 0; i < 10000; ++i) {
		expr.differentiate();
		x.value -= 0.01 * x.derivative;
		y.value -= 0.01 * y.derivative;
	}
	std::cout << "Optimization of " << expr.to_string() << std::endl;
	std::cout << "x : " << x.value << std::endl;
	std::cout << "y : " << y.value << std::endl;
	std::cout << std::endl;
}

void o() {
	using T = double;
	
	auto expr = Expression<T>();
	auto & a = expr.create_variable(1.0, "a");
	auto & b = expr.create_variable(2.0, "b");
	auto & c = expr.create_variable(3.0, "c");
	auto & d = expr.create_variable(4.0, "d");
	auto & e = expr.create_variable(5.0, "e");
	
	
}

int main() {
	optimization();
	return 0;
}
