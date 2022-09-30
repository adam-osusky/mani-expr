#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace Tutorial1{
    constexpr char separator = ';';
    void loadLine(std::vector<std::vector<std::string>> &db, std::string &line){
        std::vector<std::string> dbLine;
        int oldPosition = 0;
        for (int currentPosition = 0; currentPosition < line.size(); ++currentPosition) {
            if (line[currentPosition] == separator){
                dbLine.push_back(line.substr(oldPosition, currentPosition-oldPosition));
                oldPosition = currentPosition + 1;
            }
        }
        db.push_back(dbLine);
    }
}

int main(int argc, char * * argv) {
//    for (int i = 0; i < argc; ++i) {
//        std::cout << i << ": \"" << argv[i] << "\"" << std::endl;
//    }

    std::ifstream is(argv[1]);
    if (!is.good()) {
        std::cout << "File argument is not okay" << std::endl;
    }
    std::string x;
    std::vector<std::vector<std::string>> db;

    while (!is.eof()) {
        std::getline(is, x);
        if (is.good()) {
            //std::cout << "\"" << x << "\"" << std::endl;
            Tutorial1::loadLine(db, x);
        }
    }

    for (const auto &line : db) {
        for (const auto &item : line) {
            std::cout << "\"" << item << "\"" << Tutorial1::separator;
        }
        std::cout << std::endl;
    }

    return 0;
}

// std::string x
// std::ifstream is(file)
// std::getline(is ,x) is.good() fail eof
