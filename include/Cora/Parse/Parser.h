#ifndef CORA_PARSE_PARSER_H
#define CORA_PARSE_PARSER_H

#include <Cora/AST/AST.h>
#include <Cora/Lexer/Lexer.h>

#include <memory>
#include <string>
#include <vector>

namespace cora
{
    struct ParseDiagnostic
    {
        SourceRange range{};
        std::string message;
    };
    struct ParsedUnit
    {
        TokenBuffer tokens;
        std::shared_ptr<cora::ast::AST> ast;
        std::vector<ParseDiagnostic> diagnostics;
        [[nodiscard]] bool success() const noexcept
        { return diagnostics.empty() && ast != nullptr; }
    };
    class Parser final
    {
    public:
        [[nodiscard]] static ParsedUnit parse(TokenBuffer tokens);
    };
} // namespace cora

#endif // CORA_PARSE_PARSER_H
