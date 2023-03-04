#include <iostream>
#include "src/expression.hpp"

int main() {
	std::cout << "Hello, World!" << std::endl;
	using T = int;
	
	
//	auto x = Number<T>(69);
//	auto y = Number<T>(1);
	auto x = std::make_shared< Number<T>>(69);
	auto y = std::make_shared< Number<T>>(1);
//	auto sum = Add<T>(x, y);
	auto sum = Add<T>(x, y);
	auto res = sum.eval();
	
//	auto x = std::make_unique<Number<T>>(69);
//	auto y = std::make_unique<Number<T>>(1);
//	T v = x->eval() + y->eval();
//	auto expr2 = Add<int>(v, std::move(x), std::move(y) );
//	auto z = Add<int>(70);

	return 0;
}
