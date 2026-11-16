#include <Cora/Diag/DiagnosticEngine.h>

#include <Cora/Diag/DiagnosticEmitter.h>

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string>
#include <utility>

namespace cora::diag
{

DiagnosticEngine::DiagnosticEngine(DiagnosticOptions options)
    : config_(std::move(options))
{
    buffer_.reserve(64);
}

DiagnosticBuilder DiagnosticEngine::diag(DiagID id)
{
    if (!is_valid_diag_id(id))
    {
        return DiagnosticBuilder{};
    }
    return make_builder(id, SourceSpan{}, diag_info(id).format);
}

DiagnosticBuilder DiagnosticEngine::diag(DiagID id, SourceSpan span)
{
    if (!is_valid_diag_id(id))
    {
        return DiagnosticBuilder{};
    }
    return make_builder(id, span, diag_info(id).format);
}

bool DiagnosticEngine::is_enabled(DiagID id) const noexcept
{
    if (!is_valid_diag_id(id))
    {
        return false;
    }

    const Severity severity = config_.effective_level(id);
    if (severity == Severity::Ignored)
    {
        return false;
    }
    if (is_error_severity(severity) && config_.error_limit() != 0 &&
        limit_hit_.load(std::memory_order_relaxed))
    {
        return false;
    }
    return true;
}

bool DiagnosticEngine::is_enabled(Severity severity) const noexcept
{
    const Severity effective = config_.effective_level(severity);
    if (effective == Severity::Ignored)
    {
        return false;
    }
    if (is_error_severity(effective) && config_.error_limit() != 0 &&
        limit_hit_.load(std::memory_order_relaxed))
    {
        return false;
    }
    return true;
}

DiagnosticBuilder DiagnosticEngine::make_generic(Severity severity,
                                                 std::string_view message)
{
    if (!is_enabled(severity))
    {
        return DiagnosticBuilder{};
    }

    if (severity == Severity::Warning)
    {
        severity = config_.effective_level(severity);
    }
    return DiagnosticBuilder(this, severity, message);
}

DiagnosticBuilder DiagnosticEngine::make_builder(DiagID id, SourceSpan span,
                                                 std::string_view message)
{
    if (!is_valid_diag_id(id))
    {
        return DiagnosticBuilder{};
    }

    const Severity severity = config_.effective_level(id);
    if (severity == Severity::Ignored)
    {
        return DiagnosticBuilder{};
    }
    if (is_error_severity(severity) && config_.error_limit() != 0 &&
        limit_hit_.load(std::memory_order_relaxed))
    {
        return DiagnosticBuilder{};
    }

    const DiagInfo &info = diag_info(id);
    DiagnosticBuilder builder(this, severity, message, info.code);
    if (span.is_valid())
    {
        builder.primary(span);
    }
    return builder;
}

void DiagnosticEngine::report(Diagnostic diagnostic)
{
    Severity severity = diagnostic.severity();
    if (severity == Severity::Ignored)
    {
        return;
    }

    if (severity == Severity::Warning)
    {
        if (!diagnostic.severity_resolved())
        {
            bool policy_applied = false;
            if (diagnostic.has_code())
            {
                if (const auto id = lookup_warning(diagnostic.code()))
                {
                    severity = config_.effective_level(*id);
                    diagnostic.set_severity(severity);
                    policy_applied = true;
                }
            }

            if (!policy_applied)
            {
                if (config_.suppress_warnings())
                {
                    skipped_count_.fetch_add(1, std::memory_order_relaxed);
                    return;
                }
                if (config_.all_werror())
                {
                    diagnostic.set_severity(Severity::Error);
                    severity = Severity::Error;
                }
            }

            if (severity == Severity::Ignored)
            {
                skipped_count_.fetch_add(1, std::memory_order_relaxed);
                return;
            }
        }
        else if (config_.suppress_warnings())
        {
            skipped_count_.fetch_add(1, std::memory_order_relaxed);
            return;
        }
    }

    bool over_limit = false;
    bool add_limit_notice = false;

    if (is_error_severity(severity))
    {
        const std::size_t limit = config_.error_limit();
        if (limit != 0 && limit_hit_.load(std::memory_order_relaxed))
        {
            suppressed_error_count_.fetch_add(1, std::memory_order_relaxed);
            return;
        }

        const std::size_t count =
            error_count_.fetch_add(1, std::memory_order_relaxed) + 1;
        if (limit != 0 && count > limit)
        {
            over_limit = true;
            suppressed_error_count_.fetch_add(1, std::memory_order_relaxed);
            if (!limit_notice_added_.exchange(true, std::memory_order_relaxed))
            {
                add_limit_notice = true;
            }
            else
            {
                return;
            }
        }
        else if (limit != 0 && count == limit)
        {
            limit_hit_.store(true, std::memory_order_relaxed);
            if (!limit_notice_added_.exchange(true, std::memory_order_relaxed))
            {
                add_limit_notice = true;
            }
        }
    }
    else if (severity == Severity::Warning)
    {
        warning_count_.fetch_add(1, std::memory_order_relaxed);
    }

    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        if (!over_limit)
        {
            buffer_.push_back(std::move(diagnostic));
            reported_count_.fetch_add(1, std::memory_order_relaxed);
        }
        if (add_limit_notice)
        {
            buffer_.push_back(make_limit_notice());
        }
    }
}

