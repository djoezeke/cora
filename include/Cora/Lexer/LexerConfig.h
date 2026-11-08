#ifndef CORA_LEXER_LEXERCONFIG_H
#define CORA_LEXER_LEXERCONFIG_H

namespace cora::parse
{
    struct LexerConfig
    {
        bool enableComments = true;
        bool enableWhitespace = true;
    };

} // namespace cora::parse

#endif // CORA_LEXER_LEXERCONFIG_H
