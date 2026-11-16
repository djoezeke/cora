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

private:
    mutable std::shared_mutex mutex_;
    std::vector<std::unique_ptr<SourceFile>> files_;
    std::unordered_map<std::string, FileID> path_to_id_;
};

} // namespace cora::core

#endif // CORA_CORE_SOURCEMANAGER_H