Diagnostic DiagnosticEngine::make_limit_notice() const
{
    const DiagInfo &info = diag_info(DiagID::err_too_many_errors);
    Diagnostic notice(Severity::Fatal, info.format, info.code);
    notice.add_hint("use -ferror-limit=0 to remove the limit");
    return notice;
}

void DiagnosticEngine::add_emitter(std::shared_ptr<DiagnosticEmitter> emitter)
{
    if (!emitter)
    {
        return;
    }
    std::lock_guard<std::mutex> lock(emitter_mutex_);
    emitters_.push_back(std::move(emitter));
}

void DiagnosticEngine::add_output(DiagnosticFormat format,
                                  const cora::core::SourceManager &source_manager,
                                  std::ostream &out)
{
    const DiagnosticOptions &options = config_.options();
    switch (format)
    {
    case DiagnosticFormat::Terminal:
        add_emitter(std::make_shared<TextEmitter>(
            source_manager, out, options.color, options.show_carets,
            options.show_column, options.show_source_ranges, options.show_fixits,
            options.show_summary, options.show_option, options.absolute_paths));
        break;
    case DiagnosticFormat::Json:
        add_emitter(std::make_shared<JsonEmitter>(out, &source_manager));
        break;
    case DiagnosticFormat::Lsp:
        add_emitter(std::make_shared<LspEmitter>(source_manager, out));
        break;
    }
}

std::size_t DiagnosticEngine::emitter_count() const
{
    std::lock_guard<std::mutex> lock(emitter_mutex_);
    return emitters_.size();
}

void DiagnosticEngine::clear_emitters()
{
    std::lock_guard<std::mutex> lock(emitter_mutex_);
    emitters_.clear();
}

std::vector<std::shared_ptr<DiagnosticEmitter>>
DiagnosticEngine::emitter_snapshot() const
{
    std::lock_guard<std::mutex> lock(emitter_mutex_);
    return emitters_;
}

std::size_t DiagnosticEngine::buffered_count() const
{
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    return buffer_.size();
}

void DiagnosticEngine::flush_locked()
{
    std::vector<Diagnostic> to_emit;
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        to_emit.swap(buffer_);
    }

    if (config_.options().sort_on_flush)
    {
        std::stable_sort(
            to_emit.begin(), to_emit.end(),
            [](const Diagnostic &left, const Diagnostic &right)
            {
                const SourceSpan left_span = left.primary_span();
                const SourceSpan right_span = right.primary_span();

                // Diagnostics without a source location (for example the
                // "too many errors" notice) belong at the end of the output.
                const bool left_located = left_span.is_valid();
                const bool right_located = right_span.is_valid();
                if (left_located != right_located)
                {
                    return left_located;
                }
                if (left_located)
                {
                    if (left_span.file_id != right_span.file_id)
                    {
                        return left_span.file_id < right_span.file_id;
                    }
                    if (left_span.start_offset != right_span.start_offset)
                    {
                        return left_span.start_offset < right_span.start_offset;
                    }
                    if (left_span.end_offset != right_span.end_offset)
                    {
                        return left_span.end_offset < right_span.end_offset;
                    }
                }
                if (left.severity() != right.severity())
                {
                    return static_cast<std::uint8_t>(left.severity()) <
                           static_cast<std::uint8_t>(right.severity());
                }
                if (left.message() != right.message())
                {
                    return left.message() < right.message();
                }
                return left.code() < right.code();
            });
    }

    const auto emitters = emitter_snapshot();
    for (const Diagnostic &diagnostic : to_emit)
    {
        for (const auto &emitter : emitters)
        {
            emitter->emit(diagnostic);
        }
    }
    for (const auto &emitter : emitters)
    {
        emitter->flush();
    }
}

void DiagnosticEngine::flush()
{
    std::lock_guard<std::mutex> lock(flush_mutex_);
    flush_locked();
}

void DiagnosticEngine::finish()
{
    std::lock_guard<std::mutex> lock(flush_mutex_);
    if (finished_.exchange(true, std::memory_order_relaxed))
    {
        return;
    }

    flush_locked();
    const auto emitters = emitter_snapshot();
    for (const auto &emitter : emitters)
    {
        emitter->finish();
    }
}

} // namespace cora::diag
