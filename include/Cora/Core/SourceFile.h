#ifndef CORA_CORE_SOURCEFILE_H
#define CORA_CORE_SOURCEFILE_H

#include <Cora/Core/SourceLocation.h>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cora::core
{

    struct SourceFile
    {
        uint32_t id = 0;            ///< Unique identifier assigned by SourceManager.
        std::string path;           ///< Canonical path or file name.
        std::string content;        ///< Full file buffer content.
        std::vector<uint32_t> line; ///< Line offset table: line[i] is the byte offset where line (i+1) starts.

        SourceFile(uint32_t id, const std::string &path, const std::string &content);

        /// Accessors

        const std::string &filename() const;
        const std::string &source() const;
        uint32_t size() const;

        uint32_t getId() const noexcept { return id; }
        const std::string &getPath() const noexcept { return path; }
        std::string getFilename() const;
        const std::string &getContent() const noexcept { return content; }
        size_t getSize() const noexcept { return content.size(); }

        uint32_t getLineCount() const noexcept
        {
            return static_cast<uint32_t>(line.size());
        }

        /// Map a 0-based byte offset to a SourceLocation
        SourceLocation getLocation(uint32_t offset) const;

        /// Map a range [startOffset, startOffset + length) to a SourceSpan
        SourceSpan getSpan(uint32_t startOffset, uint32_t length) const;

        /// Compute line (1-based) and column (1-based) for an offset
        void getLineAndColumn(uint32_t offset, uint32_t &outLine, uint32_t &outColumn) const;

        /// Compute 0-based byte offset from line (1-based) and column (1-based)
        uint32_t getOffset(uint32_t lineNum, uint32_t colNum) const;

        /// Get content of a specific 1-based line (excluding trailing newline / \r)
        std::string_view getLineContent(uint32_t lineNum) const;

        /// Get a slice of the source content
        std::string_view getSlice(uint32_t startOffset, uint32_t length) const;

        /// Get a snippet corresponding to a SourceSpan
        std::string_view getSnippet(const SourceSpan &span) const;

    private:
        void buildLineTable();
    };

} // namespace cora::core

#endif // CORA_CORE_SOURCEFILE_H