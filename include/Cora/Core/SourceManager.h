#ifndef CORA_CORE_SOURCEMANAGER_H
#define CORA_CORE_SOURCEMANAGER_H

#include <Cora/Core/SourceFile.h>

#include <fstream>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cora::core
{

class SourceManager
{
public:
    SourceManager()
    {
        // Index 0 is reserved for INVALID_FILE_ID.
        files_.push_back(nullptr);
    }

    SourceManager(const SourceManager &) = delete;
    SourceManager &operator=(const SourceManager &) = delete;

    /// Thread-safe file loading, deduplicated by path.
    [[nodiscard]] std::optional<FileID> load_file(const std::string &path)
    {
        {
            std::shared_lock read_lock(mutex_);
            const auto existing = path_to_id_.find(path);
            if (existing != path_to_id_.end())
            {
                return existing->second;
            }
        }

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            return std::nullopt;
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();
        return add_virtual_file(path, buffer.str());
    }

    /// Thread-safe registration for in-memory buffers, tests, or a REPL.
    FileID add_virtual_file(std::string name, std::string content)
    {
        std::unique_lock write_lock(mutex_);

        const auto existing = path_to_id_.find(name);
        if (existing != path_to_id_.end())
        {
            return existing->second;
        }

        const FileID new_id = static_cast<FileID>(files_.size());
        auto source_file =
            std::make_unique<SourceFile>(new_id, name, std::move(content));

        path_to_id_.emplace(std::move(name), new_id);
        files_.push_back(std::move(source_file));
        return new_id;
    }

    /// Replace the immutable snapshot for an existing virtual file while
    /// preserving its FileID for incremental parser clients.
    bool update_virtual_file(FileID id, std::string content)
    {
        std::unique_lock write_lock(mutex_);
        if (id == INVALID_FILE_ID || id >= files_.size() || files_[id] == nullptr)
            return false;
        const std::string path(files_[id]->GetPath());
        files_[id] = std::make_unique<SourceFile>(id, path, std::move(content));
        return true;
    }

    /// Safe to call concurrently from worker threads.
    [[nodiscard]] const SourceFile *get_file(FileID id) const
    {
        std::shared_lock read_lock(mutex_);
        if (id == INVALID_FILE_ID || id >= files_.size())
        {
            return nullptr;
        }
        return files_[id].get();
    }

    /// Resolve the start of a span to a 1-based line and column.
    [[nodiscard]] std::optional<LineCol> resolve_location(SourceSpan span) const
    {
        const SourceFile *file = get_file(span.file_id);
        if (file == nullptr)
        {
            return std::nullopt;
        }
        return file->GetLineCol(span.start_offset);
    }

    [[nodiscard]] std::optional<Utf8Location> resolve_utf8_location(
        SourceSpan span) const
    {
        const auto location = resolve_location(span);
        if (!location)
            return std::nullopt;
        return Utf8Location{location->line, location->column};
    }

    [[nodiscard]] std::optional<Utf16Location> resolve_utf16_location(
        SourceSpan span) const
    {
        const SourceFile *file = get_file(span.file_id);
        if (file == nullptr)
            return std::nullopt;
        const LineCol byte_location = file->GetLineCol(span.start_offset);
        const std::string_view line = file->GetLineText(byte_location.line);
        const std::size_t byte_count =
            byte_location.column == 0 ? 0 : byte_location.column - 1;
        std::uint32_t units = 0;
        for (std::size_t index = 0; index < byte_count && index < line.size();)
        {
            const unsigned char value =
                static_cast<unsigned char>(line[index]);
            const std::size_t width =
                value < 0x80 ? 1 : (value < 0xE0 ? 2 : (value < 0xF0 ? 3 : 4));
            units += width == 4 ? 2 : 1;
            index += width;
        }
        return Utf16Location{byte_location.line, units + 1};
    }

private:
    mutable std::shared_mutex mutex_;
    std::vector<std::unique_ptr<SourceFile>> files_;
    std::unordered_map<std::string, FileID> path_to_id_;
};

} // namespace cora::core

#endif // CORA_CORE_SOURCEMANAGER_H
