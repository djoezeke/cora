#ifndef CORA_PARSE_PARSER_H
#define CORA_PARSE_PARSER_H

#include <Cora/AST/AST.h>
#include <Cora/Lexer/Lexer.h>
#include <Cora/Parser/Config.h>

namespace cora::parser
{

    using namespace cora::core;
    using namespace cora::diag;

    class Parser final
    {
    public:
        Parser(SourceManager &sm, DiagnosticEngine &de, Config config = {});

        ast::AST parse();

    private:
        const Token &peek() const;
        Token advance();

    private:
        Token current;
        Config config;
        lexer::Lexer lexer;
        SourceManager &m_SM;
        DiagnosticEngine &m_DE;
    };

} // namespace cora::parser

#endif // CORA_PARSE_PARSER_H
