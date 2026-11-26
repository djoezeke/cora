#include <Cora/Sema/Types.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("primitive types are interned and integer types are compatible",
          "[sema][types]")
{
    cora::sema::Types types;
    const auto integer = types.primitive(cora::sema::Type::Tag::Int);
    const auto unsigned_integer = types.primitive(cora::sema::Type::Tag::UInt);

    REQUIRE(types.primitive(cora::sema::Type::Tag::Int) == integer);
    REQUIRE(types.compatible(integer, unsigned_integer));
    REQUIRE(types.describe(integer) == "i64");
}

TEST_CASE("pointer types are interned by pointee type", "[sema][types]")
{
    cora::sema::Types types;
    const auto integer = types.primitive(cora::sema::Type::Tag::Int);

    REQUIRE(types.pointer(integer) == types.pointer(integer));
    REQUIRE(types.describe(types.pointer(integer)) == "*i64");
}
