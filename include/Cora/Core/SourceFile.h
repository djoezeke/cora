#ifndef CORA_CORE_SOURCEFILE_H
#define CORA_CORE_SOURCEFILE_H

#include <Cora/Core/SourceLocation.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cora::core
{

/// Immutable source buffer with a lazily computed, thread-safe line table.
struct SourceFile
{
public:
    SourceFile(FileID id, std::string path, std::string content)
        : id_(id), path_(std::move(path)), content_(std::move(content))
    {
    }

    // Non-copyable to prevent accidental duplication of large buffers.
    SourceFile(const SourceFile &) = delete;
    SourceFile &operator=(const SourceFile &) = delete;

    [[nodiscard]] FileID GetID() const noexcept { return id_; }
    [[nodiscard]] std::string_view GetPath() const noexcept { return path_; }
    [[nodiscard]] std::string_view GetContent() const noexcept
    {
        return content_;
    }

    /// Map a 0-based byte offset to a 1-based line and column.
    [[nodiscard]] LineCol GetLineCol(std::uint32_t byte_offset) const
    {
        ensure_line_offsets();

        const auto it =
            std::upper_bound(lines_.begin(), lines_.end(), byte_offset);
        const auto line_index =
            static_cast<std::size_t>(std::distance(lines_.begin(), it) - 1);
        const std::uint32_t column_offset = byte_offset - lines_[line_index];
        return LineCol{static_cast<std::uint32_t>(line_index + 1),
                       column_offset + 1};
    }

    /// Get the text of a 1-based line without its trailing newline / CR.
    [[nodiscard]] std::string_view GetLineText(std::uint32_t line_number) const
    {
        ensure_line_offsets();

        if (line_number == 0 || line_number > lines_.size())
        {
            return {};
        }

        const std::size_t start = lines_[line_number - 1];
        std::size_t end = (line_number < lines_.size())
                              ? lines_[line_number]
                              : content_.size();

        if (end > start && content_[end - 1] == '\n')
        {
            --end;
        }
        if (end > start && content_[end - 1] == '\r')
        {
            --end;
        }
        return std::string_view(content_).substr(start, end - start);
    }

private:
    void ensure_line_offsets() const
    {
        std::call_once(line_init_flag_,
                       [this]()
                       {
                           lines_.push_back(0);
                           for (std::size_t index = 0; index < content_.size();
                                ++index)
                           {
                               if (content_[index] == '\n')
                               {
                                   lines_.push_back(
                                       static_cast<std::uint32_t>(index + 1));
                               }
                           }
                       });
    }

    FileID id_ = INVALID_FILE_ID;
    std::string path_;
    std::string content_;
    mutable std::vector<std::uint32_t> lines_;
    mutable std::once_flag line_init_flag_;
};

} // namespace cora::core

#endif // CORA_CORE_SOURCEFILE_H
