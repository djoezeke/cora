#ifndef CORA_PARSER_CONFIG_H
#define CORA_PARSER_CONFIG_H

#include <Cora/Lexer/Config.h>

#include <cstddef>

namespace cora::parser
{
    struct Config
    {
        /// Recover from syntax errors and keep parsing instead of stopping at
        /// the first diagnostic.
        bool errorRecovery = true;

        /// Forward syntax diagnostics to the DiagnosticEngine. The AST always
        /// records them in `AST::errors` regardless of this setting.
        bool emitDiagnostics = true;

        /// Reuse unchanged top-level declarations when a source update only
        /// touches part of the file.
        bool reuseIncremental = true;

        /// Accept expression statements and `name := value` at file scope.
        bool allowTopLevelExpressions = true;

        /// Stop recording new parser errors after this many. 0 means unlimited.
        std::size_t maxErrors = 100;

        /// Lexer configuration used for the parser-owned token stream.
        lexer::Config lexer{};
    };

} // namespace cora::parser

#endif // CORA_PARSER_CONFIG_H
