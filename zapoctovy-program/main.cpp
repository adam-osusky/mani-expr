#include <iostream>
#include "src/expression.hpp"

int main() {
	std::cout << "Hello, World!" << std::endl;
	using T = int;
	
	
	auto x = Number<int>(69, NodeType::Num);
	auto y = Number<int>(1, NodeType::Num);
	
	
	
//	auto x_ptr = ptr_node<T>(&x);

//	auto x = std::make_unique<Number<T>>(69);
//	auto y = std::make_unique<Number<T>>(1);
//	T v = x->eval() + y->eval();
//	auto expr2 = Add<int>(v, std::move(x), std::move(y) );
//	auto z = Add<int>(70);

	return 0;
}
