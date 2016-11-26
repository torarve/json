#include "catch.hpp"

#include <iostream>
#include <sstream>

#include <json/reader.hpp>


using Type = json::Token::Type;
using Token = json::Token;

// Parse input string and return tokens found 
std::vector<json::Token> lex(std::string input) {
    std::stringstream stream{input};
    json::Lexer lexer{stream};

    std::vector<json::Token> res;
    for(auto t=lexer.next(); t.type!=Type::EndOfFile; t=lexer.next()) {
        res.push_back(t);
    }

    return res;
}


// Initial wrapper
struct that {
    that(std::vector<Token> tokens) : tokens_{std::move(tokens)} {}

    bool equals(std::vector<Token> other) const {
        auto lhs = tokens_.begin();
        auto rhs = other.begin();
        for(; lhs!=tokens_.end() && rhs!=other.end(); ++lhs, ++rhs) {
            if(*lhs!=*rhs) {
                std::stringstream msg;
                msg << "At position " << std::distance(tokens_.begin(), lhs) << ": "
                    << "expected " << *rhs << ", was " << *lhs << ".";
                throw std::runtime_error{msg.str()};
            }
        } 

        return lhs==tokens_.end() && rhs==other.end();
    }
private:
    std::vector<Token> tokens_;
};


TEST_CASE("Lexer") {

    SECTION("Returns eof token at end of stream")
    {
        std::stringstream input{""};
        json::Lexer lexer{input};
        REQUIRE(lexer.next()==Token::eof());
    }

    REQUIRE(that(lex("null")).equals({{Token::nullValue()}}));
    REQUIRE(that(lex("true")).equals({Token::boolean(true)}));
    REQUIRE(that(lex("false")).equals({Token::boolean(false)}));
    REQUIRE(that(lex("[]")).equals({Token::startArray(), Token::endArray()}));
    REQUIRE(that(lex("{}")).equals({Token::startObject(), Token::endObject()}));
    REQUIRE(that(lex(",")).equals({Token::comma()}));
    REQUIRE(that(lex(":")).equals({Token::colon()}));

    // Positive integers
    REQUIRE(that(lex("0")).equals({Token::integer(0)}));
    REQUIRE(that(lex("123")).equals({Token::integer(123)}));
    REQUIRE(that(lex("12e1")).equals({Token::integer(120)}));
    REQUIRE(that(lex("12e+1")).equals({Token::integer(120)}));
    REQUIRE(that(lex("23E2")).equals({Token::integer(2300)}));
    REQUIRE(that(lex("34E+3")).equals({Token::integer(34000)}));

    // Negative integers
    REQUIRE(that(lex("-1")).equals({Token::integer(-1)}));
    REQUIRE(that(lex("-1e1")).equals({Token::integer(-10)}));

    // Floats
    REQUIRE(that(lex("1.2")).equals({Token::floatingPoint(1.2)}));
    REQUIRE(that(lex("-1.2")).equals({Token::floatingPoint(-1.2)}));
    REQUIRE(that(lex("1.2e1")).equals({Token::floatingPoint(12.0)}));
    REQUIRE(that(lex("0.01e+1")).equals({Token::floatingPoint(0.1)}));
    REQUIRE(that(lex("0.1")).equals({Token::floatingPoint(0.1)}));
    REQUIRE(that(lex("1e-1")).equals({Token::floatingPoint(0.1)}));
    REQUIRE(that(lex("1E-1")).equals({Token::floatingPoint(0.1)}));

    // Invalid numbers
    REQUIRE_THROWS(lex("34e"));
    //REQUIRE_THROWS(lex("02"));

    // Strings
    REQUIRE(that(lex(R"_("simple string")_")).equals({Token::string("simple string")}));
    REQUIRE(that(lex("\"\\\"\"")).equals({Token::string("\"")}));
    REQUIRE(that(lex("\"\\/\"")).equals({Token::string("/")}));
    REQUIRE(that(lex("\"\\\\\"")).equals({Token::string("\\")}));
    REQUIRE(that(lex("\"\\b\"")).equals({Token::string("\b")}));
    REQUIRE(that(lex("\"\\f\"")).equals({Token::string("\f")}));
    REQUIRE(that(lex("\"\\n\"")).equals({Token::string("\n")}));
    REQUIRE(that(lex("\"\\r\"")).equals({Token::string("\r")}));
    REQUIRE(that(lex("\"\\t\"")).equals({Token::string("\t")}));
    REQUIRE(that(lex("\"\\u0020\"")).equals({Token::string("\u0020")}));
    REQUIRE(that(lex("\"\\u03B8\"")).equals({Token::string("\u03b8")}));
    REQUIRE(that(lex("\"\\u1234\"")).equals({Token::string("\u1234")}));
    // TODO: Handle surrogate pairs

    // spaces and lack of
    REQUIRE(that(lex(" [ ] ")).equals({Token::startArray(), Token::endArray()}));
    REQUIRE(that(lex("123[")).equals({Token::integer(123), Token::startArray()}));
    REQUIRE(that(lex("\"string\"[]")).equals({Token::string("string"), Token::startArray(), Token::endArray()}));

    REQUIRE(that(lex("abc")).equals({Token::identifier("abc")}));
    REQUIRE(that(lex("number ")).equals({Token::identifier("number")}));
    REQUIRE(that(lex("trues")).equals({Token::identifier("trues")}));
    REQUIRE(that(lex("falsed")).equals({Token::identifier("falsed")}));

    SECTION("Error handling") 
    {
        // TODO: Check that unknown token causes exception to be thrown
        // TODO: Check that integers starting with 0 raise error; fex 0123
    }

}


