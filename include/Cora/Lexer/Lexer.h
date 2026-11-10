#ifndef CORA_LEXER_LEXER_H
#define CORA_LEXER_LEXER_H

#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Lexer/Config.h>
#include <Cora/Lexer/Token.h>

#include <string>

namespace cora::lexer
{

    using namespace cora::core;
    using namespace cora::diag;

    class Lexer final
    {

        enum class State
        {
        };

    public:
        Lexer(std::uint32_t fileId, SourceManager &sm, DiagnosticEngine &de, Config config = {});

        // DFA states:
        //   Start -> (ws|comment)* -> Identifier | Number | Operator
        //   Identifier: [A-Za-z_][A-Za-z0-9_]*
        //   Number:     [0-9]+ ( (NumberSuffix: [A-Za-z_][A-Za-z0-9_]*) | ε )
        //   LineComment:  '//' .* ( '\n' | EOF )
        //   BlockComment: '/*' .* '*/'   (with BlockCommentStar substate)
        //   Operator: length-1 or length-2 operator
        //
        // Error tolerant: bad input emits a diagnostic and is skipped / marked Invalid.
        Token next();

    private:
        char peek(uint32_t la = 0) const;
        char advance();

    private:
        Config config;
        SourceManager &m_SM;
        DiagnosticEngine &m_DE;
    };

} // namespace cora::lexer

#endif // CORA_LEXER_LEXER_H
