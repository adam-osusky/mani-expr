#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace Tutorial1 {
    constexpr char separator = ',';

    void loadLine(std::vector<std::vector<std::string>> &db, std::string &line) {
        std::vector<std::string> dbLine;
        std::size_t oldPosition = 0;
        for (std::size_t currentPosition = 0; currentPosition < line.size(); ++currentPosition) {
            if (line[currentPosition] == separator) {
                dbLine.push_back(line.substr(oldPosition, currentPosition - oldPosition));
                oldPosition = currentPosition + 1;
            }
        }
        db.push_back(dbLine);
    }
}

int main(int argc, char * * argv) {
    for (int i = 0; i < argc; ++i) {
        std::cout << i << ": \"" << argv[i] << "\"" << std::endl;
    }

    if (argc < 2) {
        std::cout << "Missing file name" << std::endl;
        return 8;
    }

    std::ifstream is(argv[1]);
    if (!is.good()) {
        std::cout << "File argument is not okay" << std::endl;
        return 8;
    }

    std::string x;
    std::vector<std::vector<std::string>> db;

    for(;;) {
        std::getline(is, x);
        if (is.good()) {
            //std::cout << "\"" << x << "\"" << std::endl;
            Tutorial1::loadLine(db, x);
        }
        if (is.fail()) {
            if (is.eof())
                break;
            std::cout << "Cannot read from the file" << std::endl;
            return 8;
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
// emplace_back std::ptrdiff_t signed size t