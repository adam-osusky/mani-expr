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

//TODO big 5 for Node amd Expression
//TODO check const

template<typename T>
class Expression {
public:
	Var<T> & create_variable(T value, const std::string & name);
	auto operator[](const std::string x) -> Var<T> &;
	void differentiate();
	std::string to_string();
	void factorize();
	void simplify();
	
	std::map< std::string, Var<T>, std::less<>> variables;
	std::unique_ptr<ExpressionNode<T>> root;
private:
	void build_topo(ExpressionNode<T> * node,
					std::set<ExpressionNode<T> *> &visited,
					std::vector<ExpressionNode<T> *> &topo);
	std::vector<ExpressionNode<T> *> topo_order;
	std::unique_ptr<ExpressionNode<T>> factorize_impl(ExpressionNode<T> * node);
	std::unique_ptr<ExpressionNode<T>> simplify_impl(ExpressionNode<T> * node);
	std::unique_ptr<ExpressionNode<T>> plus_or_minus(std::unique_ptr<ExpressionNode<T>> && l, std::unique_ptr<ExpressionNode<T>> && r, bool is_minus);
	
	bool changed_graph = true;
};

template<typename T>
std::unique_ptr<ExpressionNode<T>> Expression<T>::simplify_impl(ExpressionNode<T> *node) {
	for (std::size_t i = 0; i < node->children_.size(); i++) {
		auto new_child = simplify_impl(node->children_[i].get());
		if (new_child) {
			node->children_[i] = std::move(new_child);
		}
	}
	
	// MULT
	if (node->n_type_ == NodeType::Multiplication) {
		
		//constant * something
		if (node->children_[0]->n_type_ == NodeType::Const) {
			
			//1 * something = something
			if (node->children_[0]->value_ == T(1)) {
				return std::move(node->children_[1]);
			}
			//0 * something = 0
			if (node->children_[0]->value_ == T(0)) {
				return std::make_unique< ConstantNode<T>>(0);
			}
			
			// const * const
			if (node->children_[1]->n_type_ == NodeType::Const) {
				T value = node->children_[0]->value_ * node->children_[1]->value_;
				return std::make_unique< ConstantNode<T>>(value);
			}
		}
		
		//something * constant
		if (node->children_[1]->n_type_ == NodeType::Const) {

			//something * 1 = something
			if (node->children_[1]->value_ == T(1)) {
				return std::move(node->children_[0]);
			}
			//something * 0 = 0
			if (node->children_[1]->value_ == T(0)) {
				return std::make_unique< ConstantNode<T>>(0);
			}
		}
		
		//something / 1 = something
		if (node->children_[1]->n_type_ == NodeType::Denominator && node->children_[1]->children_[0]->value_ == T(1)) {
			return std::move(node->children_[0]);
		}
	}
	
	//SUM
	if (node->n_type_ == NodeType::Addition) {
		auto sub = dynamic_cast<AddNode<T> *>(node)->sub_;
		bool is_minus = false;
		if (sub == T(-1)) is_minus = true;
		
		//a + 0 = a
		if (node->children_[1]->n_type_ == NodeType::Const && node->children_[1]->value_ == T(0)) {
			return std::move(node->children_[0]);
		}
		// 0 + a = a
		if (node->children_[0]->n_type_ == NodeType::Const && node->children_[0]->value_ == T(0)) {
			return std::move(node->children_[1]);
		}
	}
	
	//TODO 0*s, 1*s, a/1, a+0, a-0, c1 op c2 => c3
	// Perform trivial simplifications: 0*a => 0, 1*a => a, a/1 => a,
	//a+0 => a, a-0 => a  c1 op c2 => c3  where a is any expression,
	//the ci are constants, and op is any of * / + -
	
	return std::unique_ptr<ExpressionNode<T>>();
}

