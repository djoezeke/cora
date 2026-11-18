#ifndef CORA_DIAGNOSTIC_DIAGNOSTIC_H
#define CORA_DIAGNOSTIC_DIAGNOSTIC_H

#include <Cora/Core/SourceLocation.h>

#include <cstdint>
#include <string_view>
#include <vector>

#include <string>
// #include <utility>

namespace cora::diag
{

using SourceSpan = cora::core::SourceSpan;

/// Severity of a diagnostic. The ordering is intentional: values at or above
/// `Warning` participate in warning/error accounting.
enum class Severity : std::uint8_t
{
    Ignored,
    Remark,
    Note,
    Help,
    Warning,
    Error,
    Fatal,
};

[[nodiscard]] constexpr bool is_error_severity(Severity severity) noexcept
{
    return severity == Severity::Error || severity == Severity::Fatal;
}

[[nodiscard]] constexpr bool is_warning_severity(Severity severity) noexcept
{
    return severity == Severity::Warning;
}

[[nodiscard]] constexpr const char *severity_name(Severity severity) noexcept
{
    switch (severity)
    {
    case Severity::Ignored:
        return "ignored";
    case Severity::Remark:
        return "remark";
    case Severity::Note:
        return "note";
    case Severity::Help:
        return "help";
    case Severity::Warning:
        return "warning";
    case Severity::Error:
        return "error";
    case Severity::Fatal:
        return "fatal error";
    }
    return "unknown";
}

/// How a source label is rendered.
enum class LabelStyle
{
    Primary,  ///< Rendered with ^^^
    Secondary ///< Rendered with ---
};

[[nodiscard]] constexpr const char *label_style_name(LabelStyle style) noexcept
{
    return style == LabelStyle::Primary ? "primary" : "secondary";
}

/// Whether a machine-applicable fix can be applied automatically.
enum class Applicability
{
    MachineApplicable, ///< Safe to auto-apply (e.g. cora fix / IDE quick-fix)
    MaybeIncorrect,    ///< Likely right, requires human review
    HasPlaceholders    ///< Contains template placeholders like `<expr>`
};

[[nodiscard]] constexpr const char *applicability_name(Applicability applicability) noexcept
{
    switch (applicability)
    {
    case Applicability::MachineApplicable:
        return "machine-applicable";
    case Applicability::MaybeIncorrect:
        return "maybe-incorrect";
    case Applicability::HasPlaceholders:
        return "has-placeholders";
    }
    return "unknown";
}

struct Label
{
    SourceSpan span{};
    std::string message;
    LabelStyle style = LabelStyle::Primary;

    Label() = default;
    Label(SourceSpan source_span, std::string label_message = {},
          LabelStyle label_style = LabelStyle::Primary)
        : span(source_span), message(std::move(label_message)), style(label_style)
    {
    }
};

struct Suggestion
{
    SourceSpan span{};
    std::string replacement;
    std::string message;
    Applicability applicability = Applicability::MachineApplicable;

    Suggestion() = default;
    Suggestion(SourceSpan source_span, std::string replacement_text,
               std::string suggestion_message = {},
               Applicability suggestion_applicability = Applicability::MachineApplicable)
        : span(source_span),
          replacement(std::move(replacement_text)),
          message(std::move(suggestion_message)),
          applicability(suggestion_applicability)
    {
    }
};

/// A fully materialized diagnostic.
///
/// `Diagnostic` is a plain value type. It is intentionally independent of the
/// engine and of any output format so that emitters can be plugged in without
/// touching the compiler front-end.
class Diagnostic
{
public:
    Diagnostic() = default;
    Diagnostic(Severity severity, std::string_view message, std::string_view code = {});

    static Diagnostic error(std::string_view message, std::string_view code = {});
    static Diagnostic warning(std::string_view message, std::string_view code = {});
    static Diagnostic remark(std::string_view message, std::string_view code = {});
    static Diagnostic note(std::string_view message, std::string_view code = {});
    static Diagnostic help(std::string_view message, std::string_view code = {});
    static Diagnostic fatal(std::string_view message, std::string_view code = {});

