#include <iostream>
#include "src/Expression.hpp"

int main() {
	std::cout << "Hello, World!" << std::endl;
	using T = double;
	
	auto expr = Expression<T>();
	
	auto & x = expr.create_variable(4.0, "x");
	auto & y = expr.create_variable(3.0, "y");
	auto & z = expr.create_variable(10.0, "z");
	
	auto r = (x*x) / (z+x);
	
	r->eval();
	r->deriv_ = 1.0;
	r->differentiate();

	r->children_[0]->differentiate();
	r->children_[1]->differentiate();

	r->children_[0]->children_[0]->differentiate();
	r->children_[0]->children_[1]->differentiate();

	r->children_[1]->children_[0]->differentiate(); // 1/n

	r->children_[1]->children_[0]->children_[0]->differentiate();
	r->children_[1]->children_[0]->children_[1]->differentiate();
	
	expr.root = std::move(r);
	
	return 0;
}
