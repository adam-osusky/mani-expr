//
// Created by adas on 4/20/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSION_HPP

#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include "ExpressionNode.hpp"

template<typename T>
class Expression {
public:
	Var<T> & create_variable(T value, const std::string & name);
	auto operator[](const std::string x) -> Var<T> &;
	void differentiate();
	std::string to_string();
	void factorize();
	
	std::map< std::string, Var<T>, std::less<>> variables;
	std::unique_ptr<ExpressionNode<T>> root;
private:
	void build_topo(ExpressionNode<T> * node,
					std::set<ExpressionNode<T> *> &visited,
					std::vector<ExpressionNode<T> *> &topo);
	std::vector<ExpressionNode<T> *> topo_order;
	std::unique_ptr<ExpressionNode<T>> factorize_impl(ExpressionNode<T> * node);
};

template<typename T>
std::unique_ptr<ExpressionNode<T>>  Expression<T>::factorize_impl(ExpressionNode<T> *node) {
	bool child_factorized = false;
	for (auto && child : node->children_) {
		auto fact = factorize_impl(child.get());
		if (fact) {
			child_factorized = true; //TODO change childs;
		}
	}
	if (node->n_type_ == NodeType::Addition) {
		if (node->children_[0]->n_type_ == NodeType::Num and node->children_[1]->n_type_ == NodeType::Num) {
			auto * l = dynamic_cast<Number<T>*>(node->children_[0].get());
			auto * p = dynamic_cast<Number<T>*>(node->children_[1].get());
			if (l->val_ref.name == p->val_ref.name) {
				std::cout << "hit" << std::endl;
				if (dynamic_cast<AddNode<T>*>(node)->sub_ == T(-1)){
					return std::make_unique<ConstantNode<T>>(0);
				}
				return T(2) * p->val_ref;
			}
		}
	}
	return std::unique_ptr<ExpressionNode<T>>();
}

template<typename T>
void Expression<T>::factorize() {
	auto new_r = factorize_impl(root.get());
	if (new_r) {
		root = std::move(new_r);
	}
	std::cout << "expr.factorize() = " << new_r << std::endl;
}

template<typename T>
Var<T> & Expression<T>::create_variable(T value, const std::string & name) {
	variables.insert_or_assign(name, Var<T>(value, name));
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

template<typename T>
std::string Expression<T>::to_string() {
	std::stringstream ss;
	
	root->to_string(ss);
	
	return ss.str();
}

#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
