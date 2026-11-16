#ifndef CORA_LEXER_TOKENNAMES_H
#define CORA_LEXER_TOKENNAMES_H

#include <Cora/Lexer/Token.h>

namespace cora
{

    /// Symbolic tag name, e.g. `T_IMPORT`. Never returns null.
    [[nodiscard]] const char *token_tag_name(Token::Tag tag) noexcept;

    /// Human readable spelling, e.g. `import` or `'('`. Never returns null.
    [[nodiscard]] const char *token_display_name(Token::Tag tag) noexcept;

    [[nodiscard]] constexpr bool is_keyword_tag(Token::Tag tag) noexcept
    {
        return tag > Token::Tag::KEYWORD_START && tag < Token::Tag::KEYWORD_END;
    }

    [[nodiscard]] constexpr bool is_operator_tag(Token::Tag tag) noexcept
    {
        return tag > Token::Tag::PUNCT_START && tag < Token::Tag::PUNCT_END;
    }

    [[nodiscard]] constexpr bool is_literal_tag(Token::Tag tag) noexcept
    {
        return tag == Token::Tag::T_INTLIT || tag == Token::Tag::T_FLTLIT ||
               tag == Token::Tag::T_STRLIT || tag == Token::Tag::T_CHARLIT;
    }

} // namespace cora

#endif // CORA_LEXER_TOKENNAMES_H
