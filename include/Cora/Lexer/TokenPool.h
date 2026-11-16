#ifndef CORA_LEXER_TOKENPOOL_H
#define CORA_LEXER_TOKENPOOL_H

#include <Cora/Lexer/Token.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace cora::lexer
{

    /// Contiguous structure-of-arrays storage for tokens.
    ///
    /// All five fields live in separate contiguous buffers so a parser that
    /// only inspects tags never touches the payload arrays. Memory is retained
    /// across `clear()`, and `deallocate()` is a stack-style rewind used by
    /// backtracking and incremental lexing. That keeps allocations bounded and
    /// avoids per-token heap traffic.
    class TokenPool final
    {
    public:
        using index_type = std::size_t;

        TokenPool() = default;
        explicit TokenPool(index_type capacity) { reserve(capacity); }

        [[nodiscard]] index_type size() const noexcept { return m_tags.size(); }
        [[nodiscard]] index_type capacity() const noexcept { return m_tags.capacity(); }
        [[nodiscard]] bool empty() const noexcept { return m_tags.empty(); }

        void reserve(index_type capacity)
        {
            m_tags.reserve(capacity);
            m_starts.reserve(capacity);
            m_ends.reserve(capacity);
            m_flags.reserve(capacity);
            m_values.reserve(capacity);
        }

        /// Drop every token but keep the allocated buffers for reuse.
        void clear() noexcept
        {
            m_tags.clear();
            m_starts.clear();
            m_ends.clear();
            m_flags.clear();
            m_values.clear();
        }

        /// Allocate the next slot and return its index.
        index_type allocate(Token::Tag tag = Token::Tag::T_EOF,
                            std::uint32_t start = 0, std::uint32_t end = 0,
                            std::uint8_t flag = 0, std::uint64_t value = 0)
        {
            const index_type index = size();
            m_tags.push_back(tag);
            m_starts.push_back(start);
            m_ends.push_back(end);
            m_flags.push_back(flag);
            m_values.push_back(value);
            return index;
        }

        /// Stack-style deallocation: remove the last `count` tokens.
        void deallocate(index_type count = 1) noexcept
        {
            if (count > size())
            {
                count = size();
            }
            const auto new_size = size() - count;
            m_tags.resize(new_size);
            m_starts.resize(new_size);
            m_ends.resize(new_size);
            m_flags.resize(new_size);
            m_values.resize(new_size);
        }

        void set(index_type index, Token::Tag tag, std::uint32_t start,
                 std::uint32_t end, std::uint8_t flag, std::uint64_t value) noexcept
        {
            m_tags[index] = tag;
            m_starts[index] = start;
            m_ends[index] = end;
            m_flags[index] = flag;
            m_values[index] = value;
        }

        [[nodiscard]] Token::Tag tag(index_type index) const noexcept
        {
            return m_tags[index];
        }
        [[nodiscard]] std::uint32_t start(index_type index) const noexcept
        {
            return m_starts[index];
        }
        [[nodiscard]] std::uint32_t end(index_type index) const noexcept
        {
            return m_ends[index];
        }
        [[nodiscard]] std::uint8_t flag(index_type index) const noexcept
        {
            return m_flags[index];
        }
        [[nodiscard]] std::uint64_t value(index_type index) const noexcept
        {
            return m_values[index];
        }

        [[nodiscard]] const std::vector<Token::Tag> &tags() const noexcept
        {
            return m_tags;
        }
        [[nodiscard]] const std::vector<std::uint32_t> &starts() const noexcept
        {
            return m_starts;
        }
        [[nodiscard]] const std::vector<std::uint32_t> &ends() const noexcept
        {
            return m_ends;
        }

        /// Reconstruct a value token from the columnar storage.
        [[nodiscard]] Token token(index_type index) const noexcept
        {
            Token result{};
            result.tag = m_tags[index];
            result.loc.start = m_starts[index];
            result.loc.end = m_ends[index];
            result.flag = m_flags[index];
            if (result.tag == Token::Tag::T_FLTLIT)
            {
                const std::uint64_t bits = m_values[index];
                std::memcpy(&result.float_val, &bits, sizeof(result.float_val));
            }
            else
            {
                result.int_val = m_values[index];
            }
            return result;
        }

        static std::uint64_t value_bits(const Token &token) noexcept
        {
            std::uint64_t bits = 0;
            if (token.tag == Token::Tag::T_FLTLIT)
            {
                std::memcpy(&bits, &token.float_val, sizeof(bits));
            }
            else
            {
                bits = token.int_val;
            }
            return bits;
        }

    private:
        std::vector<Token::Tag> m_tags;
        std::vector<std::uint32_t> m_starts;
        std::vector<std::uint32_t> m_ends;
        std::vector<std::uint8_t> m_flags;
        std::vector<std::uint64_t> m_values;
    };

} // namespace cora::lexer

#endif // CORA_LEXER_TOKENPOOL_H
