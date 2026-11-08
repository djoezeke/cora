
#include <Cora/Lexer/Token.h>

#include <Catch2/catch_test_macros.hpp>

int main(int argc, char const *argv[])
{

    static_assert(sizeof(Token) == 16, "Token size is not 16 bytes");
    static_assert(alignof(Token) == 8, "Token size is not 16 bytes");
    static_assert(offsetof(Token, type) == 0, "Token::type is not at offset 0");
    static_assert(offsetof(Token, flags) == 1, "Token::flags is not at offset 1");
    static_assert(offsetof(Token, lenght) == 2, "Token::lenght is not at offset 2");
    static_assert(offsetof(Token, offset) == 4, "Token::offset is not at offset 4");
    static_assert(offsetof(Token, int_val) == 8, "Token::int_val is not at offset 8");
    return 0;
}
