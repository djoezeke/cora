#include <Cora/Lexer/Lexer.h>
#include <Cora/Lexer/TokenNames.h>
#include <Cora/Diag/DiagnosticCodes.h>

#include <algorithm>
#include <charconv>
#include <limits>
#include <string>
#include <system_error>

namespace cora::lexer
{
    namespace
    {
        struct Utf8Decoded
        {
            std::uint32_t code_point = 0;
            std::uint8_t length = 1;
            bool valid = false;
        };

        [[nodiscard]] bool is_ascii_digit(char c) noexcept
        {
            return c >= '0' && c <= '9';
        }

        [[nodiscard]] bool is_ascii_alpha(char c) noexcept
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
        }

        [[nodiscard]] bool is_ascii_alnum(char c) noexcept
        {
            return is_ascii_alpha(c) || is_ascii_digit(c);
        }

        [[nodiscard]] bool is_ascii_space(char c) noexcept
        {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
        }

        [[nodiscard]] int digit_value(char c) noexcept
        {
            if (c >= '0' && c <= '9')
            {
                return c - '0';
            }
            if (c >= 'a' && c <= 'f')
            {
                return c - 'a' + 10;
            }
            if (c >= 'A' && c <= 'F')
            {
                return c - 'A' + 10;
            }
            return -1;
        }

        [[nodiscard]] bool is_unicode_space(std::uint32_t code_point) noexcept
        {
            return code_point == 0x0085 || code_point == 0x00A0 ||
                   code_point == 0x1680 ||
                   (code_point >= 0x2000 && code_point <= 0x200A) ||
                   code_point == 0x2028 || code_point == 0x2029 ||
                   code_point == 0x202F || code_point == 0x205F ||
                   code_point == 0x3000 || code_point == 0xFEFF;
        }

        [[nodiscard]] Utf8Decoded decode_utf8(std::string_view text,
                                              std::size_t offset) noexcept
        {
            if (offset >= text.size())
            {
                return {};
            }

            const auto lead = static_cast<unsigned char>(text[offset]);
            if (lead < 0x80)
            {
                return {lead, 1, true};
            }

            std::uint8_t length = 0;
            std::uint32_t code_point = 0;
            if ((lead & 0xE0) == 0xC0)
            {
                length = 2;
                code_point = lead & 0x1F;
            }
            else if ((lead & 0xF0) == 0xE0)
            {
                length = 3;
                code_point = lead & 0x0F;
            }
            else if ((lead & 0xF8) == 0xF0)
            {
                length = 4;
                code_point = lead & 0x07;
            }
            else
            {
                return {0, 1, false};
            }

            if (offset + length > text.size())
            {
                return {0, 1, false};
            }

            for (std::uint8_t index = 1; index < length; ++index)
            {
                const auto byte = static_cast<unsigned char>(text[offset + index]);
                if ((byte & 0xC0) != 0x80)
                {
                    return {0, 1, false};
                }
                code_point = (code_point << 6) | (byte & 0x3F);
            }

            if ((length == 2 && code_point < 0x80) ||
                (length == 3 && code_point < 0x800) ||
                (length == 4 && code_point < 0x10000) ||
                code_point > 0x10FFFF ||
                (code_point >= 0xD800 && code_point <= 0xDFFF))
            {
                return {0, 1, false};
            }

            return {code_point, length, true};
        }

