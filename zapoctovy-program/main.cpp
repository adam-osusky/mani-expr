#include <iostream>
#include "src/expression.hpp"

int main() {
	std::cout << "Hello, World!" << std::endl;
	using T = double;
	
	
	auto x = std::make_unique< Number<T>>(5);
	auto y = std::make_unique< Number<T>>(1);
	auto sum = std::make_unique< AddNode<T>>(std::move(x), std::move(y));
	
	auto z = std::make_unique< Number<T>>(10);
	auto R = MultNode<T>(std::move(sum), std::move(z));
	auto res = R.eval();
//	R.eval();
	R.deriv_ = 1;
	R.differentiate();
	R.children_[0]->differentiate();
//	sum->differentiate();

	// (x + y) * z   ---- (5 + 1) * 10

	return 0;
}
