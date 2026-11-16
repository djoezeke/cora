#include <Cora/Diag/DiagnosticEmitter.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace cora::diag
{
namespace
{

[[nodiscard]] const char *color_for(Severity severity) noexcept
{
    switch (severity)
    {
    case Severity::Warning:
        return "\033[1;33m";
    case Severity::Error:
    case Severity::Fatal:
        return "\033[1;31m";
    case Severity::Remark:
    case Severity::Note:
    case Severity::Help:
        return "\033[1;36m";
    case Severity::Ignored:
        return "";
    }
    return "";
}

} // namespace

TextEmitter::TextEmitter(const SourceManager &source_manager, std::ostream &out,
                         bool use_color, bool show_carets, bool show_column,
                         bool show_source_ranges, bool show_fixits,
                         bool show_summary, bool show_option,
                         bool absolute_paths)
    : source_manager_(source_manager),
      out_(out),
      color_(use_color),
      show_carets_(show_carets),
      show_column_(show_column),
      show_source_ranges_(show_source_ranges),
      show_fixits_(show_fixits),
      show_summary_(show_summary),
      show_option_(show_option),
      absolute_paths_(absolute_paths)
{
}

void TextEmitter::emit(const Diagnostic &diagnostic)
{
    std::lock_guard<std::mutex> lock(mutex_);
    summary_written_ = false;
    if (diagnostic.severity() == Severity::Warning)
    {
        ++warnings_;
    }
    else if (is_error_severity(diagnostic.severity()))
    {
        ++errors_;
    }
    emit_locked(diagnostic);
}

void TextEmitter::emit_locked(const Diagnostic &diagnostic)
{
    const SourceSpan span = diagnostic.primary_span();
    write_header(diagnostic, span);

    if (show_carets_)
    {
        for (const Label &label : diagnostic.labels())
        {
            write_snippet(label);
        }
    }

    for (const std::string &hint : diagnostic.hints())
    {
        if (hint.rfind("help: ", 0) == 0)
        {
            out_ << "  " << hint << '\n';
        }
        else
        {
            out_ << "  note: " << hint << '\n';
        }
    }

    if (show_fixits_)
    {
        for (const Suggestion &suggestion : diagnostic.suggestions())
        {
            out_ << "  help: "
                 << (suggestion.message.empty() ? "suggested fix"
                                                : suggestion.message)
                 << '\n';
            out_ << "    replace with: "
                 << (suggestion.replacement.empty() ? "<delete>"
                                                    : suggestion.replacement)
                 << '\n';
        }
    }
}

void TextEmitter::write_header(const Diagnostic &diagnostic, SourceSpan span)
{
    if (span.is_valid())
    {
        if (const cora::core::SourceFile *file =
                source_manager_.get_file(span.file_id))
        {
            const cora::core::LineCol location =
                file->GetLineCol(span.start_offset);

            std::string path(file->GetPath());
            if (absolute_paths_)
            {
                std::error_code error;
                const std::filesystem::path absolute =
                    std::filesystem::absolute(path, error);
                if (!error)
                {
                    path = absolute.string();
                }
            }
            out_ << path;
            if (show_column_)
            {
                out_ << ':' << location.line << ':' << location.column;
            }
            else
            {
                out_ << ':' << location.line;
            }
            out_ << ": ";
        }
        else
        {
            out_ << "<unknown>: ";
        }
    }

    if (color_)
    {
        out_ << color_for(diagnostic.severity());
    }
    out_ << severity_name(diagnostic.severity());
    if (color_)
    {
        out_ << "\033[0m";
    }
    out_ << ": " << diagnostic.message();

    if (show_option_ && diagnostic.has_code())
    {
        out_ << " [";
        if (detail::needs_warning_prefix(diagnostic.code()))
        {
            out_ << "-W";
        }
        out_ << diagnostic.code() << ']';
    }
    out_ << '\n';
}

void TextEmitter::write_snippet(const Label &label)
{
    if (!label.span.is_valid())
    {
        return;
    }

    const cora::core::SourceFile *file =
        source_manager_.get_file(label.span.file_id);
    if (file == nullptr)
    {
        return;
    }

    const cora::core::LineCol location =
        file->GetLineCol(label.span.start_offset);
    const std::string_view text = file->GetLineText(location.line);
    const std::string line_number = std::to_string(location.line);
    const std::string gutter(line_number.size() + 1, ' ');

    out_ << ' ' << line_number << " | " << text << '\n';

    const std::size_t column =
        location.column > 0 ? static_cast<std::size_t>(location.column - 1) : 0;
    std::size_t length = 1;
    if (label.span.end_offset > label.span.start_offset)
    {
        length = static_cast<std::size_t>(label.span.end_offset -
                                          label.span.start_offset);
        if (column < text.size())
        {
            length = std::min(length, text.size() - column);
        }
        else
        {
            length = 1;
        }
        if (length == 0)
        {
            length = 1;
        }
    }

    out_ << gutter << " | " << std::string(column, ' ')
         << std::string(length, '^');
    if (!label.message.empty())
    {
        out_ << ' ' << label.message;
    }
    out_ << '\n';

    if (show_source_ranges_)
    {
        out_ << gutter << " = bytes [" << label.span.start_offset << ", "
             << label.span.end_offset << ")\n";
    }
}

void TextEmitter::flush()
{
    std::lock_guard<std::mutex> lock(mutex_);
    out_.flush();
}

void TextEmitter::finish()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (summary_written_)
    {
        return;
    }
    if (show_summary_ && (errors_ != 0 || warnings_ != 0))
    {
        out_ << errors_ << (errors_ == 1 ? " error" : " errors") << ", "
             << warnings_ << (warnings_ == 1 ? " warning" : " warnings")
             << " generated.\n";
    }
    summary_written_ = true;
    out_.flush();
}

