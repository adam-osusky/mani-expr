//
// Created by adas on 2/19/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSION_HPP


#include <memory>
#include <vector>

enum NodeType {
	Num, Var, Addition, Mult
};

template<typename T>
class ExpressionNode;

template<typename T>
using ptr_node = std::unique_ptr<ExpressionNode<T>>;

template<typename T>
class ExpressionNode {
public:
	explicit ExpressionNode(T val, NodeType n_type) : value_(val), n_type_(n_type), deriv_(0) {}
	virtual ~ExpressionNode() = default;
	
	virtual T eval() = 0;
	
	virtual void differentiate() = 0;
	
	virtual void set_derivative(T d) = 0;

//	void simplify();
protected:
	T value_;
	NodeType n_type_;
	T deriv_;
	std::vector< ptr_node<T>> children_;
};


template<typename T>
class Number : public ExpressionNode<T> {
public:
	explicit Number(T val, NodeType n_type) : ExpressionNode<T>(val, n_type) {}
	
	T eval() override {
		return ExpressionNode<T>::value_;
	}
	
	void differentiate() override {}
	
	void set_derivative(T d) override {}
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
