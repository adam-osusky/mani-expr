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
	Var<T> & create_variable(T value, const std::string & name);
	auto operator[](const std::string x) -> Var<T> &;
	void differentiate();
	std::map< std::string, Var<T>, std::less<>> variables;
	std::unique_ptr<ExpressionNode<T>> root;
private:
	void build_topo(ExpressionNode<T> * node,
					std::set<ExpressionNode<T> *> &visited,
					std::vector<ExpressionNode<T> *> &topo);
	std::vector<ExpressionNode<T> *> topo_order;
};

template<typename T>
Var<T> & Expression<T>::create_variable(T value, const std::string & name) {
	variables.insert_or_assign(name, Var<T>(value));
	return variables[name];
}

template<typename T>
auto Expression<T>::operator[](const std::string x) -> Var<T> & {
	return variables[x];
}

template<typename T>
void Expression<T>::differentiate() {
	if (topo_order.empty()){
		std::set< ExpressionNode<T>*> visited;
		build_topo(root.get(), visited, topo_order);
	}

	root->eval();
	root->deriv_ = T(1);

	for (auto it = topo_order.rbegin(); it != topo_order.rend() ; ++it) {
		(*it)->differentiate();
	}
}

template<typename T>
void Expression<T>::build_topo(ExpressionNode<T> * node,
								std::set<ExpressionNode<T> *> &visited,
								std::vector<ExpressionNode<T> *> &topo) {
	if (!visited.contains(node)) {
		visited.insert(node);
		for (auto && child : node->children_) {
			build_topo(child.get(), visited, topo);
		}
	}
	topo.push_back(node);
}

#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