void JsonEmitter::emit(const Diagnostic &diagnostic)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (finished_)
    {
        started_ = false;
        finished_ = false;
    }
    if (!started_)
    {
        out_ << (pretty_ ? "[\n" : "[");
        started_ = true;
    }
    else
    {
        out_ << (pretty_ ? ",\n" : ",");
    }
    write_diagnostic(diagnostic);
}

void JsonEmitter::write_location(SourceSpan span)
{
    if (!span.is_valid() || source_manager_ == nullptr)
    {
        out_ << "null";
        return;
    }

    const cora::core::SourceFile *file = source_manager_->get_file(span.file_id);
    if (file == nullptr)
    {
        out_ << "null";
        return;
    }

    const cora::core::LineCol start = file->GetLineCol(span.start_offset);
    const cora::core::LineCol end = file->GetLineCol(span.end_offset);
    out_ << "{\"file\":";
    detail::write_json_string(out_, file->GetPath());
    out_ << ",\"line\":" << start.line << ",\"column\":" << start.column
         << ",\"endLine\":" << end.line << ",\"endColumn\":" << end.column
         << '}';
}

void JsonEmitter::write_diagnostic(const Diagnostic &diagnostic)
{
    out_ << "{\"severity\":";
    detail::write_json_string(out_, severity_name(diagnostic.severity()));
    if (diagnostic.has_code())
    {
        out_ << ",\"code\":";
        detail::write_json_string(out_, diagnostic.code());
    }
    out_ << ",\"message\":";
    detail::write_json_string(out_, diagnostic.message());

    const SourceSpan span = diagnostic.primary_span();
    if (span.is_valid())
    {
        out_ << ",\"location\":";
        write_location(span);
    }

    out_ << ",\"labels\":[";
    for (std::size_t index = 0; index < diagnostic.labels().size(); ++index)
    {
        if (index != 0)
        {
            out_ << ',';
        }
        const Label &label = diagnostic.labels()[index];
        out_ << "{\"location\":";
        write_location(label.span);
        out_ << ",\"message\":";
        detail::write_json_string(out_, label.message);
        out_ << ",\"style\":";
        detail::write_json_string(out_, label_style_name(label.style));
        out_ << '}';
    }
    out_ << "],\"hints\":[";
    for (std::size_t index = 0; index < diagnostic.hints().size(); ++index)
    {
        if (index != 0)
        {
            out_ << ',';
        }
        detail::write_json_string(out_, diagnostic.hints()[index]);
    }
    out_ << "],\"suggestions\":[";
    for (std::size_t index = 0; index < diagnostic.suggestions().size(); ++index)
    {
        if (index != 0)
        {
            out_ << ',';
        }
        const Suggestion &suggestion = diagnostic.suggestions()[index];
        out_ << "{\"location\":";
        write_location(suggestion.span);
        out_ << ",\"replacement\":";
        detail::write_json_string(out_, suggestion.replacement);
        out_ << ",\"message\":";
        detail::write_json_string(out_, suggestion.message);
        out_ << ",\"applicability\":";
        detail::write_json_string(out_, applicability_name(suggestion.applicability));
        out_ << '}';
    }
    out_ << "]}";
}

