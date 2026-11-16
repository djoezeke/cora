#ifndef CORA_DIAGNOSTIC_EMITTER_H
#define CORA_DIAGNOSTIC_EMITTER_H

#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/Diagnostic.h>

#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cora::diag
{

using SourceSpan = cora::core::SourceSpan;
using SourceManager = cora::core::SourceManager;

/// Pluggable diagnostic output.
///
/// Implement this interface to add a new format or sink. The engine serializes
/// calls, but implementations may still guard their own state for direct use.
class DiagnosticEmitter
{
public:
    virtual ~DiagnosticEmitter() = default;
    virtual void emit(const Diagnostic &diagnostic) = 0;
    virtual void flush() {}
    virtual void finish() {}
};

namespace detail
{

inline void write_json_string(std::ostream &out, std::string_view value)
{
    constexpr const char *hex = "0123456789abcdef";
    out.put('"');
    for (unsigned char character : value)
    {
        switch (character)
        {
        case '"':
            out << "\\\"";
            break;
        case '\\':
            out << "\\\\";
            break;
        case '\b':
            out << "\\b";
            break;
        case '\f':
            out << "\\f";
            break;
        case '\n':
            out << "\\n";
            break;
        case '\r':
            out << "\\r";
            break;
        case '\t':
            out << "\\t";
            break;
        default:
            if (character < 0x20)
            {
                out << "\\u00" << hex[(character >> 4) & 0x0f]
                    << hex[character & 0x0f];
            }
            else
            {
                out.put(static_cast<char>(character));
            }
            break;
        }
    }
    out.put('"');
}

[[nodiscard]] inline std::string json_string(std::string_view value)
{
    std::ostringstream stream;
    write_json_string(stream, value);
    return stream.str();
}

[[nodiscard]] inline std::string file_uri(std::string_view path)
{
    std::string uri = "file://";
    if (path.size() >= 2 && path[1] == ':')
    {
        uri.push_back('/');
    }
    for (char character : path)
    {
        switch (character)
        {
        case '\\':
            uri.push_back('/');
            break;
        case ' ':
            uri.append("%20");
            break;
        case '%':
            uri.append("%25");
            break;
        case '#':
            uri.append("%23");
            break;
        case '?':
            uri.append("%3F");
            break;
        default:
            uri.push_back(character);
            break;
        }
    }
    return uri;
}

[[nodiscard]] inline int lsp_severity(Severity severity) noexcept
{
    switch (severity)
    {
    case Severity::Error:
    case Severity::Fatal:
        return 1;
    case Severity::Warning:
        return 2;
    case Severity::Remark:
    case Severity::Note:
        return 3;
    case Severity::Help:
        return 4;
    case Severity::Ignored:
        return 4;
    }
    return 1;
}

[[nodiscard]] inline bool needs_warning_prefix(const std::string &code) noexcept
{
    return !code.empty() && code.front() != 'E' && code.front() != 'W';
}

} // namespace detail

/// Human-readable clang-like terminal renderer with source snippets.
class TextEmitter final : public DiagnosticEmitter
{
public:
    explicit TextEmitter(const SourceManager &source_manager,
                         std::ostream &out = std::cerr, bool use_color = true,
                         bool show_carets = true, bool show_column = true,
                         bool show_source_ranges = false, bool show_fixits = true,
                         bool show_summary = true, bool show_option = true,
                         bool absolute_paths = false);

    void emit(const Diagnostic &diagnostic) override;
    void flush() override;
    void finish() override;

private:
    void emit_locked(const Diagnostic &diagnostic);
    void write_header(const Diagnostic &diagnostic, SourceSpan span);
    void write_snippet(const Label &label);

    const SourceManager &source_manager_;
    std::ostream &out_;
    bool color_;
    bool show_carets_;
    bool show_column_;
    bool show_source_ranges_;
    bool show_fixits_;
    bool show_summary_;
    bool show_option_;
    bool absolute_paths_;
    mutable std::mutex mutex_;
    std::size_t errors_ = 0;
    std::size_t warnings_ = 0;
    bool summary_written_ = false;
};

/// Machine-readable JSON array output.
///
/// Each diagnostic becomes an object containing severity, code, message,
/// location, labels, hints and suggestions.
class JsonEmitter final : public DiagnosticEmitter
{
public:
    explicit JsonEmitter(std::ostream &out, const SourceManager *source_manager = nullptr,
                         bool pretty = false)
        : out_(out), source_manager_(source_manager), pretty_(pretty)
    {
    }

    JsonEmitter(const SourceManager &source_manager, std::ostream &out,
                bool pretty = false)
        : out_(out), source_manager_(&source_manager), pretty_(pretty)
    {
    }

    void emit(const Diagnostic &diagnostic) override;
    void flush() override;
    void finish() override;

private:
    void write_diagnostic(const Diagnostic &diagnostic);
    void write_location(SourceSpan span);

    std::ostream &out_;
    const SourceManager *source_manager_;
    bool pretty_;
    bool started_ = false;
    bool finished_ = false;
    mutable std::mutex mutex_;
};

/// LSP `textDocument/publishDiagnostics` notifications grouped by file.
class LspEmitter final : public DiagnosticEmitter
{
public:
    explicit LspEmitter(const SourceManager &source_manager, std::ostream &out)
        : source_manager_(source_manager), out_(out)
    {
    }

    void emit(const Diagnostic &diagnostic) override;
    void flush() override;
    void finish() override;

private:
    void write_notification(cora::core::FileID file_id,
                            const std::vector<Diagnostic> &diagnostics);
    void write_pending_locked();

    const SourceManager &source_manager_;
    std::ostream &out_;
    mutable std::mutex mutex_;
    std::map<cora::core::FileID, std::vector<Diagnostic>> pending_;
    bool dirty_ = false;
};

/// In-memory emitter for unit tests.
class CollectingEmitter final : public DiagnosticEmitter
{
public:
    void emit(const Diagnostic &diagnostic) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        diagnostics_.push_back(diagnostic);
    }

    [[nodiscard]] std::vector<Diagnostic> diagnostics() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return diagnostics_;
    }

    [[nodiscard]] std::vector<Diagnostic> take()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Diagnostic> result;
        result.swap(diagnostics_);
        return result;
    }

    [[nodiscard]] std::size_t count() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return diagnostics_.size();
    }

    [[nodiscard]] std::optional<Diagnostic> last() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (diagnostics_.empty())
        {
            return std::nullopt;
        }
        return diagnostics_.back();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        diagnostics_.clear();
    }

private:
    mutable std::mutex mutex_;
    std::vector<Diagnostic> diagnostics_;
};

using UnitTestEmitter = CollectingEmitter;

/// Adapter for arbitrary callbacks. Useful for embedding and custom sinks.
class CallbackEmitter final : public DiagnosticEmitter
{
public:
    using Callback = std::function<void(const Diagnostic &)>;

    explicit CallbackEmitter(Callback callback) : callback_(std::move(callback)) {}

    void emit(const Diagnostic &diagnostic) override
    {
        if (callback_)
        {
            callback_(diagnostic);
        }
    }

private:
    Callback callback_;
};

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_EMITTER_H
