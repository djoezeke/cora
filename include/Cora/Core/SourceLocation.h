#ifndef CORA_CORE_SOURCELOCATION_H
#define CORA_CORE_SOURCELOCATION_H

#include <algorithm>
#include <cstdint>

namespace cora::core
{

using FileID = std::uint32_t;
constexpr FileID INVALID_FILE_ID = 0;

struct SourceSpan
{
    FileID file_id = INVALID_FILE_ID;
    std::uint32_t start_offset = 0; ///< Inclusive byte offset.
    std::uint32_t end_offset = 0;   ///< Exclusive byte offset.

    [[nodiscard]] constexpr bool is_valid() const
    {
        return file_id != INVALID_FILE_ID;
    }

    [[nodiscard]] constexpr std::uint32_t length() const
    {
        return end_offset > start_offset ? end_offset - start_offset : 0;
    }

    [[nodiscard]] constexpr bool is_empty() const
    {
        return is_valid() && start_offset == end_offset;
    }

    /// Merge two spans (e.g. combining child AST node spans into a parent).
    [[nodiscard]] static constexpr SourceSpan merge(SourceSpan a, SourceSpan b)
    {
        if (!a.is_valid())
        {
            return b;
        }
        if (!b.is_valid())
        {
            return a;
        }
        return SourceSpan{a.file_id, std::min(a.start_offset, b.start_offset),
                          std::max(a.end_offset, b.end_offset)};
    }
};

/// Resolved human-readable position (only created for diagnostics/LSP).
struct LineCol
{
    std::uint32_t line;   ///< 1-based.
    std::uint32_t column; ///< 1-based (byte or UTF-8 codepoint offset).
};

struct Utf8Location
{
    std::uint32_t line = 1;
    std::uint32_t column = 1;
};

struct Utf16Location
{
    std::uint32_t line = 1;
    std::uint32_t column = 1;
};

} // namespace cora::core

#endif // CORA_CORE_SOURCELOCATION_H
