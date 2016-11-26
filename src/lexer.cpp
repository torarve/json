// lexer.cpp
// Run lexer on input file

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
    for(auto t=lexer.next(); t.type!=json::Token::Type::EndOfFile; t=lexer.next()) {
        //std::cout << t << std::endl;
    }

    return EXIT_SUCCESS;
}
