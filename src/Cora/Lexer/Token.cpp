#include <Cora/Lexer/TokenNames.h>

namespace cora
{

    const char *token_tag_name(Token::Tag tag) noexcept
    {
        switch (tag)
        {
#define F(e, n) \
    case Token::Tag::e: return #e;
            TOKENS_MISC
            TOKENS_KEYWORDS
            TOKENS_PUNCT
#undef F

        case Token::Tag::KEYWORD_START: return "KEYWORD_START";
        case Token::Tag::KEYWORD_END: return "KEYWORD_END";
        case Token::Tag::PUNCT_START: return "PUNCT_START";
        case Token::Tag::PUNCT_END: return "PUNCT_END";
        case Token::Tag::TOKEN_COUNT: return "TOKEN_COUNT";
        }
        return "TOKEN_UNKNOWN";
    }

    const char *token_display_name(Token::Tag tag) noexcept
    {
        switch (tag)
        {
#define F(e, n) \
    case Token::Tag::e: return n;
            TOKENS_MISC
            TOKENS_KEYWORDS
            TOKENS_PUNCT
#undef F

        case Token::Tag::KEYWORD_START: return "keyword start";
        case Token::Tag::KEYWORD_END: return "keyword end";
        case Token::Tag::PUNCT_START: return "punctuation start";
        case Token::Tag::PUNCT_END: return "punctuation end";
        case Token::Tag::TOKEN_COUNT: return "token count";
        }
        return "unknown token";
    }

} // namespace cora
