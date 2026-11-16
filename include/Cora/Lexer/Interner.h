#ifndef CORA_LEXER_INTERNER_H
#define CORA_LEXER_INTERNER_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>

namespace cora::lexer
{

    /// Interns identifier and keyword spellings.
    ///
    /// The interner owns every string it hands out, so the returned views stay
    /// valid for the lifetime of the interner (or until `clear()` is called).
    /// Identifiers compare as integers after interning, which keeps the parser
    /// and symbol tables allocation-free on the hot path.
    class StringInterner final
    {
    public:
        static constexpr std::uint32_t invalid_id = UINT32_MAX;

        [[nodiscard]] std::uint32_t intern(std::string_view text);
        [[nodiscard]] std::string_view lookup(std::uint32_t id) const noexcept;
        [[nodiscard]] std::uint32_t find(std::string_view text) const noexcept;

        [[nodiscard]] std::size_t size() const noexcept { return m_storage.size(); }
        [[nodiscard]] bool empty() const noexcept { return m_storage.empty(); }
        void clear() noexcept;

    private:
        // `std::deque` keeps references (and therefore string_views) stable
        // while the interner grows.
        std::deque<std::string> m_storage;
        std::unordered_map<std::string_view, std::uint32_t> m_index;
    };

} // namespace cora::lexer

#endif // CORA_LEXER_INTERNER_H
