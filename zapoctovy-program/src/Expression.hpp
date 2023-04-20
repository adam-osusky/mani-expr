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
	
	std::map< std::string, Var<T>> variables;
	ExpressionNode<T> root;
private:
};


#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
