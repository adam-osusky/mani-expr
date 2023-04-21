//
// Created by adas on 4/20/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSION_HPP

#include <map>
#include <set>
#include <sstream>
#include "ExpressionNode.hpp"

template<typename T>
class Expression {
public:
	Var<T> & create_variable(T value, const std::string & name);
	auto operator[](const std::string x) -> Var<T> &;
	void differentiate();
	std::string to_string();
	
	std::map< std::string, Var<T>, std::less<>> variables;
	std::unique_ptr<ExpressionNode<T>> root;
private:
	void build_topo(ExpressionNode<T> * node,
					std::set<ExpressionNode<T> *> &visited,
					std::vector<ExpressionNode<T> *> &topo);
	void traverse_for_string(ExpressionNode<T> * node, std::stringstream & ss);
	std::vector<ExpressionNode<T> *> topo_order;
};

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
	
	traverse_for_string(root.get(), ss);
	
	return ss.str();
}

template<typename T>
void Expression<T>::traverse_for_string(ExpressionNode<T> * node, std::stringstream & ss) {
	switch (node->n_type_) {
		case NodeType::Num:
		{
			auto * p = dynamic_cast<Number<T>*>(node);
			ss << p->val_ref.name;
			break;
		}
		case NodeType::Addition:
		{
			ss << "(";
			traverse_for_string(node->children_[0].get(), ss);
			auto * p = dynamic_cast<AddNode<T>*>(node);
			if (p->sub_ == T(1)) {
				ss << " + ";
			} else {
				ss << " - ";
			}
			traverse_for_string(node->children_[1].get(), ss);
			ss << ")";
			break;
			
		}
		case NodeType::Multiplication:
//			ss << "(";
			traverse_for_string(node->children_[0].get(), ss);
			ss << " * ";
			traverse_for_string(node->children_[1].get(), ss);
//			ss << ")";
			break;
		case NodeType::Denominator:
			ss << "(1/";
			traverse_for_string(node->children_[0].get(), ss);
			ss << ")";
			break;
		default:
			break;
	}
}

#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
