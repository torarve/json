#include "catch.hpp"

#include "json/writer.hpp"


TEST_CASE("Json writer")
{
    SECTION("Writes 'null' for null value") 
    {
        REQUIRE(json::write_to_string(nullptr)=="null");
    }

    SECTION("Boolean values")
    {
        SECTION("Writes 'true' for true values")
        {
            REQUIRE(json::write_to_string(true)=="true");
        }

        SECTION("Writes 'false' for false values")
        {
            REQUIRE(json::write_to_string(false)=="false");
        }
    }

    SECTION("Numeric values") 
    {
        SECTION("Writes integer values")
        {
            REQUIRE(json::write_to_string(1234)=="1234");
        }

        SECTION("Writes real values")
        {
            REQUIRE(json::write_to_string(123.456)=="123.456");
        }
    }

    SECTION("Strings") {

        SECTION("String values are written with double-quotes")
        {
            REQUIRE(json::write_to_string("string value")=="\"string value\"");
        }

        SECTION("Special characters escaped")
        {
            REQUIRE(json::write_to_string("\"")=="\"\\\"\"");
            REQUIRE(json::write_to_string("\\")==R"_("\\")_");
            REQUIRE(json::write_to_string("/")==R"_("\/")_");
            REQUIRE(json::write_to_string("\b")==R"_("\b")_");
            REQUIRE(json::write_to_string("\f")==R"_("\f")_");
            REQUIRE(json::write_to_string("\n")==R"_("\n")_");
            REQUIRE(json::write_to_string("\r")==R"_("\r")_");
            REQUIRE(json::write_to_string("\t")==R"_("\t")_");
        }

    }

    SECTION("Each element of array is inside brackets")
    {
        json::value array = json::value::array_t{nullptr, 1, true};
        REQUIRE(json::write_to_string(array)=="[null,1,true]");
    }

    SECTION("Writes each named member") {
        json::value object = json::value::object_t{ {"id", 1}, {"name", "Name"} };
        REQUIRE(json::write_to_string(object)=="{\"id\":1,\"name\":\"Name\"}");
    }
}
