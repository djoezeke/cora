#ifndef CORA_CORE_SOURCEMANAGER_H
#define CORA_CORE_SOURCEMANAGER_H

#include <Cora/Core/SourceFile.h>
#include <Cora/Core/SourceLocation.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace cora::core
{

    class SourceManager
    {
    public:
        SourceManager() = default;
        ~SourceManager() = default;

        // Non-copyable, movable
        SourceManager(const SourceManager &) = delete;
        SourceManager &operator=(const SourceManager &) = delete;
        SourceManager(SourceManager &&) noexcept = default;
        SourceManager &operator=(SourceManager &&) noexcept = default;

        // ===================================================================
        // File Management
        // ===================================================================

        /// Add or retrieve an existing source file from memory.
        uint32_t addFile(const std::string &path, const std::string &content);
        uint32_t addSourceFile(const std::string &path, const std::string &content);

        /// Load a file from the disk into the SourceManager.
        uint32_t loadFile(const std::string &filePath);

        /// Retrieve a file by its unique 1-based or 0-based ID.
        const SourceFile *getFile(uint32_t id) const;

        /// Retrieve a file by path or name.
        const SourceFile *getFile(const std::string &path) const;

        size_t getFileCount() const noexcept { return files.size(); }
        const std::vector<std::unique_ptr<SourceFile>> &getAllFiles() const noexcept { return files; }

    public:
        // Storage collections
        std::vector<std::unique_ptr<SourceFile>> files;
    };

} // namespace cora::core

#endif // CORA_CORE_SOURCEMANAGER_H