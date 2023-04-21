#include <iostream>
#include "src/Expression.hpp"

int main() {
	using T = double;
	
	auto expr = Expression<T>();
	
	auto & x = expr.create_variable(4.0, "x");
	auto & y = expr.create_variable(3.0, "y");
	auto & z = expr.create_variable(10.0, "z");
	
	auto r = (expr["x"]*expr["x"]) / (expr["z"]-expr["x"]);
//	auto r = expr["z"]+expr["x"];
	
	expr.root = std::move(r);
	expr.differentiate();
	
	std::cout << "derivation of x : " << expr["x"].derivative << std::endl;
	std::cout << "derivation of y : " << expr["y"].derivative << std::endl;
	std::cout << "derivation of z : " << expr["z"].derivative << std::endl;
	
	return 0;
}
