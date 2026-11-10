#ifndef CORA_LEXER_CONFIG_H
#define CORA_LEXER_CONFIG_H

namespace cora::lexer
{
    struct Config
    {
        bool enableComments = true;
        bool enableWhitespace = true;
    };

} // namespace cora::lexer

#endif // CORA_LEXER_CONFIG_H
