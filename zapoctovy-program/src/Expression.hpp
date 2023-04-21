//
// Created by adas on 4/20/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSION_HPP

#include <map>
#include <set>
#include "ExpressionNode.hpp"

template<typename T>
class Expression {
public:
	Var<T> & create_variable(T value, const std::string & name) {
		variables.insert_or_assign(name, Var<T>(value));
		return variables[name];
	}
	
	auto operator[](const std::string x) -> Var<T> & {
		return variables[x];
	}
	
	void differentiate() {
		std::set< ExpressionNode<T>*> visited;
		std::vector< ExpressionNode<T>*> topo;
		
		build_topo(root.get(), visited, topo);
		
		root->eval();
		root->deriv_ = T(1);
		
		for (auto it = topo.rbegin(); it != topo.rend() ; ++it) {
			(*it)->differentiate();
		}
	}
	
	std::map< std::string, Var<T>, std::less<>> variables;
	std::unique_ptr<ExpressionNode<T>> root;

private:
	void build_topo(ExpressionNode<T> * node,
					std::set<ExpressionNode<T> *> &visited,
					std::vector<ExpressionNode<T> *> &topo) {
		if (!visited.contains(node)) {
			visited.insert(node);
			for (auto && child : node->children_) {
//				auto * child_ptr = child.get();
				build_topo(child.get(), visited, topo);
			}
		}
		topo.push_back(node);
	}
};


#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
