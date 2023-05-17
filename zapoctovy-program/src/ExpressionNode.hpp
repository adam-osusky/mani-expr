//
// Created by adas on 2/19/23.
//

#ifndef ZAPOCTOVY_PROGRAM_EXPRESSIONNODE_HPP
#define ZAPOCTOVY_PROGRAM_EXPRESSIONNODE_HPP


#include <memory>
#include <utility>
#include <vector>
#include <sstream>

enum NodeType {
	Num, Addition, Multiplication, Denominator, Const
};

template<typename T>
class ExpressionNode;

template<typename T>
class ExpressionNode {
public:
	using ptr_node = std::unique_ptr<ExpressionNode>;
	using value_t = T;
	
	ExpressionNode() = default;
	
	ExpressionNode(NodeType n_type, T val) : n_type_(n_type), value_(val) {} //const for args?

//	explicit ExpressionNode(NodeType n_type) : n_type_(n_type) {}
	template<typename... Ts>
	explicit ExpressionNode(NodeType n_type, Ts &&... args) : n_type_(n_type) {
		(children_.push_back(std::forward<Ts>(args)), ...);
	}
	
	virtual ~ExpressionNode() = default;
	
	virtual T eval() = 0;
	
	virtual void differentiate() = 0;
	virtual void to_string(std::stringstream & ss) const = 0;

	NodeType n_type_;
	T value_ = 0;
	T deriv_ = 0;
	std::vector<ptr_node> children_ = {};
};

template<typename T>
class Number;

template<typename T>
struct Var {
	using is_var = void;
	explicit Var(T v) : value(v) {};
	explicit Var(T v, std::string  var_name) : value(v), name(std::move(var_name)) {};
	
	Var() = default;
	
	T value;
	T derivative = 0;
	std::string name;
};

template<typename T>
class Number : public ExpressionNode<T> {
public:
	explicit Number(Var<T> &v_r) : ExpressionNode<T>(NodeType::Num, v_r.value), val_ref(v_r) {}
	
	T eval() override {
		this->value_ = val_ref.value;
		return this->value_;
	}
	
	void differentiate() override {
		val_ref.derivative += this->deriv_;
	}
	
	void to_string(std::stringstream & ss) const override {
		ss << val_ref.name;
	}
	
	Var<T> &val_ref;
};

template<typename T>
class ConstantNode : public ExpressionNode<T> {
public:
	explicit ConstantNode(T v) : ExpressionNode<T>(NodeType::Const, v) {}
	
	T eval() override {
		return this->value_;
	}
	
	void differentiate() override {}
	
	void to_string(std::stringstream & ss) const override {
		ss << this->value_;
	}
};


template<typename T>
class AddNode : public ExpressionNode<T> {
public:
	using ptr_node = typename ExpressionNode<T>::ptr_node;
	
	AddNode(ptr_node &&l, ptr_node &&r, bool is_subtract = false) : ExpressionNode<T>(NodeType::Addition, std::move(l),
																					  std::move(r)) {
		if (is_subtract) sub_ = (-1);
	}
	
	T eval() override {
		this->value_ = this->children_[0]->eval() + (sub_ * this->children_[1]->eval());
		return this->value_;
	}
	
	void differentiate() override {
		this->children_[0]->deriv_ = this->deriv_;
		this->children_[1]->deriv_ = (sub_ * this->deriv_);
	}
	
	void to_string(std::stringstream & ss) const override {
		ss << "(";
		this->children_[0].get()->to_string(ss);
		if (sub_ == T(1)) {
			ss << " + ";
		} else {
			ss << " - ";
		}
		this->children_[1].get()->to_string(ss);
		ss << ")";
	}

//private:
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
		this->children_[0]->deriv_ = this->deriv_ * this->children_[1]->value_;
		this->children_[1]->deriv_ = this->deriv_ * this->children_[0]->value_;
	}
	
	void to_string(std::stringstream & ss) const override {
		this->children_[0].get()->to_string(ss);
		ss << " * ";
		this->children_[1].get()->to_string(ss);
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
		this->children_[0]->deriv_ = this->deriv_ * T(-1) / (this->children_[0]->value_ * this->children_[0]->value_);
	}
	
	void to_string(std::stringstream & ss) const override {
		ss << "(1/";
		this->children_[0].get()->to_string(ss);
		ss << ")";
	}
};

