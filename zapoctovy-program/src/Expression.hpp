//
// Created by adas on 4/20/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSION_HPP

#include <map>
#include "ExpressionNode.hpp"

template<typename T>
class Expression {
public:
	
	Var<T> & create_variable(T value, const std::string & name) {
		variables.insert_or_assign(name, Var<T>(value));
		return variables[name];
	}
	
	std::map< std::string, Var<T>> variables;
//	ExpressionNode<T> root;
private:
};


#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
