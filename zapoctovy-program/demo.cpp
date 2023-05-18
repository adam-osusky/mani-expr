#include <iostream>
#include <limits>
#include "src/Expression.hpp"

int main() {
	// example of gradient optimization
	using T = long double;
	
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
	return 0;
}