template<typename T>
std::unique_ptr<ExpressionNode<T>> convert_to_node(Var<T> & var) {
	return std::make_unique<Number<T>>(var);
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> convert_to_node(T value) {
	return std::make_unique< ConstantNode<T>>(value);
}

template<typename T>
std::unique_ptr<ExpressionNode<T>> convert_to_node(std::unique_ptr<ExpressionNode<T>> && value) {
	return std::move(value);
}

//template<typename T, typename V>
//concept is_convertible_to_node = requires(T t) {
//	{convert_to_node(t)} -> std::same_as<std::unique_ptr<ExpressionNode<V>>>;
//};

template<typename T>
concept not_primitive_impl = requires { typename std::remove_cvref_t<T>::is_var; } or
						requires { typename std::remove_cvref_t<T>::ptr_node; };

template<typename T>
concept not_primitive = not_primitive_impl<T> or not_primitive_impl<typename std::remove_cvref_t<T>::element_type>;

//+ operator
auto operator+(auto && l, auto && r) -> decltype(convert_to_node(std::forward<decltype(l)>(l)))
requires not_primitive<decltype(l)> or not_primitive<decltype(r)>{
	using expr_node_t = decltype(convert_to_node(std::forward<decltype(l)>(l)))::element_type;
	using T = expr_node_t::value_t;
	auto l_p = convert_to_node(std::forward<decltype(l)>(l));
	auto r_p = convert_to_node(std::forward<decltype(r)>(r));
	auto res_p = std::make_unique<AddNode<T>>(std::move(l_p), std::move(r_p));
	return res_p;
}

//* operator
auto operator*(auto && l, auto && r) -> decltype(convert_to_node(std::forward<decltype(l)>(l)))
requires not_primitive<decltype(l)> or not_primitive<decltype(r)>{
	using expr_node_t = decltype(convert_to_node(std::forward<decltype(l)>(l)))::element_type;
	using T = expr_node_t::value_t;
	auto l_p = convert_to_node(std::forward<decltype(l)>(l));
	auto r_p = convert_to_node(std::forward<decltype(r)>(r));
	auto res_p = std::make_unique<MultNode<T>>(std::move(l_p), std::move(r_p));
	return res_p;
}

// subtraction
auto operator-(auto && l, auto && r) -> decltype(convert_to_node(std::forward<decltype(l)>(l)))
requires not_primitive<decltype(l)> or not_primitive<decltype(r)>{
	using expr_node_t = decltype(convert_to_node(std::forward<decltype(l)>(l)))::element_type;
	using T = expr_node_t::value_t;
	auto l_p = convert_to_node(std::forward<decltype(l)>(l));
	auto r_p = convert_to_node(std::forward<decltype(r)>(r));
	auto res_p = std::make_unique<AddNode<T>>(std::move(l_p), std::move(r_p), true);
	return res_p;
}
// division
auto operator/(auto && l, auto && r) -> decltype(convert_to_node(std::forward<decltype(l)>(l)))
requires not_primitive<decltype(l)> or not_primitive<decltype(r)>{
	using expr_node_t = decltype(convert_to_node(std::forward<decltype(l)>(l)))::element_type;
	using T = expr_node_t::value_t;
	auto l_p = convert_to_node(std::forward<decltype(l)>(l));
	auto r_p = convert_to_node(std::forward<decltype(r)>(r));
	auto denominator = std::make_unique<DenominatorNode<T>>(std::move(r_p));
	auto res_p = std::make_unique<MultNode<T>>(std::move(l_p), std::move(denominator));
	return res_p;
}

#endif //ZAPOCTOVY_PROGRAM_EXPRESSIONNODE_HPP
