#pragma once
#ifndef JSON_TOKEN_HPP_
#define JSON_TOKEN_HPP_

namespace json {

class Token {
public:

    enum class Type {
        Null,
        Boolean,
        Integer,
        Float,
        String,
        StartArray,
        EndArray,
        StartObject,
        EndObject,
        Comma,
        Colon,
        StartOfFile,
        EndOfFile,
        Identifier
    };

    Token() : type{Type::StartOfFile} {}

    // Create start array token.
    static inline Token startArray() {
        return Type::StartArray;
    }

    // Create end array token.
    static inline Token endArray() {
        return Type::EndArray;
    }

    // create start object oken.
    static inline Token startObject() {
        return Type::StartObject;
    }

    // Create end of object token
    static inline Token endObject() {
        return Type::EndObject;
    }

    // Create comma token
    static inline Token comma() {
        return Type::Comma;
    }

    // Create colon token.
    static inline Token colon() {
        return Type::Colon;
    }

    // Create end-of-file token
    static inline Token eof() {
        return Type::EndOfFile;
    }

    // Create null token
    static inline Token nullValue() {
        return Type::Null;
    }

    // Create floating point token with the given value.
    static inline Token floatingPoint(double val) {
        return val;
    }
    
    // Create integer token with the given value.
    static inline Token integer(int val) {
        return val;
    }

    // Create boolean token with the given value.
    static inline Token boolean(bool val) {
        return val;
    }

    // Create string token with the given value
    static inline Token string(std::string && str) {
        return {Type::String, std::move(str)};
    }

    // Create string token with the given value
    static inline Token string(const char * val) {
        return {Type::String, val};
    }

    // Create identifier token with the given name
    static inline Token identifier(std::string && name) {
        return {Type::Identifier, std::move(name)};
    }

    // Create identifier token with the given name
    static inline Token identifier(const char * val) {
        return {Type::Identifier, val};
    }

private:
    // Internal constructors
    // NOTE: Must be called through the use of the static factory
    // methods to ensure that the type and value match.
    Token(Type t) : type{t} {}
    Token(Type t, std::string && str) : type{t}, stringValue{std::move(str)} {}
    Token(Type t, const char * val) : type{t}, stringValue{val} {}
    Token(bool val) : type{Type::Boolean}, booleanValue{val} {}
    Token(int val) : type{Type::Integer}, intValue{val} {}
    Token(double val) : type{Type::Float}, floatValue{val} {}

public:
    Token(const Token & other) : type{other.type} {
        switch(type) {
            case Type::Boolean:
                booleanValue = other.booleanValue; break;
            case Type::Integer:
                intValue = other.intValue; break;
            case Type::Float:
                floatValue = other.floatValue; break;
            case Type::String:
            case Type::Identifier:
                new (&stringValue) std::string{other.stringValue}; break;
            default: break;
        }
    }

    Token(Token && other) : type{other.type} {
        switch(type) {
            case Type::Boolean: booleanValue = other.booleanValue; break;
            case Type::Integer: intValue = other.intValue; break;
            case Type::Float: floatValue = other.floatValue; break;
            case Type::String: 
            case Type::Identifier:
                new(&stringValue) std::string{std::move(other.stringValue)}; break;
            default: break;
        }
    }


    Token & operator=(Token && other) {
        using string = std::string;
        if(hasStringValue() && other.hasStringValue()) {
            stringValue = std::move(other.stringValue);
        }
        // other is string
        else if(other.hasStringValue()) {
            new (&stringValue) string{std::move(other.stringValue)};
        }
        // this is string
        else if(hasStringValue()) {
            stringValue.~string();
        }

        type = other.type;
        switch(type) {
            case Type::Boolean: booleanValue = other.booleanValue; break;
            case Type::Integer: intValue = other.intValue; break;
            case Type::Float: floatValue = other.floatValue; break;
            default: break;
        }

        return *this;
    }

    ~Token() {
        using string = std::string;
        if(hasStringValue()) {
            stringValue.~string();
        }
    }

    Type type;
    union {
        bool booleanValue;
        int intValue;
        double floatValue;
        std::string stringValue;
    };

    bool operator==(const Token & other) const {
        if(type!=other.type) return false;
        else if(type==Type::Boolean) { return booleanValue==other.booleanValue; }
        else if(type==Type::Integer) { return intValue==other.intValue; }
        else if(type==Type::Float) { return floatValue==other.floatValue; }
        else if(hasStringValue()) { return stringValue==other.stringValue; }
        return true;
    }

    bool operator!=(const Token & other) const {
        return !(*this==other);
    }

private:
    bool inline hasStringValue() const {
        return type==Type::String || type==Type::Identifier;
    }

};


/// Output type to stream
std::ostream & operator<<(std::ostream & out, const Token::Type t) {
    using Type = Token::Type;
    switch(t) {
        case Type::Null: out << "Null"; break;
        case Type::Boolean: out << "Boolean"; break;
        case Type::Integer: out << "Integer"; break;
        case Type::Float: out << "Float"; break;
        case Type::String: out << "String"; break;
        case Type::StartArray: out << "StartArray"; break;
        case Type::EndArray: out << "EndArray"; break;
        case Type::StartObject: out << "StartObject"; break;
        case Type::EndObject: out << "EndObject"; break;
        case Type::Comma: out << "Comma"; break;
        case Type::Colon: out << "Colon"; break;
        case Type::StartOfFile: out << "StartOfFile"; break;
        case Type::EndOfFile: out << "EndOfFile"; break;
        case Type::Identifier: out << "Identifier"; break;
    }

    return out;
}


/// Output token to stream
std::ostream & operator<<(std::ostream & out, const Token & t) {
    using Type = Token::Type;
    out << "Token{" << t.type ;
    switch(t.type) {
        case Type::Boolean: 
            out << ", " << t.booleanValue; break;
        case Type::Integer: 
            out << ", " << t.intValue; break;
        case Type::Float: 
            out << "," << t.floatValue; break;
        case Type::String:
        case Type::Identifier:
             out << "," << t.stringValue; break;
        default: break; 
    }

    out << "}";
    return out;
}


}

#endif
