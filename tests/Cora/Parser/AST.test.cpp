#include <Cora/AST/AST.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("AST placeholder", "[ast]")
{
    cora::ast::AST ast;
    REQUIRE(ast.nodes.empty());
}
