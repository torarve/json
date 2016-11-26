// json::value tests
#include <json/value.hpp>
#include "catch.hpp"

TEST_CASE("json::value construction")
{
    SECTION("Default construction")
    {
        json::value val;
        REQUIRE(val.is_null());
        REQUIRE(!val.is_int());
        REQUIRE(!val.is_real());
        REQUIRE(!val.is_boolean());
        // string
        // array
        // object
    }

    SECTION("Null value")
    {
        json::value val{ nullptr };
        REQUIRE(val.is_null());
        REQUIRE(!val.is_int());
        REQUIRE(!val.is_real());
        REQUIRE(!val.is_boolean());
    }

    SECTION("Integer value") 
    {
        json::value val{ 10 };
        REQUIRE(!val.is_null());
        REQUIRE(val.is_int());
        REQUIRE(!val.is_real());
        REQUIRE(!val.is_boolean());
    }

    SECTION("Double value")
    {
        json::value val{ 3.14 };
        REQUIRE(!val.is_null());
        REQUIRE(!val.is_int());
        REQUIRE(val.is_real());
        REQUIRE(!val.is_boolean());
    }

    SECTION("Boolean value")
    {
        json::value val{ true };
        REQUIRE(!val.is_null());
        REQUIRE(!val.is_int());
        REQUIRE(!val.is_real());
        REQUIRE(val.is_boolean());
    }
}


TEST_CASE("json::value integer")
{
    SECTION("Constructor", "[integer,constructor]")
    {
        json::value val{ 10 };
        REQUIRE(val.is_int());
        REQUIRE(val.integer()==10);
    }

    SECTION("Assignment", "[integer,assignment]")
    {
        json::value val;
        REQUIRE(!val.is_int());
        val = 123;
        REQUIRE(val.is_int());
        REQUIRE(val.integer()==123);
    }

}

TEST_CASE("json double value")
{
    SECTION("Constructor", "[double,constructor]")
    {
        json::value val{ 3.14 };
        REQUIRE(val.is_real());
        REQUIRE(val.real()==3.14);
    }

    SECTION("Assignment", "[double,assignment]")
    {
        json::value val;
        REQUIRE(!val.is_real());
        val = 3.14;
        REQUIRE(val.is_real());
        REQUIRE(val.real()==3.14);
    }
}


TEST_CASE("json boolean value")
{
    SECTION("Constructor", "[bool,constructor]")
    {
        json::value val{true};
        REQUIRE(val.is_boolean());
        REQUIRE(val.boolean()==true);
    }

    SECTION("Assignment", "[bool,assignment]")
    {
        json::value val;
        REQUIRE(!val.is_boolean());
        val = true;
        REQUIRE(val.is_boolean());
        REQUIRE(val.boolean()==true);
    }
}

TEST_CASE("json string value")
{
    SECTION("Constructor", "[string,constructor]")
    {
        SECTION("Construct from string literal", "[string, constructor,string-literal]")
        {
            auto strLiteral = "string value";
            json::value val{strLiteral};
            REQUIRE(val.type()==json::value::Type::string);
            REQUIRE(val.is_string());
            REQUIRE(val.string()==strLiteral);
        }


        SECTION("Construct from string", "[string,constructor,std-string]")
        {
            std::string strVal{ "string value" };
            json::value val{strVal};
            REQUIRE(val.is_string());
            REQUIRE(val.string()==strVal);
        }
    }

    SECTION("Assignment", "[string,assignment]")
    {
        SECTION("Assign string literal", "[string,assignment,string-literal]")
        {
            json::value val;
            REQUIRE(!val.is_string());
            val = "string literal";
            REQUIRE(val.type()==json::value::Type::string);
            REQUIRE(val.is_string());
            REQUIRE(val.string()=="string literal");
        }

        SECTION("Assign std::string value", "[string,assignment,std-string]")
        {
            json::value val;
            REQUIRE(!val.is_string());
            val = std::string{"string value"};
            REQUIRE(val.type()==json::value::Type::string);
            REQUIRE(val.is_string());
            REQUIRE(val.string()=="string value");
        }
    }
}

TEST_CASE("json array")
{
    SECTION("Construction", "[array,constructor]")
    {
        std::vector<json::value> array{ 10, "string", 3.14, nullptr };
        json::value val{ array };
        REQUIRE(val.is_array());
        //REQUIRE(val.array==)
    }

    SECTION("Assignment", "[array,assignment]")
    {
        json::value val;
        REQUIRE(!val.is_array());
        val = json::value::array_t{ 10, "string", 3.14, nullptr };
        REQUIRE(val.is_array());
    }

    SECTION("Array in array")
    {
        json::value val{ json::value::array_t{} };
        REQUIRE(val.is_array());
        val = json::value::array_t{ val };

        auto entry = val.array()[0];
        REQUIRE(entry.is_array());
    }
}


TEST_CASE("Json object")
{
    SECTION("Constrcutor", "[object,constructor]")
    {
        json::value val{ json::value::object_t{} };
        REQUIRE(val.is_object());
    }

    SECTION("Copy assignent", "[object,assignent]")
    {
        json::value val;
        REQUIRE(!val.is_object());
        val = json::value::object_t{ {"name", "value"} };
        REQUIRE(val.is_object());
        auto name = val.object()["name"];
        REQUIRE(name.is_string());
        REQUIRE(name.string()=="value");
    }
}