        void append_utf8(std::string &out, std::uint32_t code_point)
        {
            if (code_point <= 0x7F)
            {
                out.push_back(static_cast<char>(code_point));
            }
            else if (code_point <= 0x7FF)
            {
                out.push_back(static_cast<char>(0xC0 | (code_point >> 6)));
                out.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
            }
            else if (code_point <= 0xFFFF)
            {
                out.push_back(static_cast<char>(0xE0 | (code_point >> 12)));
                out.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
            }
            else
            {
                out.push_back(static_cast<char>(0xF0 | (code_point >> 18)));
                out.push_back(static_cast<char>(0x80 | ((code_point >> 12) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
            }
        }

        [[nodiscard]] bool is_identifier_start(std::string_view text,
                                               std::size_t offset) noexcept
        {
            if (offset >= text.size())
            {
                return false;
            }
            const char c = text[offset];
            if (is_ascii_alpha(c))
            {
                return true;
            }
            if (static_cast<unsigned char>(c) < 0x80)
            {
                return false;
            }
            const auto decoded = decode_utf8(text, offset);
            return decoded.valid && !is_unicode_space(decoded.code_point);
        }

        [[nodiscard]] bool is_identifier_continue(std::string_view text,
                                                  std::size_t offset) noexcept
        {
            if (offset >= text.size())
            {
                return false;
            }
            const char c = text[offset];
            if (is_ascii_alnum(c))
            {
                return true;
            }
            if (static_cast<unsigned char>(c) < 0x80)
            {
                return false;
            }
            const auto decoded = decode_utf8(text, offset);
            return decoded.valid && !is_unicode_space(decoded.code_point);
        }

        [[nodiscard]] Token::Tag keyword_tag(std::string_view text) noexcept
        {
            switch (text.size())
            {
            case 2:
                if (text == "if") return Token::Tag::T_IF;
                if (text == "in") return Token::Tag::T_IN;
                if (text == "fn") return Token::Tag::T_FN;
                break;
            case 3:
                if (text == "for") return Token::Tag::T_FOR;
                if (text == "var" || text == "mut") return Token::Tag::T_VAR;
                if (text == "let") return Token::Tag::T_LET;
                break;
            case 4:
                if (text == "else") return Token::Tag::T_ELSE;
                if (text == "enum") return Token::Tag::T_ENUM;
                if (text == "type") return Token::Tag::T_TYPE;
                if (text == "func") return Token::Tag::T_FN;
                if (text == "true") return Token::Tag::T_TRUE;
                if (text == "null") return Token::Tag::T_NULLLIT;
                if (text == "bool") return Token::Tag::T_TBOOL;
                break;
            case 5:
                if (text == "const") return Token::Tag::T_CONST;
                if (text == "while") return Token::Tag::T_WHILE;
                if (text == "match") return Token::Tag::T_MATCH;
                if (text == "defer") return Token::Tag::T_DEFER;
                if (text == "break") return Token::Tag::T_BREAK;
                if (text == "false") return Token::Tag::T_FALSE;
                break;
            case 6:
                if (text == "import") return Token::Tag::T_IMPORT;
                if (text == "struct") return Token::Tag::T_STRUCT;
                if (text == "export") return Token::Tag::T_EXPORT;
                if (text == "extern") return Token::Tag::T_EXTERN;
                if (text == "return") return Token::Tag::T_RETURN;
                break;
            case 8:
                if (text == "continue") return Token::Tag::T_CONTINUE;
                break;
            default:
                break;
            }
            return Token::Tag::T_IDENT;
        }

    } // namespace

    Lexer::Lexer(std::uint32_t fileId, SourceManager &sm, DiagnosticEngine &de,
                 Config config)
        : m_config(config), m_DE(de), m_fileId(fileId)
    {
        if (const SourceFile *file = sm.get_file(fileId); file != nullptr)
        {
            m_source.assign(file->GetContent());
        }
        m_limit = static_cast<std::uint32_t>(m_source.size());
    }

    Token Lexer::next()
    {
        m_limit = static_cast<std::uint32_t>(m_source.size());
        if (m_offset > m_limit)
        {
            m_offset = m_limit;
        }
        return scan_token();
    }

    void Lexer::lex(TokenStream &stream)
    {
        stream.clear();
        stream.set_file_id(m_fileId);
        stream.set_interner(&m_interner);
        stream.set_source(m_source);
        reset();

        for (;;)
        {
            const Token token = next();
            stream.push(token);
            if (token.tag == Token::Tag::T_EOF)
            {
                break;
            }
        }
    }

    void Lexer::reset()
    {
        m_offset = 0;
        m_limit = static_cast<std::uint32_t>(m_source.size());
        m_at_line_start = true;
    }

    std::uint32_t Lexer::peek_byte(std::size_t lookahead) const
    {
        const std::size_t position = static_cast<std::size_t>(m_offset) + lookahead;
        if (position >= m_source.size())
        {
            return 0;
        }
        return static_cast<unsigned char>(m_source[position]);
    }

    void Lexer::report_invalid_utf8(std::uint32_t start, std::uint32_t length)
    {
        (void)m_DE.emit(DiagID::err_invalid_utf8, span(start, start + length));
    }

    void Lexer::report_digit_separator(std::uint32_t position)
    {
        if (m_config.checkDigitSeparators)
        {
            (void)m_DE.emit(DiagID::warn_digit_separator, span(position, position + 1));
        }
    }

    void Lexer::skip_trivia()
    {
        for (;;)
        {
            bool progressed = false;

            if (m_config.enableWhitespace)
            {
                while (m_offset < m_limit)
                {
                    const char c = m_source[m_offset];
                    if (static_cast<unsigned char>(c) < 0x80)
                    {
                        if (!is_ascii_space(c))
                        {
                            break;
                        }
                        if (c == '\n' || c == '\r')
                        {
                            m_at_line_start = true;
                        }
                        ++m_offset;
                        progressed = true;
                        continue;
                    }

                    const auto decoded = decode_utf8(m_source, m_offset);
                    if (!decoded.valid || !is_unicode_space(decoded.code_point))
                    {
                        break;
                    }
                    if (decoded.code_point == 0x2028 || decoded.code_point == 0x2029)
                    {
                        m_at_line_start = true;
                    }
                    m_offset += decoded.length;
                    progressed = true;
                }
            }

            if (m_config.enableComments && m_offset + 1 < m_limit &&
                m_source[m_offset] == '/')
            {
                const char next = m_source[m_offset + 1];
                if (next == '/')
                {
                    m_offset += 2;
                    while (m_offset < m_limit && m_source[m_offset] != '\n' &&
                           m_source[m_offset] != '\r')
                    {
                        ++m_offset;
                    }
                    progressed = true;
                    continue;
                }
                if (next == '*')
                {
                    (void)skip_block_comment();
                    progressed = true;
                    continue;
                }
            }

            if (!progressed)
            {
                return;
            }
        }
    }

    bool Lexer::skip_block_comment()
    {
        const std::uint32_t start = m_offset;
        m_offset += 2;
        std::size_t depth = 1;
        bool multiline = false;

        while (m_offset < m_limit)
        {
            if (m_offset + 1 < m_limit && m_source[m_offset] == '/' &&
                m_source[m_offset + 1] == '*')
            {
                ++depth;
                m_offset += 2;
                continue;
            }
            if (m_offset + 1 < m_limit && m_source[m_offset] == '*' &&
                m_source[m_offset + 1] == '/')
            {
                --depth;
                m_offset += 2;
                if (depth == 0)
                {
                    if (multiline && m_config.warnOnMultilineComments)
                    {
                        (void)m_DE.emit(DiagID::warn_multiline_comment,
                                        span(start, m_offset));
                    }
                    return true;
                }
                continue;
            }
            if (m_source[m_offset] == '\n' || m_source[m_offset] == '\r')
            {
                multiline = true;
                m_at_line_start = true;
            }
            ++m_offset;
        }

        (void)m_DE.error("unterminated block comment")
            .primary(span(start, static_cast<std::uint32_t>(m_source.size())));
        return false;
    }

    Token Lexer::scan_token()
    {
        for (;;)
        {
            skip_trivia();

            if (m_offset >= m_limit)
            {
                Token eof{};
                eof.tag = Token::Tag::T_EOF;
                eof.loc.start = m_offset;
                eof.loc.end = m_offset;
                return eof;
            }

            const std::uint32_t start = m_offset;
            const std::uint8_t flag =
                m_at_line_start ? static_cast<std::uint8_t>(TOKEN_AT_LINE_START) : 0;
            const char c = m_source[m_offset];

            if (static_cast<unsigned char>(c) >= 0x80)
            {
                const auto decoded = decode_utf8(m_source, m_offset);
                if (!decoded.valid)
                {
                    report_invalid_utf8(m_offset, decoded.length);
                    ++m_offset;
                    continue;
                }
                if (is_unicode_space(decoded.code_point))
                {
                    // Whitespace skipping is disabled, or this is a separator
                    // that is not enabled by the configuration.
                    m_offset += decoded.length;
                    continue;
                }
            }

            Token token{};
            if (is_identifier_start(m_source, m_offset))
            {
                token = scan_identifier(start, flag);
            }
            else if (is_ascii_digit(c))
            {
                token = scan_number(start, flag);
            }
            else if (c == '.' && is_ascii_digit(static_cast<char>(peek_byte(1))))
            {
                token = scan_number(start, flag);
            }
            else if (c == '"')
            {
                token = scan_string(start, flag);
            }
            else if (c == '\'')
            {
                token = scan_char(start, flag);
            }
            else
            {
                token = scan_operator(start, flag);
            }

            if (token.tag == Token::Tag::T_EOF)
            {
                // `scan_operator` uses T_EOF as its "invalid byte skipped"
                // sentinel. A real EOF is returned above.
                if (m_offset > start)
                {
                    m_at_line_start = false;
                }
                continue;
            }

            m_at_line_start = false;
            return token;
        }
    }

    Token Lexer::scan_identifier(std::uint32_t start, std::uint8_t flag)
    {
        while (m_offset < m_limit)
        {
            const char c = m_source[m_offset];
            if (static_cast<unsigned char>(c) < 0x80)
            {
                if (!is_ascii_alnum(c))
                {
                    break;
                }
                ++m_offset;
                continue;
            }

            const auto decoded = decode_utf8(m_source, m_offset);
            if (!decoded.valid || is_unicode_space(decoded.code_point))
            {
                break;
            }
            m_offset += decoded.length;
        }

        const std::string_view text = std::string_view(m_source).substr(
            start, static_cast<std::size_t>(m_offset) - start);
        const std::uint32_t interned = m_interner.intern(text);

        Token token{};
        token.tag = keyword_tag(text);
        token.loc.start = start;
        token.loc.end = m_offset;
        token.flag = flag;
        token.interned_id = interned;
        return token;
    }

    Token Lexer::scan_operator(std::uint32_t start, std::uint8_t flag)
    {
        const auto byte = [this](std::size_t lookahead) -> char
        {
            return static_cast<char>(peek_byte(lookahead));
        };
        const auto make = [&](Token::Tag tag, std::uint32_t length)
        {
            Token token{};
            token.tag = tag;
            token.loc.start = start;
            token.loc.end = start + length;
            token.flag = flag;
            m_offset = start + length;
            return token;
        };

        const char c = m_source[m_offset];
        switch (c)
        {
        case '(': return make(Token::Tag::T_LPAREN, 1);
        case ')': return make(Token::Tag::T_RPAREN, 1);
        case '[': return make(Token::Tag::T_LBRACKET, 1);
        case ']': return make(Token::Tag::T_RBRACKET, 1);
        case '{': return make(Token::Tag::T_LCURLY, 1);
        case '}': return make(Token::Tag::T_RCURLY, 1);
        case ',': return make(Token::Tag::T_COMMA, 1);
        case ';': return make(Token::Tag::T_SEMI, 1);
        case '?': return make(Token::Tag::T_QUESTION, 1);
        case '~': return make(Token::Tag::T_BITNOT, 1);
        case ':':
            if (byte(1) == ':') return make(Token::Tag::T_COLONCOLON, 2);
            return make(Token::Tag::T_COLON, 1);
        case '.':
            if (byte(1) == '=' && byte(2) == '=')
                return make(Token::Tag::T_DOTEQ, 3);
            if (byte(1) == '=') return make(Token::Tag::T_DOTASSIGN, 2);
            if (byte(1) == '!' && byte(2) == '=')
                return make(Token::Tag::T_DOTNEQ, 3);
            if (byte(1) == '.') return make(Token::Tag::T_DOTDOT, 2);
            return make(Token::Tag::T_DOT, 1);
        case '/':
            if (byte(1) == '=') return make(Token::Tag::T_DIVEQ, 2);
            return make(Token::Tag::T_DIV, 1);
        case '+':
            if (byte(1) == '+') return make(Token::Tag::T_INC, 2);
            if (byte(1) == '=') return make(Token::Tag::T_PLUSEQ, 2);
            return make(Token::Tag::T_PLUS, 1);
        case '-':
            if (byte(1) == '>') return make(Token::Tag::T_ARROW, 2);
            if (byte(1) == '-') return make(Token::Tag::T_DEC, 2);
            if (byte(1) == '=') return make(Token::Tag::T_MINUSEQ, 2);
            return make(Token::Tag::T_MINUS, 1);
        case '*':
            if (byte(1) == '=') return make(Token::Tag::T_MULEQ, 2);
            return make(Token::Tag::T_MUL, 1);
        case '%':
            if (byte(1) == '=') return make(Token::Tag::T_MODEQ, 2);
            return make(Token::Tag::T_MOD, 1);
        case '=':
            if (byte(1) == '>') return make(Token::Tag::T_FATARROW, 2);
            if (byte(1) == '=') return make(Token::Tag::T_EQ, 2);
            return make(Token::Tag::T_ASSIGN, 1);
        case '!':
            if (byte(1) == '=') return make(Token::Tag::T_NEQ, 2);
            return make(Token::Tag::T_NOT, 1);
        case '<':
            if (byte(1) == '<' && byte(2) == '=')
                return make(Token::Tag::T_SHLEQ, 3);
            if (byte(1) == '<') return make(Token::Tag::T_SHL, 2);
            if (byte(1) == '=') return make(Token::Tag::T_LTEQ, 2);
            return make(Token::Tag::T_LT, 1);
        case '>':
            if (byte(1) == '>' && byte(2) == '=')
                return make(Token::Tag::T_SHREQ, 3);
            if (byte(1) == '>') return make(Token::Tag::T_SHR, 2);
            if (byte(1) == '=') return make(Token::Tag::T_GTEQ, 2);
            return make(Token::Tag::T_GT, 1);
        case '&':
            if (byte(1) == '&') return make(Token::Tag::T_ANDAND, 2);
            if (byte(1) == '=') return make(Token::Tag::T_ANDEQ, 2);
            return make(Token::Tag::T_BITAND, 1);
        case '|':
            if (byte(1) == '|') return make(Token::Tag::T_OROR, 2);
            if (byte(1) == '=') return make(Token::Tag::T_OREQ, 2);
            return make(Token::Tag::T_BITOR, 1);
        case '^':
            if (byte(1) == '=') return make(Token::Tag::T_XOREQ, 2);
            return make(Token::Tag::T_XOR, 1);
        default:
            break;
        }

        const std::string_view invalid(m_source.data() + start, 1);
        (void)m_DE.emit(DiagID::err_invalid_char, span(start, start + 1), invalid);
        ++m_offset;
        return Token{};
    }

    Token Lexer::scan_number(std::uint32_t start, std::uint8_t flag)
    {
        bool is_float = false;
        bool prefixed = false;
        std::uint32_t digits_start = start;
        int base = 10;

        if (m_offset + 1 < m_limit && m_source[m_offset] == '0')
        {
            const char prefix = m_source[m_offset + 1];
            if (prefix == 'x' || prefix == 'X')
            {
                base = 16;
                prefixed = true;
                m_offset += 2;
                digits_start = m_offset;
            }
            else if (prefix == 'b' || prefix == 'B')
            {
                base = 2;
                prefixed = true;
                m_offset += 2;
                digits_start = m_offset;
            }
            else if (prefix == 'o' || prefix == 'O')
            {
                base = 8;
                prefixed = true;
                m_offset += 2;
                digits_start = m_offset;
            }
        }

        const auto scan_digits = [&](int radix)
        {
            while (m_offset < m_limit)
            {
                const char c = m_source[m_offset];
                const int value = digit_value(c);
                if (value >= 0 && value < radix)
                {
                    ++m_offset;
                    continue;
                }
                if (c != '_')
                {
                    break;
                }

                const bool before = m_offset > start &&
                                    digit_value(m_source[m_offset - 1]) >= 0 &&
                                    digit_value(m_source[m_offset - 1]) < radix;
                const bool after = m_offset + 1 < m_limit &&
                                   digit_value(m_source[m_offset + 1]) >= 0 &&
                                   digit_value(m_source[m_offset + 1]) < radix;
                if (!before || !after)
                {
                    report_digit_separator(m_offset);
                }
                ++m_offset;
            }
        };

        if (prefixed)
        {
            scan_digits(base);
        }
        else if (m_source[m_offset] == '.')
        {
            is_float = true;
            ++m_offset;
            scan_digits(10);
        }
        else
        {
            scan_digits(10);
            if (m_offset + 1 < m_limit && m_source[m_offset] == '.' &&
                is_ascii_digit(m_source[m_offset + 1]))
            {
                is_float = true;
                ++m_offset;
                scan_digits(10);
            }
            if (m_offset < m_limit &&
                (m_source[m_offset] == 'e' || m_source[m_offset] == 'E'))
            {
                std::size_t exponent = static_cast<std::size_t>(m_offset) + 1;
                if (exponent < m_source.size() &&
                    (m_source[exponent] == '+' || m_source[exponent] == '-'))
                {
                    ++exponent;
                }
                if (exponent < m_source.size() &&
                    is_ascii_digit(m_source[exponent]))
                {
                    is_float = true;
                    ++m_offset;
                    if (m_source[m_offset] == '+' || m_source[m_offset] == '-')
                    {
                        ++m_offset;
                    }
                    scan_digits(10);
                }
            }
        }

        const std::uint32_t number_end = m_offset;
        const std::uint32_t suffix_start = m_offset;

        // Numeric suffixes are identifiers: `42u32`, `1.0f64`, `0xFF_usize`.
        if (is_identifier_start(m_source, m_offset))
        {
            ++m_offset;
            while (m_offset < m_limit)
            {
                if (is_identifier_continue(m_source, m_offset))
                {
                    const auto decoded = decode_utf8(m_source, m_offset);
                    m_offset += static_cast<unsigned char>(m_source[m_offset]) < 0x80
                                    ? 1
                                    : decoded.length;
                    continue;
                }
                break;
            }
        }

        if (!is_float && !prefixed && suffix_start < m_offset &&
            (m_source[suffix_start] == 'f' || m_source[suffix_start] == 'F'))
        {
            is_float = true;
        }

        Token token{};
        token.tag = is_float ? Token::Tag::T_FLTLIT : Token::Tag::T_INTLIT;
        token.loc.start = start;
        token.loc.end = m_offset;
        token.flag = flag;

        if (is_float)
        {
            std::string text;
            text.reserve(static_cast<std::size_t>(number_end) - start + 1);
            if (m_source[start] == '.')
            {
                text.push_back('0');
            }
            for (std::uint32_t index = start; index < number_end; ++index)
            {
                if (m_source[index] != '_')
                {
                    text.push_back(m_source[index]);
                }
            }

            double value = 0.0;
            const char* const first = text.data();
            const char* const last = first + text.size();
            const auto result =
                std::from_chars(first, last, value, std::chars_format::general);
            if (result.ec == std::errc::result_out_of_range)
            {
                // `from_chars` leaves `value` untouched on a range error, so
                // saturate explicitly: overflow becomes infinity, underflow 0.
                const char* const exponent = std::find_if(
                    first, last,
                    [](char c) { return c == 'e' || c == 'E'; });
                const bool underflow = exponent != last && exponent + 1 != last &&
                                       exponent[1] == '-';
                value = underflow ? 0.0 : std::numeric_limits<double>::infinity();
            }
            token.float_val = value;
            return token;
        }

        std::uint64_t value = 0;
        const std::uint32_t begin = prefixed ? digits_start : start;
        for (std::uint32_t index = begin; index < number_end; ++index)
        {
            const char c = m_source[index];
            if (c == '_')
            {
                continue;
            }
            const int digit = digit_value(c);
            if (digit < 0 || digit >= base)
            {
                continue;
            }
            if (value > (UINT64_MAX - static_cast<std::uint64_t>(digit)) /
                            static_cast<std::uint64_t>(base))
            {
                value = UINT64_MAX;
                break;
            }
            value = value * static_cast<std::uint64_t>(base) +
                    static_cast<std::uint64_t>(digit);
        }
        token.int_val = value;
        return token;
    }

    std::uint32_t Lexer::decode_escape()
    {
        if (m_offset >= m_limit)
        {
            return '\\';
        }

        const std::uint32_t escape_start = m_offset - 1;
        const char c = m_source[m_offset++];
        switch (c)
        {
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case '0': return 0;
        case 'a': return 7;
        case 'b': return 8;
        case 'f': return 12;
        case 'v': return 11;
        case '\\': return '\\';
        case '\'': return '\'';
        case '"': return '"';
        case 'x':
        {
            std::uint32_t value = 0;
            std::uint8_t count = 0;
            while (count < 2 && m_offset < m_limit)
            {
                const int digit = digit_value(m_source[m_offset]);
                if (digit < 0)
                {
                    break;
                }
                value = value * 16 + static_cast<std::uint32_t>(digit);
                ++m_offset;
                ++count;
            }
            if (count == 0)
            {
                (void)m_DE.emit(DiagID::warn_unknown_escape,
                                span(escape_start, escape_start + 2), c);
            }
            return value;
        }
        case 'u':
        {
            if (m_offset < m_limit && m_source[m_offset] == '{')
            {
                ++m_offset;
                std::uint32_t value = 0;
                while (m_offset < m_limit && m_source[m_offset] != '}')
                {
                    const int digit = digit_value(m_source[m_offset]);
                    if (digit < 0)
                    {
                        break;
                    }
                    value = value * 16 + static_cast<std::uint32_t>(digit);
                    ++m_offset;
                }
                if (m_offset < m_limit && m_source[m_offset] == '}')
                {
                    ++m_offset;
                }
                return value;
            }

            std::uint32_t value = 0;
            std::uint8_t count = 0;
            while (count < 4 && m_offset < m_limit)
            {
                const int digit = digit_value(m_source[m_offset]);
                if (digit < 0)
                {
                    break;
                }
                value = value * 16 + static_cast<std::uint32_t>(digit);
                ++m_offset;
                ++count;
            }
            if (count == 0)
            {
                (void)m_DE.emit(DiagID::warn_unknown_escape,
                                span(escape_start, escape_start + 2), c);
            }
            return value;
        }
        case 'U':
        {
            std::uint32_t value = 0;
            std::uint8_t count = 0;
            while (count < 8 && m_offset < m_limit)
            {
                const int digit = digit_value(m_source[m_offset]);
                if (digit < 0)
                {
                    break;
                }
                value = value * 16 + static_cast<std::uint32_t>(digit);
                ++m_offset;
                ++count;
            }
            if (count == 0)
            {
                (void)m_DE.emit(DiagID::warn_unknown_escape,
                                span(escape_start, escape_start + 2), c);
            }
            return value;
        }
        default:
            (void)m_DE.emit(DiagID::warn_unknown_escape,
                            span(escape_start, escape_start + 2),
                            std::string_view(m_source.data() + escape_start + 1, 1));
            return static_cast<unsigned char>(c);
        }
    }

    Token Lexer::scan_string(std::uint32_t start, std::uint8_t flag)
    {
        ++m_offset;
        std::string decoded;
        bool closed = false;

        while (m_offset < m_limit)
        {
            const char c = m_source[m_offset];
            if (c == '"')
            {
                ++m_offset;
                closed = true;
                break;
            }
            if (c == '\n' || c == '\r')
            {
                break;
            }
            if (c == '\\')
            {
                ++m_offset;
                append_utf8(decoded, decode_escape());
                continue;
            }
            if (static_cast<unsigned char>(c) >= 0x80)
            {
                const auto utf8 = decode_utf8(m_source, m_offset);
                if (!utf8.valid)
                {
                    report_invalid_utf8(m_offset, utf8.length);
                    ++m_offset;
                    continue;
                }
                decoded.append(m_source, m_offset, utf8.length);
                m_offset += utf8.length;
                continue;
            }
            decoded.push_back(c);
            ++m_offset;
        }

        if (!closed)
        {
            (void)m_DE.emit(DiagID::err_unterminated_string,
                            span(start, m_offset));
        }

        Token token{};
        token.tag = Token::Tag::T_STRLIT;
        token.loc.start = start;
        token.loc.end = m_offset;
        token.flag = flag;
        token.interned_id = m_interner.intern(decoded);
        return token;
    }

    Token Lexer::scan_char(std::uint32_t start, std::uint8_t flag)
    {
        ++m_offset;
        std::uint32_t code_point = 0;
        bool closed = false;

        if (m_offset < m_limit && m_source[m_offset] != '\'' &&
            m_source[m_offset] != '\n' && m_source[m_offset] != '\r')
        {
            const char c = m_source[m_offset];
            if (c == '\\')
            {
                ++m_offset;
                code_point = decode_escape();
            }
            else if (static_cast<unsigned char>(c) >= 0x80)
            {
                const auto utf8 = decode_utf8(m_source, m_offset);
                if (utf8.valid)
                {
                    code_point = utf8.code_point;
                    m_offset += utf8.length;
                }
                else
                {
                    report_invalid_utf8(m_offset, utf8.length);
                    ++m_offset;
                }
            }
            else
            {
                code_point = static_cast<unsigned char>(c);
                ++m_offset;
            }

            if (m_offset < m_limit && m_source[m_offset] == '\'')
            {
                ++m_offset;
                closed = true;
            }
            else if (m_offset < m_limit && m_source[m_offset] != '\n' &&
                     m_source[m_offset] != '\r')
            {
                const std::string_view invalid(m_source.data() + m_offset, 1);
                (void)m_DE.emit(DiagID::err_invalid_char,
                                span(m_offset, m_offset + 1), invalid);
                while (m_offset < m_limit && m_source[m_offset] != '\'' &&
                       m_source[m_offset] != '\n' && m_source[m_offset] != '\r')
                {
                    ++m_offset;
                }
                if (m_offset < m_limit && m_source[m_offset] == '\'')
                {
                    ++m_offset;
                    closed = true;
                }
            }
        }
        else if (m_offset < m_limit && m_source[m_offset] == '\'')
        {
            ++m_offset;
            closed = true;
        }

        if (!closed)
        {
            (void)m_DE.emit(DiagID::err_unterminated_char, span(start, m_offset));
        }

        Token token{};
        token.tag = Token::Tag::T_CHARLIT;
        token.loc.start = start;
        token.loc.end = m_offset;
        token.flag = flag;
        token.int_val = code_point;
        return token;
    }

    Lexer::IncrementalResult Lexer::update(std::string_view new_source,
                                           TokenStream &stream)
    {
        IncrementalResult result;

        if (stream.empty())
        {
            std::string updated(new_source);
            result.changed = !m_source.empty() || !updated.empty();
            result.full_relex = true;
            m_source = std::move(updated);
            lex(stream);
            result.relexed_tokens = stream.size() > 0 ? stream.size() - 1 : 0;
            result.relexed_bytes = m_source.size();
            return result;
        }

        if (std::string_view(m_source) == new_source)
        {
            const std::size_t content = stream.size() > 0 &&
                                                stream.tag(stream.size() - 1) ==
                                                    Token::Tag::T_EOF
                                            ? stream.size() - 1
                                            : stream.size();
            result.changed = false;
            result.prefix_tokens = content;
            return result;
        }

        // Copy eagerly: `new_source` may alias `m_source`.
        std::string updated(new_source);
        const std::size_t old_size = m_source.size();
        const std::size_t new_size = updated.size();

        std::size_t prefix = 0;
        const std::size_t common = std::min(old_size, new_size);
        while (prefix < common && m_source[prefix] == updated[prefix])
        {
            ++prefix;
        }
        while (prefix > 0 && prefix < m_source.size() &&
               (static_cast<unsigned char>(m_source[prefix]) & 0xC0) == 0x80)
        {
            --prefix;
        }

        std::size_t suffix = 0;
        while (suffix < common - prefix &&
               m_source[old_size - 1 - suffix] == updated[new_size - 1 - suffix])
        {
            ++suffix;
        }
        // Keep the suffix start on a UTF-8 code point boundary.
        while (suffix > 0 &&
               (static_cast<unsigned char>(m_source[old_size - suffix]) & 0xC0) == 0x80)
        {
            --suffix;
        }

        const std::size_t content_count =
            stream.size() > 0 &&
                    stream.tag(stream.size() - 1) == Token::Tag::T_EOF
                ? stream.size() - 1
                : stream.size();

        if (content_count == 0)
        {
            result.changed = true;
            result.full_relex = true;
            m_source = std::move(updated);
            lex(stream);
            result.relexed_tokens = stream.size() > 0 ? stream.size() - 1 : 0;
            result.relexed_bytes = m_source.size();
            return result;
        }

        const auto &starts = stream.pool().starts();
        auto first_after_prefix = std::lower_bound(
            starts.begin(), starts.begin() + static_cast<std::ptrdiff_t>(content_count),
            static_cast<std::uint32_t>(prefix));
        std::size_t relex_index =
            static_cast<std::size_t>(first_after_prefix - starts.begin());
        if (relex_index >= content_count)
        {
            relex_index = content_count - 1;
        }
        if (relex_index > 0)
        {
            --relex_index;
        }
        if (relex_index > 0)
        {
            --relex_index;
        }

        const std::uint32_t new_start =
            stream.start(relex_index) <= prefix ? stream.start(relex_index) : 0;
        if (new_start == 0)
        {
            relex_index = 0;
        }
        const std::uint32_t desired_fence =
            static_cast<std::uint32_t>(new_size - suffix);

        m_source = std::move(updated);
        m_limit = static_cast<std::uint32_t>(new_size);
        m_offset = new_start;
        // `new_start` always lies inside the unchanged prefix, so the old
        // token's line-start flag is exactly the scanner state a full re-lex
        // would have at this position.
        m_at_line_start =
            new_start == 0 ||
            (stream.flag(relex_index) & TOKEN_AT_LINE_START) != 0;

        m_scratch.clear();
        std::uint32_t fence_offset = static_cast<std::uint32_t>(new_size);
        std::size_t old_fence = content_count;
        std::uint8_t fence_flag = 0;
        bool full_tail = false;

        for (;;)
        {
            const Token token = scan_token();
            if (token.tag == Token::Tag::T_EOF)
            {
                break;
            }

            const bool candidate =
                !full_tail &&
                token.loc.start >= desired_fence &&
                (token.flag & TOKEN_AT_LINE_START) != 0;
            if (candidate)
            {
                const std::int64_t delta = static_cast<std::int64_t>(new_size) -
                                           static_cast<std::int64_t>(old_size);
                const std::int64_t old_start =
                    static_cast<std::int64_t>(token.loc.start) - delta;
                const std::int64_t old_end =
                    static_cast<std::int64_t>(token.loc.end) - delta;
                auto old_it = std::lower_bound(
                    starts.begin(),
                    starts.begin() + static_cast<std::ptrdiff_t>(content_count),
                    static_cast<std::uint32_t>(old_start));
                const std::size_t old_index =
                    static_cast<std::size_t>(old_it - starts.begin());
                if (old_start >= 0 && old_end >= 0 && old_index < content_count &&
                    stream.start(old_index) == static_cast<std::uint32_t>(old_start) &&
                    stream.end(old_index) == static_cast<std::uint32_t>(old_end) &&
                    stream.tag(old_index) == token.tag)
                {
                    fence_offset = static_cast<std::uint32_t>(token.loc.start);
                    old_fence = old_index;
                    // The flag was computed by the live scan over the new
                    // source, so it is valid even if the edit changed the
                    // trivia before this token.
                    fence_flag = token.flag;
                    break;
                }
                // The old stream cannot be resynchronized at this fence
                // (typically a comment or literal changed across the change
                // point). Re-lex the remaining tail from the safe prefix.
                full_tail = true;
            }

            (void)m_scratch.allocate(token.tag,
                                     static_cast<std::uint32_t>(token.loc.start),
                                     static_cast<std::uint32_t>(token.loc.end),
                                     token.flag, TokenPool::value_bits(token));
        }

        if (full_tail)
        {
            old_fence = content_count;
            fence_offset = static_cast<std::uint32_t>(new_size);
        }

        m_suffix.clear();
        if (old_fence < content_count)
        {
            m_suffix.reserve(content_count - old_fence);
            for (std::size_t index = old_fence; index < content_count; ++index)
            {
                m_suffix.push_back(stream.token(index));
            }
        }

        stream.truncate(relex_index);
        stream.set_file_id(m_fileId);
        stream.set_interner(&m_interner);
        stream.set_source(m_source);
        stream.append(m_scratch, 0, m_scratch.size());

        const std::int64_t delta = static_cast<std::int64_t>(new_size) -
                                   static_cast<std::int64_t>(old_size);
        bool first_suffix = true;
        for (Token token : m_suffix)
        {
            const std::int64_t shifted_start =
                static_cast<std::int64_t>(token.loc.start) + delta;
            const std::int64_t shifted_end =
                static_cast<std::int64_t>(token.loc.end) + delta;
            if (shifted_start >= 0 &&
                shifted_start <= static_cast<std::int64_t>(new_size))
            {
                token.loc.start = static_cast<std::size_t>(shifted_start);
            }
            if (shifted_end >= 0 &&
                shifted_end <= static_cast<std::int64_t>(new_size))
            {
                token.loc.end = static_cast<std::size_t>(shifted_end);
            }
            if (first_suffix)
            {
                token.flag = fence_flag;
                first_suffix = false;
            }
            stream.push(token);
        }

        Token eof{};
        eof.tag = Token::Tag::T_EOF;
        eof.loc.start = new_size;
        eof.loc.end = new_size;
        eof.flag = 0;
        stream.push(eof);
        (void)stream.reset(0);

        m_offset = m_limit;

        result.changed = true;
        result.prefix_tokens = relex_index;
        result.suffix_tokens = m_suffix.size();
        result.relexed_tokens = m_scratch.size();
        result.relexed_bytes = fence_offset > new_start ? fence_offset - new_start : 0;
        return result;
    }

} // namespace cora::lexer
