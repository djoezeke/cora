#include <Cora/Frontend/Frontend.h>
#include <Cora/Sema/ComptimeEvaluator.h>
#include <Cora/Sema/Types.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("semantic pipeline accepts typed integer function", "[sema]")
{
    cora::frontend::Compiler compiler;
    const auto result = compiler.compile("fn main() -> u32 { return 1 + 2; }\n");
    REQUIRE(result.status == cora::frontend::ParseStatus::Success);
    REQUIRE(result.semantic_errors.empty());
    REQUIRE(result.air.verified);
}

TEST_CASE("type interning preserves pointer identity", "[sema][types]")
{
    cora::sema::Types types;
    const auto integer = types.primitive(cora::sema::Type::Tag::Int);
    REQUIRE(types.pointer(integer) == types.pointer(integer));
    REQUIRE(types.compatible(integer, types.primitive(cora::sema::Type::Tag::UInt)));
}

TEST_CASE("uppercase type variables resolve as generic semantic types", "[sema][generics]")
{
    cora::frontend::Compiler compiler;
    const auto result = compiler.compile(
        "struct Box<T> { value: T }\n"
        "fn main() -> i32 { return 0; }\n");
    REQUIRE(result.status == cora::frontend::ParseStatus::Success);
    REQUIRE(result.semantic_errors.empty());
}
