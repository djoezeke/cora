#ifndef CORA_LEXER_TOKEN_H
#define CORA_LEXER_TOKEN_H

#include <cstdint>

namespace cora
{

    // Token Flags
#define TOKEN_AT_LINE_START (1u << 0)

    struct Token
    {
        enum class Tag : std::uint8_t
        {
            IDENTIFIER,
            NUMBER,
            FLOAT,
            STRING,
            CHAR,
            IMPORT,
            AS,
            TYPE,
            LET,
            MUT,
            CONST,
            FUNC,
            STRUCT,
            ENUM,
            INTERFACE,
            IMPL,
            COMPTIME,
            ASYNC,
            AWAIT,
            IF,
            ELSE,
            WHILE,
            FOR,
            IN,
            MATCH,
            CASE,
            DEFAULT,
            RETURN,
            PUB,
            SELF,
            TRUE,
            FALSE,
            _NULL,
            PLUS,
            MINUS,
            STAR,
            SLASH,
            PERCENT,
            EQUALS,
            EQUALS_EQUALS,
            NOT_EQUALS,
            LESS_THAN,
            LESS_THAN_EQUALS,
            GREATER_THAN,
            GREATER_THAN_EQUALS,
            AMPERSAND,
            BANG,
            DOT,
            COLON,
            COLON_COLON,
            ARROW,
            QUESTION_MARK,
            LPAREN,
            RPAREN,
            LBRACE,
            RBRACE,
            LBRACKET,
            RBRACKET,
            COMMA,
            SEMICOLON,
            _EOF,
            ERROR,

            COUNT
        };

        struct Loc
        {
            std::size_t start;
            std::size_t end;
        };

        // This
        Tag tag;
        Loc loc;
        uint8_t flag;

        union
        {
            uint64_t int_val;
            double float_val;
            uint32_t interned_id;
            uint32_t error_id;
        };
    };

} // namespace cora

using Token = cora::Token;

#endif // CORA_LEXER_TOKEN_H
