//
// Created by adas on 12/6/22.
//

#ifndef MACRO_PROCESSOR_PARAMETERS_MACRO_PROCESSOR_HPP
#define MACRO_PROCESSOR_PARAMETERS_MACRO_PROCESSOR_HPP

#include <unordered_map>
#include <istream>
#include <vector>
#include <variant>
#include <iostream>
#include <memory>
#include <deque>

namespace macro_processor {
	
	using num_t = std::size_t;
	
	using Token = std::variant< std::string, num_t>;
	
	struct Definition {
//		Definition(std::size_t n_par, std::vector<std::string> & vec) : num_param(n_par), rhs(std::move(vec)) {}
		
		num_t num_param = 0;
		std::deque< Token> rhs;
	};
	
	class Processor {
	public:
		explicit Processor(std::istream * isp) : isp_(isp) { eat(); }
		void parseLine();
		
		void processLine();
		
		num_t horner();
		
		Token popFrontLine();
		
	private:
		void eat();
		
		std::unordered_map<std::string, Definition> defs_;
		std::istream * isp_;
		std::deque< Token> line_;
		char next_ = 0;
		bool was_defining_ = false;
//		num_t pos_ = -1;
	};
	
	void processFile(std::istream & is);
	
}

#endif //MACRO_PROCESSOR_PARAMETERS_MACRO_PROCESSOR_HPP
