//
// Created by adas on 2/19/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSION_HPP


#include <memory>
#include <vector>

enum NodeType {
	Num, Addition, Multiplication
};

template<typename T>
class ExpressionNode;

template<typename T>
class ExpressionNode {
public:
	using ptr_node = std::unique_ptr<ExpressionNode>;
	
	ExpressionNode() = default;
	
	ExpressionNode(NodeType n_type, T val) : n_type_(n_type), value_(val) {}

//	explicit ExpressionNode(NodeType n_type) : n_type_(n_type) {}
	template<typename... Ts>
	explicit ExpressionNode(NodeType n_type, Ts &&... args) : n_type_(n_type) {
		(children_.push_back(std::forward<Ts>(args)), ...);
	}

//	ExpressionNode(NodeType n_type, std::vector<ptr_node> &&children) : n_type_(n_type) {}
	
	virtual ~ExpressionNode() = default;
	
	virtual T eval() = 0;
	
	virtual void differentiate() = 0;

//	virtual void set_derivative(T d) = 0;

//	void simplify();
//protected:
	T value_ = 0;
	NodeType n_type_;
	T deriv_ = 0;
	std::vector<ptr_node> children_ = {};
};

template<typename T>
struct Var {
	explicit Var(T v) : value(v) {};
	
	T value;
};

template<typename T>
class Number : public ExpressionNode<T> {
public:
	explicit Number(Var<T> &v_r) : ExpressionNode<T>(NodeType::Num, v_r.value), val_ref(v_r) {}
	
	T eval() override {
		this->value_ = val_ref.value;
		return this->value_;
	}
	
	void differentiate() override {}
	
	Var<T> &val_ref;
};


template<typename T>
class AddNode : public ExpressionNode<T> {
public:
	using ptr_node = typename ExpressionNode<T>::ptr_node;
	
	AddNode(ptr_node &&l, ptr_node &&r) : ExpressionNode<T>(NodeType::Addition, std::move(l),
															std::move(r)) {} //constructor for unique_ptr
	
	T eval() override {
		this->value_ = this->children_[0]->eval() + this->children_[1]->eval();
		return this->value_;
	}
	
	void differentiate() override {
		this->children_[0]->deriv_ = this->deriv_;
		this->children_[1]->deriv_ = this->deriv_;
	}
};

template<typename T>
class MultNode : public ExpressionNode<T> {
public:
	using ptr_node = typename ExpressionNode<T>::ptr_node;
	
	MultNode(ptr_node &&l, ptr_node &&r) : ExpressionNode<T>(NodeType::Multiplication, std::move(l), std::move(r)) {}
	
	T eval() override {
		this->value_ = this->children_[0]->eval() * this->children_[1]->eval();
		return this->value_;
	}
	
	void differentiate() override {
		this->children_[0]->deriv_ = this->deriv_ * this->children_[1]->value_;
		this->children_[1]->deriv_ = this->deriv_ * this->children_[0]->value_;
	}
};

//template<typename T>
//ExpressionNode<T> operator+()


#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
