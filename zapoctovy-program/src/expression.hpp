//
// Created by adas on 2/19/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSION_HPP


#include <memory>

enum NodeType {
	Num, Var, Addition, Mult
};

template<typename T>
class ExpressionNode {
public:
//	ExpressionNode() = default;
	virtual ~ExpressionNode() = default;
	
	virtual T eval() = 0;
	
	virtual void differentiate() = 0;
	
	virtual void set_derivative(T d) = 0;
	
//	void simplify();
private:
//	std::set<> prev_ children?
};


template<typename T>
class Number : public ExpressionNode<T> {
public:
	explicit Number(T val) : n_type_(NodeType::Num), value_(val) {deriv_ = 0;}
	
	T eval() override {
		return value_;
	}
	
	void differentiate() override {}
	
	void set_derivative(T d) override {deriv_ = d;}
	
private:
	T value_;
	T deriv_;
	NodeType n_type_;
};

template<typename T>
using ptr_node = std::unique_ptr<ExpressionNode<T>>;

template<typename T>
class Add : public ExpressionNode<T> {
public:
	Add(T val, ptr_node<T> &&l, ptr_node<T> &&r) : n_type_(NodeType::Addition), value_(val),
															left_(std::move(l)), right_(std::move(r)) {deriv_ = 0;}
	T eval() override {
		value_ = left_->eval() + right_->eval();
		return value_;
	}
	
	void differentiate() override {
		left_->set_derivative(deriv_);
		right_->set_derivative(deriv_);
	}
	
	void set_derivative(T d) override {deriv_ = d;}
	
private:
	T value_;
	T deriv_;
	NodeType n_type_;
	std::unique_ptr<ExpressionNode<T>> left_;
	std::unique_ptr<ExpressionNode<T>> right_;
};

#endif //ZAPOCTOVY_PROGRAM_EXPRESSION_HPP
