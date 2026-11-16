#include <Cora/Lexer/Interner.h>

namespace cora::lexer
{

    std::uint32_t StringInterner::intern(std::string_view text)
    {
        if (const auto existing = m_index.find(text); existing != m_index.end())
        {
            return existing->second;
        }

        const auto id = static_cast<std::uint32_t>(m_storage.size());
        m_storage.emplace_back(text);
        m_index.emplace(std::string_view(m_storage.back()), id);
        return id;
    }

    std::string_view StringInterner::lookup(std::uint32_t id) const noexcept
    {
        if (id >= m_storage.size())
        {
            return {};
        }
        return m_storage[id];
    }

    std::uint32_t StringInterner::find(std::string_view text) const noexcept
    {
        const auto existing = m_index.find(text);
        return existing == m_index.end() ? invalid_id : existing->second;
    }

    void StringInterner::clear() noexcept
    {
        m_index.clear();
        m_storage.clear();
    }

} // namespace cora::lexer
