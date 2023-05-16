#include <iostream>
#include "src/Expression.hpp"

int main() {
	using T = double;
	
	auto expr = Expression<T>();
	
	auto & x = expr.create_variable(4.0, "x");
	auto & y = expr.create_variable(3.0, "y");
	auto & z = expr.create_variable(10.0, "z");
	
//	auto r = (x*x) / (z-x) + 2.0;
//	auto r = x / 2.0;
//	auto p = std::make_unique<Number<T>>(x);
//	auto r = 2.0 * p->val_ref;
//	auto r = 12.0*x - x*6.0 - 5.0*x +4.0*x;
//	auto r = (x+x)/1 * 10.0;
//	auto r = (x+x)/1.0;
	auto r = 0.0 + x + x;
//	auto r = x * 1.0;
//	std::cout << not_primitive<decltype(r)> << std::endl;
//	std::cout << not_primitive<decltype(2.0)> << std::endl;

	expr.root = std::move(r);
	expr.differentiate();
	
	std::cout << "derivation of x : " << expr["x"].derivative << std::endl;
	std::cout << "derivation of y : " << expr["y"].derivative << std::endl;
	std::cout << "derivation of z : " << expr["z"].derivative << std::endl;
	
	std::cout << expr.to_string() << std::endl;
	expr.factorize();
	std::cout << expr.to_string() << std::endl;
	expr.simplify();
	std::cout << expr.to_string() << std::endl;
	
//	expr.factorize();
//	std::cout << expr.to_string() << std::endl;
	
//	expr.differentiate();
//
//	std::cout << "derivation of x : " << expr["x"].derivative << std::endl;
//	std::cout << "derivation of y : " << expr["y"].derivative << std::endl;
//	std::cout << "derivation of z : " << expr["z"].derivative << std::endl;
	
	return 0;
}
