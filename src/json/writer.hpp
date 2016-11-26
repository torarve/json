#pragma once
#ifndef WRITER_HPP_
#define WRITER_HPP_

#include "value.hpp"

#include <sstream>

namespace json {


    // Returns string representation of bool
    constexpr const char * to_str(bool val) {
        return val ? "true" : "false";
    }


    class Writer {
    public:
        Writer(std::ostream & out) : out_{out} {}

        void write_null() { out_ << "null"; }

        void write(bool b) { out_ << (b ? "true" : "false"); }

        void write(int i) { out_ << i; }

        void write(double d) { out_ << d; }

        void write(const std::string & s) {
            out_ << '"';
            for(auto c : s) {
                write(c);
            }
            out_ << '"';
        }

        void write(const value::array_t & array) {
            out_ << "[";
            auto i = array.cbegin();
            if(i!=array.end()) {
                write(*i);
                for(++i; i!=array.end(); ++i) {
                    out_ << ",";
                    write(*i);
                }
            }

            out_ << "]";
        }

        void write(const value::object_t & object) {
            out_ << "{";
            auto iter = object.cbegin();
            if(iter!=object.end()) {
                write(iter->first);
                out_ << ":";
                write(iter->second);
                for(++iter; iter!=object.end(); ++iter) {
                    out_ << ",";
                    write(iter->first);
                    out_ << ":";
                    write(iter->second);
                }
            }
            out_ << "}";
        }

        void write(const json::value & val) {
            using Type = json::value::Type;
            switch(val.type()) {
                case Type::null: write_null(); break;
                case Type::boolean: write(val.boolean()); break;
                case Type::integer: write(val.integer()); break;
                case Type::real: write(val.real()); break;
                case Type::string: write(val.string()); break;
                case Type::array: write(val.array()); break;
                case Type::object: write(val.object()); break;
            }
        }

    private:
        void write(char c) {
            switch(c) {
                default: out_ << c; break;
                case '"': out_ << R"_(\")_"; break;
                case '\\': out_ << R"_(\\)_"; break;
                case '/': out_ << R"_(\/)_"; break;
                case '\b': out_ << R"_(\b)_"; break;
                case '\f': out_ << R"_(\f)_"; break;
                case '\n': out_ << R"_(\n)_"; break;
                case '\r': out_ << R"_(\r)_"; break;
                case '\t': out_ << R"_(\t)_"; break;
            }
        }

        std::ostream & out_;
    };


    std::ostream & operator<<(std::ostream & out, const json::value & val) {
        Writer w{out};
        w.write(val);
        return out;
    }


    // Write json value to std::string
    std::string write_to_string(const json::value & val) {
        std::stringstream str;
        str << val;
        return str.str();
    }

} // namespace json


#endif
