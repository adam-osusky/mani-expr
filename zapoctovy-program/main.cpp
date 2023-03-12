#include <iostream>
#include "src/expression.hpp"

int main() {
	std::cout << "Hello, World!" << std::endl;
	using T = double;
	
	Var<T> x = Var(4.0);
	Var<T> y = Var(3.0);
	Var<T> z = Var(10.0);
	
	auto xn = std::make_unique< Number<T>>(x);
	auto yn = std::make_unique< Number<T>>(y);
	auto zn = std::make_unique< Number<T>>(z);
	auto sum = std::make_unique< AddNode<T>>(std::move(xn), std::move(yn));
	auto R = MultNode<T>(std::move(sum), std::move(zn));
	auto res = R.eval();
	R.deriv_ = 1;
	R.differentiate();
	R.children_[0]->differentiate();

	
//	auto x = std::make_unique< Number<T>>(5);
//	auto y = std::make_unique< Number<T>>(1);
//	auto sum = std::make_unique< AddNode<T>>(std::move(x), std::move(y));
//
//	auto z = std::make_unique< Number<T>>(10);
//	auto R = MultNode<T>(std::move(sum), std::move(z));
//	auto res = R.eval();
////	R.eval();
//	R.deriv_ = 1;
//	R.differentiate();
//	R.children_[0]->differentiate();
////	sum->differentiate();

	// (x + y) * z   ---- (5 + 1) * 10

	return 0;
}
