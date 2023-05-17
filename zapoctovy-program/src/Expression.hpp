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

//TODO auto l = x+y; auto r = l * 10.0; l will be copied

template<typename T>
class Expression {
public:
	Var<T> & create_variable(T value, const std::string & name);
	auto operator[](std::string x) -> Var<T> &;
	void differentiate();
	[[nodiscard]] std::string to_string() const;
	bool factorize();
	bool simplify();
	void normalize();
	T evaluate() const;
	void set_expr(std::unique_ptr<ExpressionNode<T>> && r);
	std::unique_ptr<ExpressionNode<T>> && get_expr();
	
	std::map< std::string, Var<T>, std::less<>> variables;
	std::unique_ptr<ExpressionNode<T>> root;
private:
	void build_topo(ExpressionNode<T> * node,
					std::set<ExpressionNode<T> *> &visited,
					std::vector<ExpressionNode<T> *> &topo);
	std::vector<ExpressionNode<T> *> topo_order;
	std::unique_ptr<ExpressionNode<T>> factorize_impl(ExpressionNode<T> * node, bool & change);
	std::unique_ptr<ExpressionNode<T>> simplify_impl(ExpressionNode<T> * node, bool & change);
	std::unique_ptr<ExpressionNode<T>> plus_or_minus(std::unique_ptr<ExpressionNode<T>> && l, std::unique_ptr<ExpressionNode<T>> && r, bool is_minus);
	void zero_derivs();
	
	bool changed_graph = true;
};

template<typename T>
std::unique_ptr<ExpressionNode<T>> &&Expression<T>::get_expr() {
	if (!root) throw std::runtime_error("Trying to get uninitialized expression");
	return std::move(root);
}

template<typename T>
void Expression<T>::set_expr(std::unique_ptr<ExpressionNode<T>> && r) {
	changed_graph = true;
	root = std::move(r);
}

template<typename T>
[[maybe_unused]] void Expression<T>::zero_derivs() {
	for (auto && [name, var] : variables) {
		var.derivative = 0;
	}
}

template<typename T>
T Expression<T>::evaluate() const {
	if (!root) return 0;
	
	return root->eval();
}

template<typename T>
void Expression<T>::normalize() {
	if (!root) return;
	
	bool factorized = true;
	bool simplified = true;
	while (factorized or simplified) {
		factorized = this->factorize();
		simplified = this->simplify();
	}
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> Expression<T>::simplify_impl(ExpressionNode<T> *node, bool & change) {
	for (std::size_t i = 0; i < node->children_.size(); i++) {
		auto new_child = simplify_impl(node->children_[i].get(), change);
		if (new_child) {
			change = true;
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
		
		//something / something
		if (node->children_[1]->n_type_ == NodeType::Denominator) {
			// something / const
			if (node->children_[1]->children_[0]->n_type_ == NodeType::Const) {
				//something / 1 = something
				if (node->children_[1]->children_[0]->value_ == T(1)) {
					return std::move(node->children_[0]);
				}
				// const / const
				if (node->children_[0]->n_type_ == NodeType::Const) {
					T value = node->children_[0]->value_ / node->children_[1]->children_[0]->value_;
					return std::make_unique< ConstantNode<T>>(value);
				}
			}
		}
	}
	
	//SUM
	if (node->n_type_ == NodeType::Addition) {
		auto sub = dynamic_cast<AddNode<T> *>(node)->sub_;
		
		//a +- 0 = a
		if (node->children_[1]->n_type_ == NodeType::Const && node->children_[1]->value_ == T(0)) {
			return std::move(node->children_[0]);
		}
		// 0 +- a = +-a
		if (node->children_[0]->n_type_ == NodeType::Const && node->children_[0]->value_ == T(0)) {
			if (sub == T(-1)) return sub * std::move(node->children_[1]);
			return std::move(node->children_[1]);
		}
		//const +- const
		if (node->children_[0]->n_type_ == NodeType::Const && node->children_[1]->n_type_ == NodeType::Const) {
			T value = node->children_[0]->value_ + (sub * node->children_[1]->value_);
			return std::make_unique< ConstantNode<T>>(value);
		}
	}
	return std::unique_ptr<ExpressionNode<T>>();
}

template<typename T>
bool Expression<T>::simplify() {
	if (!root) return false;
	
	bool has_changed = false;
	auto new_r = simplify_impl(root.get(), has_changed);
//	std::cout << "expr.simplify() = " << new_r << std::endl;
	if (new_r) {
		root = std::move(new_r);
	}
	changed_graph = changed_graph or has_changed;
	return has_changed;
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
std::unique_ptr<ExpressionNode<T>>  Expression<T>::factorize_impl(ExpressionNode<T> *node, bool & change) {
	for (std::size_t i = 0; i < node->children_.size(); i++) {
		auto new_child = factorize_impl(node->children_[i].get(), change);
		if (new_child) {
			change = true;
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
bool Expression<T>::factorize() {
	if (!root) return false;
	
	bool has_changed = false;
	auto new_r = factorize_impl(root.get(), has_changed);
//	std::cout << "expr.factorize() = " << new_r << std::endl;
	if (new_r) {
		root = std::move(new_r);
	}
	changed_graph = changed_graph or has_changed;
	return has_changed;
}

template<typename T>
Var<T> & Expression<T>::create_variable(const T value, const std::string & name) {
	variables.insert_or_assign(name, Var<T>(value, name));
	return variables[name];
}

template<typename T>
auto Expression<T>::operator[](const std::string x) -> Var<T> & {
	return variables[x];
}

template<typename T>
void Expression<T>::differentiate() {
	if (!root) return;
	
	if (topo_order.empty() or changed_graph){
		std::set< ExpressionNode<T>*> visited;
		topo_order.clear();
		build_topo(root.get(), visited, topo_order);
		changed_graph = false;
	}
	
	root->eval();
	root->deriv_ = T(1);
	zero_derivs();
	
	for (auto it = topo_order.rbegin(); it != topo_order.rend() ; ++it) {
//		std::stringstream ss;
//		(*it)->to_string(ss);
//		std::cout << ss.str() << std::endl;
		(*it)->differentiate();
	}
}

template<typename T>
void Expression<T>::build_topo(ExpressionNode<T> * const node,
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
std::string Expression<T>::to_string() const {
	std::stringstream ss;
	
	root->to_string(ss);
	
	return ss.str();
}

#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
