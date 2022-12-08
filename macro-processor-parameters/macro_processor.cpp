//
// Created by adas on 12/6/22.
//

#include "macro_processor.hpp"

namespace macro_processor {
	
	void Processor::parseLine() {
		line_.clear();
		std::string new_word;
		bool is_defining = false;
		Definition def;
		num_t max = 0;
		
		while (next_ != '\n' and next_ != 0) {
			if (std::isspace(next_) and !new_word.empty()) {
//				line_.push_back(std::make_unique<Token>(new_word));
				line_.emplace_back(new_word);
				new_word = "";
			} else if (std::isdigit(next_)) {
//				line_.push_back(std::make_unique<Token>(horner()));
				auto num = horner();
				if (num > max) { max = num; }
				line_.emplace_back(num);
				continue;
			} else if (std::isalpha(next_)) {
				new_word += next_;
			} else if (next_ == '=') {
				is_defining = true;
			}
			eat();
		}
		if (!new_word.empty()) { line_.emplace_back(new_word); }
		eat();
		
		if (is_defining and !line_.empty()) {
			def.num_param = max;
//			for (auto it = line_.begin() + 1; it != line_.end(); it++) {
////				auto &x = *it;
//				def.rhs.push_back(std::move(*it));
//			}
			std::deque<Token> v(std::make_move_iterator(line_.begin() + 1),
											   std::make_move_iterator(line_.end()));
			def.rhs = std::move(v);
			defs_.insert_or_assign(std::get<std::string>(line_[0]), std::move(def));
		}
		was_defining_ = is_defining;
	}
	
	num_t Processor::horner() {
		num_t number = 0;
		while (std::isdigit(next_)) {
			number = 10 * number + (num_t) next_ - '0';
			eat();
		}
		return number;
	}
	
	Token Processor::popFrontLine() { //make it generic for some deque DO USING = DEQUE
		Token x;
		x = line_.front();
		line_.pop_front();
		return x;
	}
	
	void Processor::processLine() {
		std::vector< Token> processed;
		while (!line_.empty()) {
			auto current_word = popFrontLine();
			
			if (std::holds_alternative<std::string>(current_word)) {
				auto & word = std::get<std::string>(current_word);
				auto it = defs_.find(word);
				if (it != defs_.end()) {
					auto & def = it->second;
				}
			}
			
			
		}
	}
	
	void Processor::eat() {
		char c;
		isp_->get(c);
		if (isp_->eof()) {
			next_ = 0;
			return;
		}
		if (isp_->fail()) throw std::runtime_error("Failed reading from the file");
		next_ = c;
//		pos_++;
	}
	
	void processFile(std::istream &is) {
		struct Visitor {
			std::string operator()(std::string & x) { return std::move(x); }
		};
		
		std::cout << "HURAAAAA" << std::endl;
		macro_processor::Processor p(&is);
		
		Token x = Token("adam");
		std::unordered_map<std::string, Definition> defs;
		Definition def;
		defs.insert_or_assign("adam", def);
//		auto visited = std::visit(Visitor(), x);
		auto & visited = std::get<std::string>(x);
		std::get<std::string>(x) = "kokot";
		auto y = defs.find( visited );
		
		p.parseLine();
		p.parseLine();
		p.parseLine();
		p.processLine();
		
		
		p.parseLine();
		p.parseLine();
		p.parseLine();
		p.parseLine();
		int b = 0;
	}
}
