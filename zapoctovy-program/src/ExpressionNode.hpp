//
// Created by adas on 2/19/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSIONNODE_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSIONNODE_HPP


#include <memory>
#include <vector>

enum NodeType {
	Num, Addition, Multiplication, Denominator
};

template<typename T>
class ExpressionNode;

template<typename T>
class ExpressionNode {
public:
	using ptr_node = std::unique_ptr<ExpressionNode>;
	
	ExpressionNode() = default;
	
	ExpressionNode(NodeType n_type, T val) : n_type_(n_type), value_(val) {} // not collision with variadic temp??

//	explicit ExpressionNode(NodeType n_type) : n_type_(n_type) {}
	template<typename... Ts>
	explicit ExpressionNode(NodeType n_type, Ts &&... args) : n_type_(n_type) {
		(children_.push_back(std::forward<Ts>(args)), ...);
	}
	
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
	T derivative = 0;
};

template<typename T>
class Number : public ExpressionNode<T> {
public:
	explicit Number(Var<T> &v_r) : ExpressionNode<T>(NodeType::Num, v_r.value),
								   val_ref(v_r) {} //why const v_r doesnt work?
	
	T eval() override {
		this->value_ = val_ref.value;
		return this->value_;
	}
	
	void differentiate() override {
		val_ref.derivative += this->deriv_;
	}
	
	Var<T> &val_ref;
};


template<typename T>
class AddNode : public ExpressionNode<T> {
public:
	using ptr_node = typename ExpressionNode<T>::ptr_node;
	
	AddNode(ptr_node &&l, ptr_node &&r, bool is_subtract = false) : ExpressionNode<T>(NodeType::Addition, std::move(l),
																					   std::move(r)) {
		if (is_subtract) sub_ = -1;
	}
	
	T eval() override {
		this->value_ = this->children_[0]->eval() + (sub_ * this->children_[1]->eval());
		return this->value_;
	}
	
	void differentiate() override {
		this->children_[0]->deriv_ += this->deriv_;
		this->children_[1]->deriv_ += (sub_ * this->deriv_);
	}

private:
	T sub_ = 1;
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
		this->children_[0]->deriv_ += this->deriv_ * this->children_[1]->value_;
		this->children_[1]->deriv_ += this->deriv_ * this->children_[0]->value_;
	}
};

// 1/f
template<typename T>
class DenominatorNode : public ExpressionNode<T> {
public:
	using ptr_node = typename ExpressionNode<T>::ptr_node;
	
	explicit DenominatorNode(ptr_node &&child) : ExpressionNode<T>(NodeType::Denominator, std::move(child)) {}
	
	T eval() override {
		this->value_ = T(1) / this->children_[0]->eval();
		return this->value_;
	}
	
	void differentiate() override {
		this->children_[0]->deriv_ += this->deriv_ * T(-1) / (this->children_[0]->value_ * this->children_[0]->value_);
	}
};


