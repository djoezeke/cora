#include <Cora/Diag/Diagnostic.h>

#include <utility>

namespace cora::diag
{

Diagnostic::Diagnostic(Severity severity, std::string_view message, std::string_view code)
    : severity_(severity)
{
    set_code(code);
    set_message(message);
}

Diagnostic Diagnostic::error(std::string_view message, std::string_view code)
{
    return Diagnostic(Severity::Error, message, code);
}

Diagnostic Diagnostic::warning(std::string_view message, std::string_view code)
{
    return Diagnostic(Severity::Warning, message, code);
}

Diagnostic Diagnostic::remark(std::string_view message, std::string_view code)
{
    return Diagnostic(Severity::Remark, message, code);
}

Diagnostic Diagnostic::note(std::string_view message, std::string_view code)
{
    return Diagnostic(Severity::Note, message, code);
}

Diagnostic Diagnostic::help(std::string_view message, std::string_view code)
{
    return Diagnostic(Severity::Help, message, code);
}

Diagnostic Diagnostic::fatal(std::string_view message, std::string_view code)
{
    return Diagnostic(Severity::Fatal, message, code);
}

SourceSpan Diagnostic::primary_span() const noexcept
{
    if (span_.is_valid())
    {
        return span_;
    }
    for (const auto &label : labels_)
    {
        if (label.style == LabelStyle::Primary)
        {
            return label.span;
        }
    }
    if (!labels_.empty())
    {
        return labels_.front().span;
    }
    return {};
}

void Diagnostic::set_code(std::string_view code)
{
    if (code.empty())
    {
        code_.clear();
        return;
    }
    code_.assign(code.data(), code.size());
}

void Diagnostic::set_message(std::string_view message)
{
    if (message.empty())
    {
        message_.clear();
        return;
    }
    message_.assign(message.data(), message.size());
}

void Diagnostic::add_label(Label label)
{
    if (label.style == LabelStyle::Primary && !span_.is_valid())
    {
        span_ = label.span;
    }
    labels_.push_back(std::move(label));
}

void Diagnostic::add_label(SourceSpan span, std::string_view message, LabelStyle style)
{
    Label label(span, {}, style);
    if (!message.empty())
    {
        label.message.assign(message.data(), message.size());
    }
    add_label(std::move(label));
}

void Diagnostic::add_hint(std::string_view hint)
{
    hints_.emplace_back();
    if (!hint.empty())
    {
        hints_.back().assign(hint.data(), hint.size());
    }
}

void Diagnostic::add_suggestion(Suggestion suggestion)
{
    suggestions_.push_back(std::move(suggestion));
}

void Diagnostic::add_suggestion(SourceSpan span, std::string_view replacement,
                                std::string_view message, Applicability applicability)
{
    Suggestion suggestion(span, {}, {}, applicability);
    if (!replacement.empty())
    {
        suggestion.replacement.assign(replacement.data(), replacement.size());
    }
    if (!message.empty())
    {
        suggestion.message.assign(message.data(), message.size());
    }
    suggestions_.push_back(std::move(suggestion));
}

void Diagnostic::clear()
{
    severity_ = Severity::Error;
    severity_resolved_ = false;
    span_ = {};
    code_.clear();
    message_.clear();
    labels_.clear();
    hints_.clear();
    suggestions_.clear();
}

} // namespace cora::diag
