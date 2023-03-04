#include <iostream>
#include "src/expression.hpp"

int main() {
	std::cout << "Hello, World!" << std::endl;
	using T = int;
	
	
	auto x = std::make_shared< Number<T>>(5);
	auto y = std::make_shared< Number<T>>(1);
	auto sum = std::make_shared< AddNode<T>>(x, y);
	
	auto z = std::make_shared< Number<T>>(10);
	auto R = MultNode<T>(sum, z);
//	auto res = sum2.eval();
	R.eval();
	R.deriv_ = 1;
	R.differentiate();
	sum->differentiate();
	
	// (x + y) * z
	
	return 0;
}
