#include <Cora/Lexer/Token.h>
#include <Cora/Lexer/TokenNames.h>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

TEST_CASE("Token is a compact trivially copyable DOD value", "[lexer][token]")
{
    static_assert(std::is_trivially_copyable_v<Token>);
    static_assert(std::is_standard_layout_v<Token>);
    static_assert(offsetof(Token, tag) == 0);
    static_assert(offsetof(Token, loc) % alignof(std::size_t) == 0);
    static_assert(offsetof(Token, flag) ==
                  offsetof(Token, loc) + 2 * sizeof(std::size_t));
    static_assert(offsetof(Token, int_val) >=
                  offsetof(Token, flag) + sizeof(std::uint8_t));
    static_assert(sizeof(Token) % alignof(Token) == 0);
}

TEST_CASE("Token tags have stable names and classifications", "[lexer][token]")
{
    REQUIRE(std::string_view(token_tag_name(Token::Tag::T_EOF)) == "T_EOF");
    REQUIRE(std::string_view(token_display_name(Token::Tag::T_EOF)) ==
            "end of file");
    REQUIRE(std::string_view(token_display_name(Token::Tag::T_LPAREN)) == "(");

    REQUIRE(is_keyword_tag(Token::Tag::T_IMPORT));
    REQUIRE(is_keyword_tag(Token::Tag::T_NULLLIT));
    REQUIRE_FALSE(is_keyword_tag(Token::Tag::T_IDENT));

    REQUIRE(is_operator_tag(Token::Tag::T_PLUS));
    REQUIRE(is_operator_tag(Token::Tag::T_FATARROW));
    REQUIRE_FALSE(is_operator_tag(Token::Tag::T_IF));

    REQUIRE(is_literal_tag(Token::Tag::T_INTLIT));
    REQUIRE(is_literal_tag(Token::Tag::T_STRLIT));
    REQUIRE_FALSE(is_literal_tag(Token::Tag::T_IDENT));
}
