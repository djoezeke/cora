#ifndef CORA_DIAGNOSTIC_BUILDER_H
#define CORA_DIAGNOSTIC_BUILDER_H

#include <Cora/Diag/Diagnostic.h>

#include <string>
#include <string_view>
#include <utility>

namespace cora::diag
{

class DiagnosticEngine;

/// Stack-only fluent builder with automatic RAII submission.
///
/// The builder is disabled when its diagnostic is filtered out. Disabled
/// builders do not allocate, copy messages, or touch the engine; this is the
/// foundation of the zero-cost diagnostic path.
///
/// Usage:
///     engine.emit(DiagID::warn_unused_variable, span, name)
///           .note("prefix it with '_' to silence this warning");
///
/// The diagnostic is submitted when the builder goes out of scope. Call
/// `emit()` to submit early, or `cancel()` to suppress it.
class DiagnosticBuilder
{
public:
    DiagnosticBuilder() = default;

    DiagnosticBuilder(DiagnosticEngine *engine, Severity severity,
                      std::string_view message, std::string_view code = {});
    DiagnosticBuilder(DiagnosticEngine *engine, Diagnostic diagnostic);

    ~DiagnosticBuilder();

    DiagnosticBuilder(const DiagnosticBuilder &) = delete;
    DiagnosticBuilder &operator=(const DiagnosticBuilder &) = delete;

    DiagnosticBuilder(DiagnosticBuilder &&other) noexcept;
    DiagnosticBuilder &operator=(DiagnosticBuilder &&other) noexcept;

    [[nodiscard]] bool active() const noexcept
    {
        return engine_ != nullptr && !cancelled_ && !emitted_;
    }
    explicit operator bool() const noexcept { return active(); }

    [[nodiscard]] const Diagnostic &diagnostic() const noexcept { return diagnostic_; }
    [[nodiscard]] Diagnostic &diagnostic() noexcept { return diagnostic_; }

    DiagnosticBuilder &code(std::string_view value)
    {
        if (active())
        {
            diagnostic_.set_code(value);
        }
        return *this;
    }

    DiagnosticBuilder &message(std::string_view value)
    {
        if (active())
        {
            diagnostic_.set_message(value);
        }
        return *this;
    }

    DiagnosticBuilder &primary(SourceSpan span, std::string_view label_message = {})
    {
        if (active() && span.is_valid())
        {
            diagnostic_.add_label(span, label_message, LabelStyle::Primary);
        }
        return *this;
    }

    DiagnosticBuilder &secondary(SourceSpan span, std::string_view label_message = {})
    {
        if (active() && span.is_valid())
        {
            diagnostic_.add_label(span, label_message, LabelStyle::Secondary);
        }
        return *this;
    }

    DiagnosticBuilder &label(SourceSpan span, std::string_view label_message = {},
                             LabelStyle style = LabelStyle::Primary)
    {
        if (active() && span.is_valid())
        {
            diagnostic_.add_label(span, label_message, style);
        }
        return *this;
    }

    DiagnosticBuilder &note(std::string_view note_message)
    {
        if (active())
        {
            diagnostic_.add_hint(note_message);
        }
        return *this;
    }

    DiagnosticBuilder &hint(std::string_view hint_message)
    {
        if (active())
        {
            diagnostic_.add_hint(hint_message);
        }
        return *this;
    }

    DiagnosticBuilder &help(std::string_view help_message)
    {
        if (active())
        {
            std::string text("help: ");
            text.append(help_message.data(), help_message.size());
            diagnostic_.add_hint(text);
        }
        return *this;
    }

    DiagnosticBuilder &suggest(
        SourceSpan span, std::string_view replacement, std::string_view message = {},
        Applicability applicability = Applicability::MachineApplicable)
    {
        if (active() && span.is_valid())
        {
            diagnostic_.add_suggestion(span, replacement, message, applicability);
        }
        return *this;
    }

    DiagnosticBuilder &fix(
        SourceSpan span, std::string_view replacement, std::string_view message = {},
        Applicability applicability = Applicability::MachineApplicable)
    {
        return suggest(span, replacement, message, applicability);
    }

    /// Cancel submission. Useful for speculative parsing/backtracking.
    void cancel() noexcept { cancelled_ = true; }
    void suppress() noexcept { cancel(); }

    /// Submit early. Safe to call multiple times; only the first call reports.
    void emit();

private:
    DiagnosticEngine *engine_ = nullptr;
    Diagnostic diagnostic_;
    bool cancelled_ = false;
    bool emitted_ = false;
};

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_BUILDER_H
