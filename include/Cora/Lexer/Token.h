#ifndef CORA_LEXER_TOKEN_H
#define CORA_LEXER_TOKEN_H

#include <cstdint>

#define TOKENS_MISC               \
    F(T_EOF, "end of file")       \
    F(T_IDENT, "identifier")      \
    F(T_INTLIT, "int literal")    \
    F(T_FLTLIT, "float literal")  \
    F(T_STRLIT, "string literal") \
    F(T_CHARLIT, "char literal")

#define TOKENS_PUNCT      \
    F(T_LPAREN, "(")      \
    F(T_RPAREN, ")")      \
    F(T_LBRACKET, "[")    \
    F(T_RBRACKET, "]")    \
    F(T_LCURLY, "{")      \
    F(T_RCURLY, "}")      \
    F(T_COMMA, ",")       \
    F(T_SEMI, ";")        \
    F(T_COLON, ":")       \
    F(T_QUESTION, "?")    \
    F(T_COLONCOLON, "::") \
    F(T_DOT, ".")         \
    F(T_DOTDOT, "..")     \
    F(T_DOTASSIGN, ".=")  \
    F(T_DOTEQ, ".==")     \
    F(T_DOTNEQ, ".!=")    \
    F(T_PLUS, "+")        \
    F(T_MINUS, "-")       \
    F(T_MUL, "*")         \
    F(T_DIV, "/")         \
    F(T_MOD, "%")         \
    F(T_PLUSEQ, "+=")     \
    F(T_MINUSEQ, "-=")    \
    F(T_MULEQ, "*=")      \
    F(T_DIVEQ, "/=")      \
    F(T_MODEQ, "%=")      \
    F(T_ANDEQ, "&=")      \
    F(T_OREQ, "|=")       \
    F(T_XOREQ, "^=")      \
    F(T_INC, "++")        \
    F(T_DEC, "--")        \
    F(T_ASSIGN, "=")      \
    F(T_EQ, "==")         \
    F(T_NEQ, "!=")        \
    F(T_LT, "<")          \
    F(T_GT, ">")          \
    F(T_LTEQ, "<=")       \
    F(T_GTEQ, ">=")       \
    F(T_SHL, "<<")        \
    F(T_SHR, ">>")        \
    F(T_SHLEQ, "<<=")     \
    F(T_SHREQ, ">>=")     \
    F(T_NOT, "!")         \
    F(T_BITNOT, "~")      \
    F(T_BITAND, "&")      \
    F(T_BITOR, "|")       \
    F(T_XOR, "^")         \
    F(T_ANDAND, "&&")     \
    F(T_OROR, "||")       \
    F(T_ARROW, "->")      \
    F(T_FATARROW, "=>")

#define TOKENS_KEYWORDS       \
    F(T_IMPORT, "import")     \
    F(T_STRUCT, "struct")     \
    F(T_ENUM, "enum")         \
    F(T_TYPE, "type")         \
    F(T_FN, "fn")             \
    F(T_EXPORT, "export")     \
    F(T_LET, "let")           \
    F(T_VAR, "var")           \
    F(T_EXTERN, "extern")     \
    F(T_CONST, "const")       \
    F(T_IF, "if")             \
    F(T_ELSE, "else")         \
    F(T_WHILE, "while")       \
    F(T_FOR, "for")           \
    F(T_IN, "in")             \
    F(T_MATCH, "match")       \
    F(T_DEFER, "defer")       \
    F(T_RETURN, "return")     \
    F(T_BREAK, "break")       \
    F(T_CONTINUE, "continue") \
    F(T_TRUE, "true")         \
    F(T_FALSE, "false")       \
    F(T_NULLLIT, "null")      \
    F(T_TBOOL, "bool")

namespace cora
{

    // Token Flags
#define TOKEN_AT_LINE_START (1u << 0)

    struct Token
    {
        enum class Tag : std::uint8_t
        {
#define F(e, n) e,
            // clang-format off

            TOKENS_MISC

            KEYWORD_START,
            TOKENS_KEYWORDS
            KEYWORD_END,

            PUNCT_START,
            TOKENS_PUNCT
            PUNCT_END,

            TOKEN_COUNT,

            // clang-format on
#undef F
        };

        struct Loc
        {
            std::size_t start;
            std::size_t end;
        };

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
