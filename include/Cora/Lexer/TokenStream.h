#ifndef CORA_LEXER_TOKENSTREAM_H
#define CORA_LEXER_TOKENSTREAM_H

#include <Cora/Core/SourceLocation.h>
#include <Cora/Lexer/Interner.h>
#include <Cora/Lexer/Token.h>
#include <Cora/Lexer/TokenPool.h>

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace cora::lexer
{

    /// Cheap, backtrackable view over a `TokenPool`.
    ///
    /// The payload stays in the pool. `peek()` returns a `Token` by value so
    /// callers never hold a pointer into storage that may be reallocated.
    class TokenStream final
    {
    public:
        TokenStream() = default;
        explicit TokenStream(cora::core::FileID file_id,
                             const StringInterner *interner = nullptr)
            : m_file_id(file_id), m_interner(interner)
        {
        }

        // ── Storage ──────────────────────────────────────────────────────
        [[nodiscard]] std::size_t size() const noexcept { return m_pool.size(); }
        [[nodiscard]] bool empty() const noexcept { return m_pool.empty(); }
        [[nodiscard]] std::size_t capacity() const noexcept { return m_pool.capacity(); }
        void reserve(std::size_t capacity) { m_pool.reserve(capacity); }
        void clear() noexcept
        {
            m_pool.clear();
            m_cursor = 0;
        }

        /// Remove tokens from the tail. The cursor is clamped to the new size.
        void truncate(std::size_t count) noexcept
        {
            if (count >= size())
            {
                return;
            }
            m_pool.deallocate(size() - count);
            if (m_cursor > count)
            {
                m_cursor = count;
            }
        }

        void push(const Token &token)
        {
            m_pool.allocate(token.tag, static_cast<std::uint32_t>(token.loc.start),
                            static_cast<std::uint32_t>(token.loc.end), token.flag,
                            TokenPool::value_bits(token));
        }

        void push(Token::Tag tag, std::uint32_t start, std::uint32_t end,
                  std::uint8_t flag = 0, std::uint64_t value = 0)
        {
            m_pool.allocate(tag, start, end, flag, value);
        }

        void append(const TokenPool &pool, std::size_t begin, std::size_t end)
        {
            for (std::size_t index = begin; index < end; ++index)
            {
                m_pool.allocate(pool.tag(index), pool.start(index), pool.end(index),
                                pool.flag(index), pool.value(index));
            }
        }

        // ── Column access ────────────────────────────────────────────────
        [[nodiscard]] Token::Tag tag(std::size_t index) const noexcept
        {
            return m_pool.tag(index);
        }
        [[nodiscard]] std::uint32_t start(std::size_t index) const noexcept
        {
            return m_pool.start(index);
        }
        [[nodiscard]] std::uint32_t end(std::size_t index) const noexcept
        {
            return m_pool.end(index);
        }
        [[nodiscard]] std::uint8_t flag(std::size_t index) const noexcept
        {
            return m_pool.flag(index);
        }
        [[nodiscard]] std::uint64_t value(std::size_t index) const noexcept
        {
            return m_pool.value(index);
        }
        [[nodiscard]] Token token(std::size_t index) const noexcept
        {
            return m_pool.token(index);
        }
        [[nodiscard]] cora::core::SourceSpan span(std::size_t index) const noexcept
        {
            return cora::core::SourceSpan{m_file_id, m_pool.start(index),
                                          m_pool.end(index)};
        }

        [[nodiscard]] std::string_view spelling(std::size_t index) const noexcept;
        [[nodiscard]] std::string_view identifier(std::size_t index) const noexcept;
        [[nodiscard]] const StringInterner *interner() const noexcept
        {
            return m_interner;
        }
        void set_interner(const StringInterner *interner) noexcept
        {
            m_interner = interner;
        }

        // ── Source association (for diagnostics and spelling) ────────────
        void set_source(std::string_view source) noexcept { m_source = source; }
        [[nodiscard]] std::string_view source() const noexcept { return m_source; }

        [[nodiscard]] cora::core::FileID file_id() const noexcept { return m_file_id; }
        void set_file_id(cora::core::FileID file_id) noexcept { m_file_id = file_id; }

        // ── Cursor ───────────────────────────────────────────────────────
        [[nodiscard]] std::size_t cursor() const noexcept { return m_cursor; }
        [[nodiscard]] std::size_t mark() const noexcept { return m_cursor; }
        [[nodiscard]] bool at_end() const noexcept { return m_cursor >= size(); }
        [[nodiscard]] std::size_t remaining() const noexcept
        {
            return size() - m_cursor;
        }

        [[nodiscard]] Token peek(std::size_t lookahead = 0) const noexcept;
        [[nodiscard]] Token::Tag peek_tag(std::size_t lookahead = 0) const noexcept;
        [[nodiscard]] bool check(Token::Tag expected) const noexcept
        {
            return peek_tag() == expected;
        }

        Token consume(std::size_t count = 1) noexcept;
        bool consume_if(Token::Tag expected, Token *consumed = nullptr) noexcept;
        void backtrack(std::size_t count = 1) noexcept;
        bool reset(std::size_t mark) noexcept;

        [[nodiscard]] TokenPool &pool() noexcept { return m_pool; }
        [[nodiscard]] const TokenPool &pool() const noexcept { return m_pool; }

    private:
        [[nodiscard]] std::uint32_t eof_offset() const noexcept;

        TokenPool m_pool;
        std::size_t m_cursor = 0;
        cora::core::FileID m_file_id = cora::core::INVALID_FILE_ID;
        std::string_view m_source;
        const StringInterner *m_interner = nullptr;
    };

} // namespace cora::lexer

#endif // CORA_LEXER_TOKENSTREAM_H
