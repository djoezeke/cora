#ifndef CORA_LEXER_TOKEN_H
#define CORA_LEXER_TOKEN_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace cora
{
    struct SourcePosition
    {
        std::uint32_t offset = 0;
        std::uint32_t line = 1;
        std::uint32_t column = 1;
    };
    struct SourceRange
    {
        SourcePosition begin{};
        SourcePosition end{};
    };

    struct Token
    {
        enum class Type : std::uint8_t
        {
            IDENTIFIER, NUMBER, FLOAT, STRING, CHAR,
            IMPORT, AS, TYPE, LET, MUT, CONST, FUNC, STRUCT, ENUM, INTERFACE, IMPL,
            COMPTIME, ASYNC, AWAIT, IF, ELSE, WHILE, FOR, IN, MATCH, CASE, DEFAULT,
            RETURN, PUB, SELF, TRUE, FALSE, _NULL,
            PLUS, MINUS, STAR, SLASH, PERCENT, EQUALS, EQUALS_EQUALS, NOT_EQUALS,
            LESS_THAN, LESS_THAN_EQUALS, GREATER_THAN, GREATER_THAN_EQUALS,
            AMPERSAND, BANG, DOT, COLON, COLON_COLON, ARROW, QUESTION_MARK,
            LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, COMMA, SEMICOLON,
            _EOF, ERROR,
        };
        Type type = Type::_EOF;
        SourceRange range{};
        std::string text;
        std::int64_t integerValue = 0;
    };

    struct TokenBuffer
    {
        std::string source;
        std::vector<Token> tokens;
        std::size_t significantCount = 0;
        std::vector<std::string> errors;
    };

    [[nodiscard]] const char *tokenKindName(Token::Type type) noexcept;
} // namespace cora

using Token = cora::Token;

#endif // CORA_LEXER_TOKEN_H