    [[nodiscard]] Severity severity() const noexcept { return severity_; }
    [[nodiscard]] const std::string &code() const noexcept { return code_; }
    [[nodiscard]] const std::string &message() const noexcept { return message_; }
    [[nodiscard]] const std::vector<Label> &labels() const noexcept { return labels_; }
    [[nodiscard]] const std::vector<std::string> &hints() const noexcept { return hints_; }
    [[nodiscard]] const std::vector<std::string> &notes() const noexcept { return hints_; }
    [[nodiscard]] const std::vector<Suggestion> &suggestions() const noexcept
    {
        return suggestions_;
    }

    [[nodiscard]] bool has_code() const noexcept { return !code_.empty(); }
    [[nodiscard]] bool is_error() const noexcept { return is_error_severity(severity_); }
    [[nodiscard]] bool is_warning() const noexcept
    {
        return is_warning_severity(severity_);
    }

    /// True when the engine/builder already applied -Werror policy to this
    /// diagnostic. Directly reported diagnostics start unresolved.
    [[nodiscard]] bool severity_resolved() const noexcept
    {
        return severity_resolved_;
    }
    void set_severity_resolved(bool resolved) noexcept
    {
        severity_resolved_ = resolved;
    }

    /// The first primary label, or the first label if no primary label exists.
    [[nodiscard]] SourceSpan primary_span() const noexcept;
    [[nodiscard]] SourceSpan span() const noexcept { return span_; }
    [[nodiscard]] std::uint64_t revision() const noexcept { return revision_; }
    void set_revision(std::uint64_t revision) noexcept { revision_ = revision; }
    [[nodiscard]] std::string identity() const;
    void set_span(SourceSpan span) noexcept { span_ = span; }

    void set_severity(Severity severity) noexcept { severity_ = severity; }
    void set_code(std::string_view code);
    void set_message(std::string_view message);

    void add_label(Label label);
    void add_label(SourceSpan span, std::string_view message = {},
                   LabelStyle style = LabelStyle::Primary);
    void add_hint(std::string_view hint);
    void add_note(std::string_view note) { add_hint(note); }
    void add_suggestion(Suggestion suggestion);
    void add_suggestion(SourceSpan span, std::string_view replacement,
                        std::string_view message = {},
                        Applicability applicability = Applicability::MachineApplicable);

    void clear();

    Diagnostic &with_code(std::string_view code)
    {
        set_code(code);
        return *this;
    }

    Diagnostic &with_message(std::string_view message)
    {
        set_message(message);
        return *this;
    }

    Diagnostic &with_span(SourceSpan span)
    {
        set_span(span);
        return *this;
    }

    Diagnostic &with_primary(SourceSpan span, std::string_view label_message = {})
    {
        add_label(span, label_message, LabelStyle::Primary);
        return *this;
    }

    Diagnostic &with_secondary(SourceSpan span, std::string_view label_message = {})
    {
        add_label(span, label_message, LabelStyle::Secondary);
        return *this;
    }

    Diagnostic &with_label(SourceSpan span, std::string_view label_message = {},
                           LabelStyle style = LabelStyle::Primary)
    {
        add_label(span, label_message, style);
        return *this;
    }

    Diagnostic &with_hint(std::string_view hint)
    {
        add_hint(hint);
        return *this;
    }

    Diagnostic &with_note(std::string_view note)
    {
        add_hint(note);
        return *this;
    }

    Diagnostic &with_suggestion(
        SourceSpan span, std::string_view replacement,
        std::string_view message = {},
        Applicability applicability = Applicability::MachineApplicable)
    {
        add_suggestion(span, replacement, message, applicability);
        return *this;
    }

private:
    Severity severity_ = Severity::Error;
    bool severity_resolved_ = false;
    std::uint64_t revision_ = 0;
    SourceSpan span_{};
    std::string code_;
    std::string message_;
    std::vector<Label> labels_;
    std::vector<std::string> hints_;
    std::vector<Suggestion> suggestions_;
};

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_DIAGNOSTIC_H
