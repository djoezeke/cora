#include <Cora/Lexer/Lexer.h>
#include <cctype>

namespace cora::lexer
{
    namespace
    {
        // ============================================================================
        // Lexer — hand-written DFA
        // ============================================================================
        static bool isDigit(char c) { return c >= '0' && c <= '9'; }
        static bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
        static bool isAlphaNum(char c) { return isAlpha(c) || isDigit(c); }
        static bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f'; }

    } // namespace

} // namespace cora::lexer
