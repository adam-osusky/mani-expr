//#include <iostream>
#include <fstream>
#include "macro_processor.hpp"

int main(int argc, char **argv) {
	try {
		if (argc < 2) {
			throw std::runtime_error("Missing arguments");
		}
		
		std::string filename = argv[1];
//		const auto low = (evaluator::val_t)std::stol(argv[2]);
//		const auto high = (evaluator::val_t)std::stol(argv[3]);
		
		// read the file:
		{
			std::ifstream is(filename);
			
			if (!is.good()) {
				throw std::runtime_error("Cannot open file \"" + filename + "\"");
			}
			
			macro_processor::processFile(is);

//			auto res = evaluator::evaluate(is, low, high);

//			std::cout << "min=" << res.minimum << " " << "max=" << res.maximum << std::endl;
		}
		
		return 0;
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown exception" << std::endl;
	}
	return 8;
}
//include <iostream>

//
//using Token = std::variant<std::string, std::size_t>;
//
//struct Definition {
//	std::size_t npar;
//	std::vector<Token> rhs;
//};
//
//int main() {
//	Token e((std::size_t)69);
//
//	struct Functor {
//		void operator()(const std::string & v) {std::cout << v << std::endl;}
//		void operator()(std::size_t v) {
//			std::cout << v << std::endl;}
//	};
//
//	Definition def;
//	def.rhs.emplace_back("adam");
//	def.rhs.emplace_back((std::size_t)2);
//	def.rhs.emplace_back("je");
//	def.rhs.emplace_back((std::size_t)3);
//	def.rhs.emplace_back("super");
//	def.rhs.emplace_back((std::size_t)1);
//	def.npar = 3;
//
//	std::vector<Token> line;
////      line.emplace_back()
//
//	std::visit(Functor(), e);
////      std::cout << std::get<1>(e) << std::endl;
//	return 0;
//}