template<typename T>
void Expression<T>::simplify() {
	auto new_r = simplify_impl(root.get());
	std::cout << "expr.simplify() = " << new_r << std::endl;
	if (new_r) {
		root = std::move(new_r);
	}
	changed_graph = true;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>>
Expression<T>::plus_or_minus(std::unique_ptr<ExpressionNode<T>> && l, std::unique_ptr<ExpressionNode<T>> && r, bool is_minus) {
	if (is_minus) {
		return std::move(l) - std::move(r);
	} else {
		return std::move(l) + std::move(r);
	}
}

template<typename T>
std::unique_ptr<ExpressionNode<T>>  Expression<T>::factorize_impl(ExpressionNode<T> *node) {
	for (std::size_t i = 0; i < node->children_.size(); i++) {
		auto new_child = factorize_impl(node->children_[i].get());
		if (new_child) {
			node->children_[i] = std::move(new_child);
//			std::cout << this->to_string() << std::endl;
		}
	}
	// if (x==y): a*x add b*y => (a add b)*x
	if (node->n_type_ == NodeType::Addition) {
		auto sub = dynamic_cast<AddNode<T> *>(node)->sub_;
		bool is_minus = false;
		if (sub == T(-1)) is_minus = true;
		
		// MULT + something
		if (node->children_[0]->n_type_ == NodeType::Multiplication) {
			
			// MULT + MULT
			if (node->children_[1]->n_type_ == NodeType::Multiplication) {
				
				// (NUM)x * something + MULT
				if (node->children_[0]->children_[0]->n_type_ == NodeType::Num) {
					auto x = dynamic_cast<Number<T> *>(node->children_[0]->children_[0].get());
					
					// (NUM)x * something + (NUM)y * something
					if (node->children_[1]->children_[0]->n_type_ == NodeType::Num) {
						auto y = dynamic_cast<Number<T> *>(node->children_[1]->children_[0].get());
						// x == y
						if (x->val_ref.name == y->val_ref.name) {
							return plus_or_minus(std::move(node->children_[0]->children_[1]),
												 std::move(node->children_[1]->children_[1]), is_minus) * x->val_ref;
						}
						
					}
					
					// (NUM)x * something + something * (NUM)y
					if (node->children_[1]->children_[1]->n_type_ == NodeType::Num) {
						auto y = dynamic_cast<Number<T> *>(node->children_[1]->children_[1].get());
						// x == y
						if (x->val_ref.name == y->val_ref.name) {
							return plus_or_minus(std::move(node->children_[0]->children_[1]),
												 std::move(node->children_[1]->children_[0]), is_minus) * x->val_ref;
						}
					}
				}
				
				// something * (NUM)x + MULT
				if (node->children_[0]->children_[1]->n_type_ == NodeType::Num) {
					auto x = dynamic_cast<Number<T> *>(node->children_[0]->children_[1].get());
					
					// something * (NUM)x + (NUM)y * something
					if (node->children_[1]->children_[0]->n_type_ == NodeType::Num) {
						auto y = dynamic_cast<Number<T> *>(node->children_[1]->children_[0].get());
						// x == y
						if (x->val_ref.name == y->val_ref.name) {
							return plus_or_minus(std::move(node->children_[0]->children_[0]),
												 std::move(node->children_[1]->children_[1]), is_minus) * x->val_ref;
						}
						
					}
					
					// something * (NUM)x + something * (NUM)y
					if (node->children_[1]->children_[1]->n_type_ == NodeType::Num) {
						auto y = dynamic_cast<Number<T> *>(node->children_[1]->children_[1].get());
						// x == y
						if (x->val_ref.name == y->val_ref.name) {
							return plus_or_minus(std::move(node->children_[0]->children_[0]),
												 std::move(node->children_[1]->children_[0]), is_minus) * x->val_ref;
						}
					}
				}
			}
			
			//MULT + (NUM)y
			if (node->children_[1]->n_type_ == NodeType::Num) {
				auto y = dynamic_cast<Number<T> *>(node->children_[1].get());
				
				//(NUM)x * something + (NUM)y
				if (node->children_[0]->children_[0]->n_type_ == NodeType::Num) {
					auto x = dynamic_cast<Number<T> *>(node->children_[0]->children_[0].get());
					if (x->val_ref.name == y->val_ref.name) {
						return plus_or_minus(std::move(node->children_[0]->children_[1]),
											 std::make_unique<ConstantNode<T>>(1), is_minus) * x->val_ref;
					}
				}
				
				// something * (NUM)x + (NUM)y
				if (node->children_[0]->children_[1]->n_type_ == NodeType::Num) {
					auto x = dynamic_cast<Number<T> *>(node->children_[0]->children_[1].get());
					if (x->val_ref.name == y->val_ref.name) {
						return plus_or_minus(std::move(node->children_[0]->children_[0]),
											 std::make_unique<ConstantNode<T>>(1), is_minus) * x->val_ref;
					}
				}
			}
		}
		
		// (NUM)x + something
		if (node->children_[0]->n_type_ == NodeType::Num) {
			auto x = dynamic_cast< Number<T> *>(node->children_[0].get());
			
			// (NUM)x + MULT
			if (node->children_[1]->n_type_ == NodeType::Multiplication) {
				// (NUM)x + something * (NUM)y
				if (node->children_[1]->children_[1]->n_type_ == NodeType::Num) {
					auto y = dynamic_cast< Number<T> *>(node->children_[1]->children_[1].get());
					if (x->val_ref.name == y->val_ref.name) {
						return plus_or_minus(std::move(node->children_[1]->children_[0]),
											 std::make_unique<ConstantNode<T>>(1), is_minus) * x->val_ref;
					}
				}
				
				// (NUM)x + (NUM)y * something
				if (node->children_[1]->children_[0]->n_type_ == NodeType::Num) {
					auto y = dynamic_cast< Number<T> *>(node->children_[1]->children_[0].get());
					if (x->val_ref.name == y->val_ref.name) {
						return plus_or_minus(std::move(node->children_[1]->children_[1]),
											 std::make_unique<ConstantNode<T>>(1), is_minus) * x->val_ref;
					}
				}
			}
			
			// (NUM)x + (NUM)y
			if (node->children_[1]->n_type_ == NodeType::Num) {
				auto y = dynamic_cast< Number<T> *>(node->children_[1].get());
				if (x->val_ref.name == y->val_ref.name) {
					if (is_minus) return std::make_unique< ConstantNode<T>>(0);
					return T(2) * x->val_ref;
				}
			}
		}
	}
	return std::unique_ptr<ExpressionNode<T>>();
}

template<typename T>
void Expression<T>::factorize() {
	auto new_r = factorize_impl(root.get());
	std::cout << "expr.factorize() = " << new_r << std::endl;
	if (new_r) {
		root = std::move(new_r);
	}
	changed_graph = true;
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
	if (topo_order.empty() or changed_graph){
		std::set< ExpressionNode<T>*> visited;
		topo_order.clear();
		build_topo(root.get(), visited, topo_order);
		changed_graph = false;
	}
	
	// TODO derivations change to zero !

	root->eval();
	root->deriv_ = T(1);

	for (auto it = topo_order.rbegin(); it != topo_order.rend() ; ++it) {
//		std::stringstream ss;
//		(*it)->to_string(ss);
//		std::cout << ss.str() << std::endl;
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
