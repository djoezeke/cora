#ifndef CORA_LEXER_LEXER_H
#define CORA_LEXER_LEXER_H

#include <Cora/Lexer/LexerConfig.h>
#include <Cora/Lexer/Token.h>
#include <string>

#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Lexer/Token.h>

namespace cora::parse
{

    class Lexer final
    {
        enum class State
        {
        };

    public:
        Lexer(std::string source, std::uint32_t fileId = 0,
              LexerConfig config = {});

    private:
    };
} // namespace cora

#endif // CORA_LEXER_LEXER_H
