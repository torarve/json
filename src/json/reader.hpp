#pragma once
#ifndef READER_HPP_
#define READER_HPP_

#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include "lexer.hpp"


namespace json {

class Parser {
public:
    Parser(Lexer & lexer) : lexer{lexer} {}

    value parse() {
        current = lexer.next();
        return parse_element();
    }

private:
    value parse_element() {
        using Type = Token::Type;


        switch(current.type) {
            case Type::Null: return json::value{}; break;
            case Type::Boolean: return json::value{current.booleanValue}; break;
            case Type::Integer: return json::value{current.intValue}; break;
            case Type::Float: return json::value{current.floatValue}; break;
            case Type::String: return json::value{std::move(current.stringValue)}; break;
            case Type::StartArray: return parse_array_tail(); break;
            case Type::StartObject: return parse_object_tail(); break;
            case Type::Identifier: return json::value{std::move(current.stringValue)}; break;
            default: throw std::runtime_error{"Unexpected token in stream."}; break;
        }

        throw std::runtime_error{"Unexpected end of token stream."};
    }


    value::array_t parse_array_tail() {
        using Type = Token::Type;

        value::array_t res;
        current = lexer.next();
        while(current.type!=Type::EndOfFile && current.type!=Type::EndArray) {
            res.emplace_back(parse_element());
            current = lexer.next();
            if(current.type==Type::Comma) { 
                current = lexer.next();
            }
        }

        if(current.type==Type::EndOfFile) 
            throw std::runtime_error{"Unexpected end of input. ']' not found."};

        return res;
    }


    value::object_t parse_object_tail() {
        using Type = Token::Type;
        value::object_t res;
        current = lexer.next();
        while(current.type!=Type::EndOfFile && current.type!=Type::EndObject) {
            if(current.type==Type::String || current.type==Type::Identifier) {
                auto name = std::move(current.stringValue);
                current = lexer.next(); 
                if(current.type!=Type::Colon)
                    throw std::runtime_error{"Expected colon after property name."};

                current = lexer.next(); 
                res[name] = parse_element();
                current = lexer.next();
                if(current.type==Type::Comma) {
                    current = lexer.next();
                }
            }
            else {
                throw std::runtime_error{"Unexpected token, expected string."};
            }
        }

        if(current.type!=Type::EndObject)
            throw std::runtime_error{"Unexpected input. '}' not found."};

        return res;
    }

    Token current;
    Lexer& lexer;
};



value parse(std::istream & in) {
    Lexer lexer{in};
    Parser parser{lexer};
    return parser.parse();
}

} // namespace json

#endif
