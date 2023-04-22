#include <iostream>
#include "src/Expression.hpp"

int main() {
	using T = double;
	
	auto expr = Expression<T>();
	
	auto & x = expr.create_variable(4.0, "x");
	auto & y = expr.create_variable(3.0, "y");
	auto & z = expr.create_variable(10.0, "z");
	
//	auto r = (expr["x"]*expr["x"]) / (expr["z"]-expr["x"]);
	auto r = 4.0 + (expr["x"]*expr["x"]) / 2.0;
//	auto r = expr["z"]+expr["x"];
//	auto r = expr["x"] + expr["x"];
	
	expr.root = std::move(r);
	expr.differentiate();
	
	std::cout << "derivation of x : " << expr["x"].derivative << std::endl;
	std::cout << "derivation of y : " << expr["y"].derivative << std::endl;
	std::cout << "derivation of z : " << expr["z"].derivative << std::endl;
	
	std::cout << expr.to_string() << std::endl;
	
	return 0;
}
