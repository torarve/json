// parser.cpp
// Run parser on input file

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <json/reader.hpp>

int main(int argc, char* argv[]) {
    if(argc<2) {
        std::cout << "Please specify input file." << std::endl;
        return -2;
    }

    std::ifstream input{argv[1]};
    json::Lexer lexer{input};
    json::Parser parser{lexer};
    auto val = parser.parse();

    return EXIT_SUCCESS;
}
