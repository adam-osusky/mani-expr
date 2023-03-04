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

//template<typename T>
//using ptr_node = std::unique_ptr<ExpressionNode<T>>;
//using ptr_node = std::shared_ptr<ExpressionNode<T>>;

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
	using ptr_node = typename ExpressionNode<T>::ptr_node;
	
	AddNode(ptr_node &&l, ptr_node &&r) : ExpressionNode<T>(NodeType::Addition, std::move(l),
															std::move(r)) {} //constructor for unique_ptr

//	AddNode(ptr_node &&l, ptr_node &&r) {
//		static_assert(std::is_same_v<std::vector<ptr_node>,
//				decltype(std::vector<ptr_node>({std::move(l), std::move(r)}))>);
//	}

//	AddNode(ptr_node<T> l, ptr_node<T> r) : ExpressionNode<T>(NodeType::Addition,
//																  {std::move(l),
//																   std::move(r)}) {}
	
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
