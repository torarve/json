#pragma once
#ifndef JSON_LEXER_HPP_
#define JSON_LEXER_HPP_

#include <json/token.hpp>
#include <json/value.hpp>


namespace json {

/// JSON lexer
class Lexer {
public:
    Lexer(std::istream & input) : iter{input}, current{*iter} {}

    Token next() {
        skip_whitespace();
        if(eof()) return Token{Token::eof()};
        switch(current) {
            case '[': get(); return Token::startArray(); break;
            case ']': get(); return Token::endArray(); break;
            case '{': get(); return Token::startObject(); break;
            case '}': get(); return Token::endObject(); break;
            case ',': get(); return Token::comma(); break;
            case ':': get(); return Token::colon(); break;

            case '0': case '1': case '2': case '3': case '4': 
            case '5': case '6': case '7': case '8': case '9':
            case '-':
                return try_parse_number(); break;

            case '"': return try_parse_string(); break;
            default: return try_parse_identifier(); break;
        }

        throw std::runtime_error{"Unexpected character in input."};
    };

private:
    // Check if end reached
    inline bool eof() const {
        constexpr std::istreambuf_iterator<char> end_of_file;
        return iter==end_of_file;
    }
    // Move to the next character
    inline char get() { ++iter; return current=*iter; }
    // skips any initial white space at the currrent location
    inline void skip_whitespace() {
        constexpr std::istreambuf_iterator<char> end_of_file;
        for(; iter!=end_of_file && std::isspace(current); current=*(++iter))
            ;
    }

    Token try_parse_number() {
        int sign = get_sign();
        int intValue = parse_int();
        double decimalPart = try_parse_decimal_part();
        int e = try_parse_exp();

        if(decimalPart==0.0 && e>=0) {
            // TODO: Check that result fits within integer value
            if(e==0) return Token::integer(sign*intValue);
            else return Token::integer(static_cast<int>(sign*intValue*std::pow(10, e)));
        }
        else {
            return Token::floatingPoint(sign*(intValue+decimalPart)*std::pow(10,e));
        }
    }

    double try_parse_decimal_part() {
        double decimal = 0.0;
        if(current=='.') {
            double weight = 0.1;
            while(std::isdigit(get())) {
                decimal += (current-'0')*weight;
                weight /= 10.0;                 
            }         
        }

        return decimal;
    }

    int parse_int() {
        int res = current - '0';
        while(std::isdigit(get())) {
            res = res*10 + (current-'0');
        }

        return res;
    }

    int get_sign() {
        char c = current;
        int sign = 1;
        if(c=='-'||c=='+') {
            sign = (c=='-' ? -1 : 1);
            get();
        }

        return sign;
    }

    int try_parse_exp() {
        int e = 0;
        if(current=='e' || current=='E') {
            get();
            int sign = get_sign();

            if(!std::isdigit(current)) {
                throw std::runtime_error("Invalid number.");
            }

            e = sign*parse_int();
        }

        return e;
    }

    Token try_parse_string() {
        res.clear();
        while(get()!='"') {
            if(current=='\\') {
                switch(get()) {
                    case 'b': res += '\b'; break;
                    case 'f': res += '\f'; break;
                    case 'n': res += '\n'; break;
                    case 'r': res += '\r'; break;
                    case 't': res += '\t'; break;
                    case 'u': res += read_utf16(); break;
                    default: res += current; break;
                }
            }
            else {
                res += current;
            }
        };

        get();
        if(res.size()==0) return Token::string(std::string{});
        return Token::string(std::move(res));
    }

    inline bool isValidIdentifierChar(const char c) const {
        return !(isspace(c)
            || c=='{' || c=='}'
            || c=='[' || c==']'
            || c=='-' || c=='+'
            || c=='"' || c==':' || c==';'
            || c==std::char_traits<char>::eof());
    }

    Token try_parse_identifier() {
        res.clear();
        res += current;
        while(isValidIdentifierChar(get())) {
            res += current;
        }

        if(res=="null") {
            return {Token::nullValue()};
        }
        else if(res=="true") {
            return Token::boolean(true);
        }
        else if(res=="false") {
            return Token::boolean(false);
        }

        return Token::identifier(std::move(res));
    }

    char utf16[4];

    const char * read_utf16() {
        int res = get_hex() << 12;
        res |= get_hex() << 8;
        res |= get_hex() << 4;
        res |= get_hex();

        if(res<0x80) {
            utf16[0] = static_cast<char>(res);
            utf16[1] = '\x00';
        }
        else if(res<0x0800) {
            utf16[0] = static_cast<char>(0xc0 | res >> 6);
            utf16[1] = static_cast<char>(0x80 | (res & 0x3f));
            utf16[2] = 0;
        }
        else if(res<0x10000) {
            utf16[0] = static_cast<char>(0xe0 | (res>>12));
            utf16[1] = static_cast<char>(0x80 | ((res>>6)&0x3f));
            utf16[2] = static_cast<char>(0x80 | (res&0x3f));
            utf16[3] = '\x00';
        }

        return utf16;
    }


    int get_hex() {
        if(std::isdigit(get())) {
            return current - '0';
        }
        if(std::isxdigit(current)) {
            switch(current) {
                case 'a': case 'A': return 10; break;
                case 'b': case 'B': return 11; break;
                case 'c': case 'C': return 12; break;
                case 'd': case 'D': return 13; break;
                case 'e': case 'E': return 14; break;
                case 'f': case 'F': return 15; break;
            }
        }

        throw std::runtime_error{"Invalid hex characeter found in unicode point."};
    }


    std::istreambuf_iterator<std::istream::char_type> iter;
    char current;
    std::string res;
}; 

}

#endif