//+ operator
template<typename T>
std::unique_ptr<ExpressionNode<T>> operator+(Var<T> &l, Var<T> &r) {
	auto l_p = std::make_unique<Number<T>>(l);
	auto r_p = std::make_unique<Number<T>>(r);
	auto res_p = std::make_unique<AddNode<T>>(std::move(l_p), std::move(r_p));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> operator+(std::unique_ptr<ExpressionNode<T>> &&l, Var<T> &r) {
	auto r_p = std::make_unique<Number<T>>(r);
	auto res_p = std::make_unique<AddNode<T>>(std::move(l), std::move(r_p));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> operator+(Var<T> &l, std::unique_ptr<ExpressionNode<T>> &&r) {
	auto l_p = std::make_unique<Number<T>>(l);
	auto res_p = std::make_unique<AddNode<T>>(std::move(l_p), std::move(r));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>>
operator+(std::unique_ptr<ExpressionNode<T>> &&l, std::unique_ptr<ExpressionNode<T>> &&r) {
	auto res_p = std::make_unique<AddNode<T>>(std::move(l), std::move(r));
	return res_p;
}


//* operator
template<typename T>
std::unique_ptr<ExpressionNode<T>> operator*(Var<T> &l, Var<T> &r) {
	auto l_p = std::make_unique<Number<T>>(l);
	auto r_p = std::make_unique<Number<T>>(r);
	auto res_p = std::make_unique<MultNode<T>>(std::move(l_p), std::move(r_p));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> operator*(std::unique_ptr<ExpressionNode<T>> &&l, Var<T> &r) {
	auto r_p = std::make_unique<Number<T>>(r);
	auto res_p = std::make_unique<MultNode<T>>(std::move(l), std::move(r_p));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> operator*(Var<T> &l, std::unique_ptr<ExpressionNode<T>> &&r) {
	auto l_p = std::make_unique<Number<T>>(l);
	auto res_p = std::make_unique<MultNode<T>>(std::move(l_p), std::move(r));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>>
operator*(std::unique_ptr<ExpressionNode<T>> &&l, std::unique_ptr<ExpressionNode<T>> &&r) {
	auto res_p = std::make_unique<MultNode<T>>(std::move(l), std::move(r));
	return res_p;
}


// subtraction
template<typename T>
std::unique_ptr<ExpressionNode<T>> operator-(Var<T> &l, Var<T> &r) {
	auto l_p = std::make_unique<Number<T>>(l);
	auto r_p = std::make_unique<Number<T>>(r);
	auto res_p = std::make_unique<AddNode<T>>(std::move(l_p), std::move(r_p), true);
	return res_p;
}
template<typename T>
std::unique_ptr<ExpressionNode<T>> operator-(std::unique_ptr<ExpressionNode<T>> &&l, Var<T> &r) {
	auto r_p = std::make_unique<Number<T>>(r);
	auto res_p = std::make_unique<AddNode<T>>(std::move(l), std::move(r_p), true);
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> operator-(Var<T> &l, std::unique_ptr<ExpressionNode<T>> &&r) {
	auto l_p = std::make_unique<Number<T>>(l);
	auto res_p = std::make_unique<AddNode<T>>(std::move(l_p), std::move(r), true);
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>>
operator-(std::unique_ptr<ExpressionNode<T>> &&l, std::unique_ptr<ExpressionNode<T>> &&r) {
	auto res_p = std::make_unique<AddNode<T>>(std::move(l), std::move(r), true);
	return res_p;
}

// division
template<typename T>
std::unique_ptr<ExpressionNode<T>> operator/(Var<T> &l, Var<T> &r) {
	auto l_p = std::make_unique<Number<T>>(l);
	auto r_p = std::make_unique<Number<T>>(r);
	
	auto denominator = std::make_unique<DenominatorNode<T>>(std::move(r_p));
	auto res_p = std::make_unique<MultNode<T>>(std::move(l_p), std::move(denominator));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> operator/(std::unique_ptr<ExpressionNode<T>> &&l, Var<T> &r) {
	auto r_p = std::make_unique<Number<T>>(r);
	
	auto denominator = std::make_unique<DenominatorNode<T>>(std::move(r_p));
	auto res_p = std::make_unique<MultNode<T>>(std::move(l), std::move(denominator));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> operator/(Var<T> &l, std::unique_ptr<ExpressionNode<T>> &&r) {
	auto l_p = std::make_unique<Number<T>>(l);
	
	auto denominator = std::make_unique<DenominatorNode<T>>(std::move(r));
	auto res_p = std::make_unique<MultNode<T>>(std::move(l_p), std::move(denominator));
	return res_p;
}

template<typename T>
std::unique_ptr<ExpressionNode<T>>
operator/(std::unique_ptr<ExpressionNode<T>> &&l, std::unique_ptr<ExpressionNode<T>> &&r) {
	auto denominator = std::make_unique<DenominatorNode<T>>(std::move(r));
	
	auto res_p = std::make_unique<MultNode<T>>(std::move(l), std::move(denominator));
	return res_p;
}


#endif //ZAPOCTOVY_PROGRAM_EXPRESSIONNODE_HPP
