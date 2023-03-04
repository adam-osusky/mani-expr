//
// Created by adas on 2/19/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSION_HPP


#include <memory>
#include <vector>

enum NodeType {
	Num, Var, Addition, Multiplication
};

template<typename T>
class ExpressionNode;

template<typename T>
//using ptr_node = std::unique_ptr<ExpressionNode<T>>;
using ptr_node = std::shared_ptr<ExpressionNode<T>>;

template<typename T>
class ExpressionNode {
public:
	ExpressionNode(NodeType n_type, T val) : value_(val), n_type_(n_type), deriv_(0) {}
	
	ExpressionNode(NodeType n_type, std::vector<ptr_node<T>> &&children) : n_type_(n_type),
																		   children_(std::move(children)), value_(0),
																		   deriv_(0) {}
	
	virtual ~ExpressionNode() = default;
	
	virtual T eval() = 0;
	
	virtual void differentiate() = 0;

//	virtual void set_derivative(T d) = 0;

//	void simplify();
//protected:
	T value_;
	NodeType n_type_;
	T deriv_;
	std::vector<ptr_node<T>> children_;
};


template<typename T>
class Number : public ExpressionNode<T> {
public:
	explicit Number(T val) : ExpressionNode<T>(NodeType::Num, val) {}
	
	T eval() override {
		return this->value_;
	}
	
	void differentiate() override {}
};

template<typename T>
class AddNode : public ExpressionNode<T> {
public:
//	AddNode(ptr_node<T> &&l, ptr_node<T> &&r) : ExpressionNode<T>(NodeType::Addition, {std::move(l), std::move(r)} ) {} //constructor for unique_ptr
	AddNode(ptr_node<int> l, ptr_node<int> r) : ExpressionNode<T>(NodeType::Addition,
																  {std::move(l),
																   std::move(r)}) {}
	
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
	MultNode(ptr_node<int> l, ptr_node<int> r) : ExpressionNode<T>(NodeType::Multiplication,
																   {std::move(l), std::move(r)}) {}
	
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
//class Add : public ExpressionNode<T> {
//public:
//	Add(T val, ptr_node<T> &&l, ptr_node<T> &&r) : n_type_(NodeType::Addition), value_(val),
//												   left_(std::move(l)), right_(std::move(r)) { deriv_ = 0; }
//
//	T eval() override {
//		value_ = left_->eval() + right_->eval();
//		return value_;
//	}
//
//	void differentiate() override {
//		left_->set_derivative(deriv_);
//		right_->set_derivative(deriv_);
//	}
//
//	void set_derivative(T d) override { deriv_ = d; }
//
//private:
//	T value_;
//	T deriv_;
//	NodeType n_type_;
//	std::unique_ptr<ExpressionNode<T>> left_;
//	std::unique_ptr<ExpressionNode<T>> right_;
//};

#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
