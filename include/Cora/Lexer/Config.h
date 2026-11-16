#ifndef CORA_LEXER_CONFIG_H
#define CORA_LEXER_CONFIG_H

namespace cora::lexer
{
    struct Config
    {
        /// Recognize and skip `//` and `/* ... */` comments.
        bool enableComments = true;
        /// Recognize and skip whitespace (required by every normal lexer).
        bool enableWhitespace = true;
        /// Emit `-Wcomment` when a block comment spans more than one line.
        bool warnOnMultilineComments = false;
        /// Diagnose malformed `_` separators inside numeric literals.
        bool checkDigitSeparators = true;
    };

} // namespace cora::lexer

#endif // CORA_LEXER_CONFIG_H
