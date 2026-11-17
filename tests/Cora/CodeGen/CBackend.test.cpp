#include <Cora/CodeGen/CBackend.h>
#include <Cora/Frontend/Frontend.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("C backend emits compilable translation unit", "[codegen][c]")
{
    cora::frontend::Compiler compiler;
    const auto result = compiler.compile("fn main() -> u32 { return 42; }\n");
    REQUIRE(result.c_source.find("#include <stdint.h>") != std::string::npos);
    REQUIRE(result.c_source.find("int main") != std::string::npos);
    REQUIRE(result.c_source.find("return 42;") != std::string::npos);
}

TEST_CASE("C backend maps Cora types and emits structs", "[codegen][types]")
{
    cora::frontend::Compiler compiler;
    const auto result = compiler.compile(
        "struct Point { x: i32, y: f64 }\n"
        "fn main() -> i32 { return 0; }\n");
    REQUIRE(result.c_source.find("typedef struct Point") != std::string::npos);
    REQUIRE(result.c_source.find("int32_t x;") != std::string::npos);
    REQUIRE(result.c_source.find("double y;") != std::string::npos);
}

TEST_CASE("C backend preserves aggregate initializers and field access", "[codegen][aggregates]")
{
    cora::frontend::Compiler compiler;
    const auto result = compiler.compile(
        "struct Point { x: i32, y: i32 }\n"
        "fn main() -> i32 { let p: Point = Point{x: 1, y: 2}; return p.x; }\n");
    REQUIRE(result.c_source.find(".x = 1") != std::string::npos);
    REQUIRE(result.c_source.find(".y = 2") != std::string::npos);
    REQUIRE(result.c_source.find("p.x") != std::string::npos);
}

TEST_CASE("C backend erases generic parameters into valid C types", "[codegen][generics]")
{
    cora::frontend::Compiler compiler;
    const auto result = compiler.compile(
        "struct Box<T> { value: T }\n"
        "type IntBox = Box<i32>;\n"
        "fn main() -> i32 { let box: IntBox = Box{value: 7}; return box.value; }\n");
    REQUIRE(result.c_source.find("typedef struct Box") != std::string::npos);
    REQUIRE(result.c_source.find("int64_t value;") != std::string::npos);
    REQUIRE(result.c_source.find("typedef Box IntBox;") != std::string::npos);
    REQUIRE(result.c_source.find("<T>") == std::string::npos);
}
