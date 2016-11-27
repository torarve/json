#pragma once
#ifndef VALUE_HPP_
#define VALUE_HPP_

#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace json {

    class value {
    public:
        enum class Type {
            null,
            integer,
            real,
            boolean,
            string,
            array,
            object
        };

        using string_t = std::string;
        using array_t = std::vector<json::value>;
        using object_t = std::map<string_t, json::value>;


        value()
        : type_{Type::null}, nullValue_{nullptr} {}

        value(const value & other)
        : type_{other.type_} {
            switch(type_) {
                case Type::null: break;
                case Type::integer: intValue_ = other.intValue_; break;
                case Type::real: realValue_ = other.realValue_; break;
                case Type::boolean: boolValue_ = other.boolValue_; break;
                case Type::string: new (&strValue_) string_t{ other.strValue_ }; break;
                case Type::array: new (&arrValue_) array_t{ other.arrValue_.begin(), other.arrValue_.end() }; break;
                case Type::object: new (&objValue_) object_t{ other.objValue_ }; break;
            }
        }

        value(value && other) 
        :type_{other.type_} {
            switch(type_) {
                case Type::null: break;
                case Type::integer: intValue_ = other.intValue_; break;
                case Type::real: realValue_ = other.realValue_; break;
                case Type::boolean: boolValue_ = other.boolValue_; break;
                case Type::string: new (&strValue_) std::string{ std::move(other.strValue_) }; break;
                case Type::array: new (&arrValue_) array_t{ std::move(other.arrValue_) }; break;
                case Type::object: new (&objValue_) object_t{ std::move(other.objValue_) }; break;
            }
        }

        
        value(decltype(nullptr) val)
        : type_{Type::null}, nullValue_{nullptr} {}


        value(int val)
        : type_{Type::integer}, intValue_{val} {}


        value(double val)
        : type_(Type::real), realValue_{val} {}


        value(bool val)
        : type_{Type::boolean}, boolValue_{val} {}


        value(const char * val)
        : type_{Type::string}, strValue_{val} {}


        value(const std::string & val)
        : type_{Type::string}, strValue_{val} {}


        value(std::string && val) 
        : type_{Type::string}, strValue_{std::move(val)} {}


        value(const array_t & val)
        : type_{Type::array}, arrValue_{val.begin(), val.end() } {}


        value(array_t && val)
        : type_{Type::array}, arrValue_(std::move(val)) {}


        value(const object_t & val)
        : type_{Type::object}, objValue_{val} {}


        value(object_t && val) 
        : type_{Type::object}, objValue_{std::move(val)} {}


        value & operator=(const value & val) {
            switch(val.type_) {
                case Type::null: operator=(nullptr); break;
                case Type::integer: operator=(val.intValue_); break;
                case Type::real: operator=(val.realValue_); break;
                case Type::boolean: operator=(val.boolValue_); break;
                case Type::string: operator=(val.strValue_); break;
                case Type::array: operator=(val.arrValue_); break;
                case Type::object: operator=(val.objValue_); break;
            }

            return *this;
        }

        value & operator=(value && val) {
            switch(val.type_) {
                case Type::null: operator=(nullptr); break;
                case Type::integer: operator=(val.intValue_); break;
                case Type::real: operator=(val.realValue_); break;
                case Type::boolean: operator=(val.boolValue_); break;
                case Type::string: operator=(std::move(val.strValue_)); break;
                case Type::array: operator=(std::move(val.arrValue_)); break;
                case Type::object: operator=(std::move(val.objValue_)); break;
            }

            return *this;
        }


        value & operator=(int val) {
            clear();
            type_ = Type::integer;
            intValue_ = val;
            return *this;
        }


        value & operator=(double val) {
            clear();
            type_ = Type::real;
            realValue_ = val;
            return *this;
        }


        value & operator=(bool val) {
            clear();
            type_ = Type::boolean;
            boolValue_ = val;
            return *this;
        }


        value & operator=(const char * val) {
            if(type_==Type::string) {
                strValue_ = val;
            } 
            else {
                clear();
                new (&strValue_) std::string{val};
                type_=Type::string;
            }

            return *this;
        }


        value & operator=(std::string val) {
            if(type_==Type::string) {
                strValue_ = std::move(val);
            }
            else {
                clear();
                new (&strValue_) std::string{std::move(val)};
                type_ = Type::string;
            }

            return *this;
        }


        value & operator=(array_t && val) {
            if(type_==Type::array) {
                arrValue_ = std::move(val);
            }
            else {
                clear();
                new (&arrValue_) array_t(std::move(val));
                type_ = Type::array;
            }

            return *this;
        }


        value & operator=(object_t && val) {
            if(type_==Type::object) {
                objValue_ = std::move(val);
            }
            else {
                clear();
                new (&objValue_) object_t{std::move(val)};
                type_ = Type::object;
            }

            return *this;
        }

        // Destruction
        ~value() {
            clear();
        }

        // Type
        Type type() const { return type_; }

        // Check type
        bool is_null() const { return type_==Type::null; }
        bool is_int() const { return type_==Type::integer; }
        bool is_real() const { return type_==Type::real; }
        bool is_boolean() const { return type_==Type::boolean; }
        bool is_string() const { return type_==Type::string; }
        bool is_array() const { return type_==Type::array; }
        bool is_object() const { return type_==Type::object; }

        // Unchecked accessors
        int integer() const { return intValue_; }
        double real() const { return realValue_; }
        bool boolean() const { return boolValue_; }
        std::string string() const { return strValue_; }
        array_t array() const { return arrValue_; }
        object_t object() const { return objValue_; }

    private:

        Type type_;
        union {
            decltype(nullptr) nullValue_;
            int intValue_;
            double realValue_;
            bool boolValue_;
            std::string strValue_;
            array_t arrValue_;
            object_t objValue_;
        };

        void clear() {
            switch(type_) {
                case Type::string: strValue_.~string_t(); break;
                case Type::array: arrValue_.~array_t(); break;
                case Type::object: objValue_.~object_t(); break;
                default: break;
            }
        }
    };


    inline std::ostream & operator <<(std::ostream & out, const value::Type type) {
        switch(type) {
            case value::Type::null: out << "null"; break;
            case value::Type::integer: out << "integer"; break;
            case value::Type::real: out << "real"; break;
            case value::Type::boolean: out << "boolean"; break;
            case value::Type::string: out << "string"; break;
            case value::Type::array: out << "array"; break;
            case value::Type::object: out << "object"; break;
        }

        return out;
    }


    inline bool operator==(const value & lhs, const value & rhs) {
        if(lhs.type()!=rhs.type()) return false;
        switch(lhs.type()) {
            case value::Type::null: return true;
            case value::Type::integer: return lhs.integer()==rhs.integer(); break;
            case value::Type::real: return lhs.real()==rhs.real(); break;
            case value::Type::boolean: return lhs.boolean()==rhs.boolean(); break;
            case value::Type::string: return lhs.string()==rhs.string(); break;
            // TODO: Compare arrays
            case value::Type::array: return false; break;
            // TODO: Compare objects
            case value::Type::object: return false; break;
        }

        return true;
    }

} // namespace json

#endif
