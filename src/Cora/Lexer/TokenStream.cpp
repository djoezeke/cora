#include <Cora/Lexer/TokenNames.h>
#include <Cora/Lexer/TokenStream.h>

namespace cora::lexer
{

    std::string_view TokenStream::spelling(std::size_t index) const noexcept
    {
        if (index >= size())
        {
            return {};
        }

        const auto begin = static_cast<std::size_t>(start(index));
        const auto finish = static_cast<std::size_t>(end(index));
        if (!m_source.empty() && finish <= m_source.size() && begin <= finish)
        {
            return m_source.substr(begin, finish - begin);
        }

        if (m_interner != nullptr && is_literal_tag(tag(index)) &&
            tag(index) == Token::Tag::T_STRLIT)
        {
            return m_interner->lookup(static_cast<std::uint32_t>(value(index)));
        }
        if (m_interner != nullptr &&
            (tag(index) == Token::Tag::T_IDENT || is_keyword_tag(tag(index))))
        {
            return m_interner->lookup(static_cast<std::uint32_t>(value(index)));
        }
        return {};
    }

    std::string_view TokenStream::identifier(std::size_t index) const noexcept
    {
        if (index >= size())
        {
            return {};
        }
        if (m_interner == nullptr)
        {
            return spelling(index);
        }
        return m_interner->lookup(static_cast<std::uint32_t>(value(index)));
    }

    std::uint32_t TokenStream::eof_offset() const noexcept
    {
        if (!m_source.empty())
        {
            return static_cast<std::uint32_t>(m_source.size());
        }
        if (!m_pool.empty())
        {
            return m_pool.end(m_pool.size() - 1);
        }
        return 0;
    }

    Token TokenStream::peek(std::size_t lookahead) const noexcept
    {
        const std::size_t index = m_cursor + lookahead;
        if (index < size())
        {
            return token(index);
        }

        Token eof{};
        eof.tag = Token::Tag::T_EOF;
        eof.loc.start = eof_offset();
        eof.loc.end = eof.loc.start;
        return eof;
    }

    Token::Tag TokenStream::peek_tag(std::size_t lookahead) const noexcept
    {
        const std::size_t index = m_cursor + lookahead;
        return index < size() ? tag(index) : Token::Tag::T_EOF;
    }

    Token TokenStream::consume(std::size_t count) noexcept
    {
        Token current = peek();
        if (m_cursor < size())
        {
            m_cursor += count < remaining() ? count : remaining();
        }
        return current;
    }

    bool TokenStream::consume_if(Token::Tag expected, Token *consumed) noexcept
    {
        if (peek_tag() != expected)
        {
            return false;
        }
        Token current = consume();
        if (consumed != nullptr)
        {
            *consumed = current;
        }
        return true;
    }

    void TokenStream::backtrack(std::size_t count) noexcept
    {
        m_cursor = count > m_cursor ? 0 : m_cursor - count;
    }

    bool TokenStream::reset(std::size_t mark) noexcept
    {
        if (mark > size())
        {
            return false;
        }
        m_cursor = mark;
        return true;
    }

} // namespace cora::lexer