json::value parse(std::string val) {
    std::stringstream input{val};
    return json::parse(input);
}


TEST_CASE("Parser") {

    SECTION("simple types") {
        SECTION("null value") {
            REQUIRE(parse("null").is_null());            
            REQUIRE(parse("false")==false);
            REQUIRE(parse("true")==true);
            REQUIRE(parse("123")==123);
            REQUIRE(parse("234.56")==234.56);
            REQUIRE(parse("\"string value\"")=="string value");
        }
    }

    SECTION("Arrays")
    {
        SECTION("Empty array") 
        {
            REQUIRE(parse("[]").is_array());
        }

        SECTION("Array containing single element") 
        {
            auto val = parse("[123]");
            REQUIRE(val.is_array());
            auto array = val.array();
            REQUIRE(array.size()==1);
            REQUIRE(array[0]==123);
        }

        SECTION("Array in array") 
        {
            auto val = parse("[ [\"name\"], null]");
            REQUIRE(val.is_array());
            auto array = val.array();
            REQUIRE(array[0].is_array());
        }

        REQUIRE_THROWS(parse("["));
    }


    SECTION("Objects") 
    {
        SECTION("Empty object")
        {
            REQUIRE(parse("{}").is_object());
        }

        SECTION("Object with single property") 
        {
            auto val = parse("{\"name\": 123}");
            REQUIRE(val.is_object());
            auto obj = val.object();
            REQUIRE(obj["name"]==123);
        }

        SECTION("Property name as identifer")
        {
            auto val = parse("{value: 123}");
            REQUIRE(val.is_object());
            auto obj = val.object();
            REQUIRE(obj["value"]==123);
        }

        SECTION("Object with multiple objects") {
            auto val = parse("{\"array\": [], \"integer\": 1234 }");
            REQUIRE(val.is_object());
            auto obj = val.object();
            REQUIRE(obj["array"].is_array());
            REQUIRE(obj["integer"].is_int());
        }

        REQUIRE_THROWS(parse("{"));
        REQUIRE_THROWS(parse("{false"));
        REQUIRE_THROWS(parse("{false}"));
        REQUIRE_THROWS(parse("{\"name\", \"value\"}"));
    }

}