void JsonEmitter::flush()
{
    std::lock_guard<std::mutex> lock(mutex_);
    out_.flush();
}

void JsonEmitter::finish()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (finished_)
    {
        return;
    }
    if (!started_)
    {
        out_ << "[]\n";
    }
    else
    {
        out_ << (pretty_ ? "\n]\n" : "]\n");
    }
    started_ = false;
    finished_ = true;
    out_.flush();
}

void LspEmitter::emit(const Diagnostic &diagnostic)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const SourceSpan span = diagnostic.primary_span();
    const cora::core::FileID file_id =
        span.is_valid() ? span.file_id : cora::core::INVALID_FILE_ID;
    pending_[file_id].push_back(diagnostic);
    dirty_ = true;
}

void LspEmitter::write_notification(
    cora::core::FileID file_id, const std::vector<Diagnostic> &diagnostics)
{
    const cora::core::SourceFile *file = source_manager_.get_file(file_id);

    out_ << "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\","
            "\"params\":{\"uri\":";
    if (file != nullptr)
    {
        detail::write_json_string(out_, detail::file_uri(file->GetPath()));
    }
    else
    {
        detail::write_json_string(out_, "");
    }
    out_ << ",\"diagnostics\":[";

    for (std::size_t index = 0; index < diagnostics.size(); ++index)
    {
        if (index != 0)
        {
            out_ << ',';
        }
        const Diagnostic &diagnostic = diagnostics[index];
        const SourceSpan span = diagnostic.primary_span();

        std::uint32_t start_line = 0;
        std::uint32_t start_character = 0;
        std::uint32_t end_line = 0;
        std::uint32_t end_character = 0;
        if (span.is_valid())
        {
            if (const cora::core::SourceFile *span_file =
                    source_manager_.get_file(span.file_id))
            {
                const cora::core::LineCol start =
                    span_file->GetLineCol(span.start_offset);
                const cora::core::LineCol end =
                    span_file->GetLineCol(span.end_offset);
                start_line = start.line == 0 ? 0 : start.line - 1;
                start_character = start.column == 0 ? 0 : start.column - 1;
                end_line = end.line == 0 ? 0 : end.line - 1;
                end_character = end.column == 0 ? 0 : end.column - 1;
            }
        }

        out_ << "{\"range\":{\"start\":{\"line\":" << start_line
             << ",\"character\":" << start_character << "},\"end\":{\"line\":"
             << end_line << ",\"character\":" << end_character << "}}"
             << ",\"severity\":" << detail::lsp_severity(diagnostic.severity());
        if (diagnostic.has_code())
        {
            out_ << ",\"code\":";
            detail::write_json_string(out_, diagnostic.code());
        }
        out_ << ",\"source\":\"cora\",\"message\":";
        detail::write_json_string(out_, diagnostic.message());
        out_ << '}';
    }

    out_ << "]}}\n";
}

void LspEmitter::write_pending_locked()
{
    // Publish a cumulative snapshot per file. LSP clients replace the
    // previous diagnostic set for a URI, so earlier entries must be kept.
    for (const auto &entry : pending_)
    {
        write_notification(entry.first, entry.second);
    }
}

void LspEmitter::flush()
{
    std::lock_guard<std::mutex> lock(mutex_);
    write_pending_locked();
    dirty_ = false;
    out_.flush();
}

void LspEmitter::finish()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (dirty_)
    {
        write_pending_locked();
        dirty_ = false;
    }
    pending_.clear();
    out_.flush();
}

} // namespace cora::diag
