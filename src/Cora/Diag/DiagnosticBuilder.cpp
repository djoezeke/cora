#include <Cora/Diag/DiagnosticBuilder.h>

#include <Cora/Diag/DiagnosticEngine.h>

#include <utility>

namespace cora::diag
{

DiagnosticBuilder::DiagnosticBuilder(DiagnosticEngine *engine, Severity severity,
                                     std::string_view message, std::string_view code)
    : engine_(engine)
{
    if (engine_ == nullptr)
    {
        return;
    }
    diagnostic_.set_severity(severity);
    diagnostic_.set_severity_resolved(true);
    diagnostic_.set_message(message);
    if (!code.empty())
    {
        diagnostic_.set_code(code);
    }
}

DiagnosticBuilder::DiagnosticBuilder(DiagnosticEngine *engine, Diagnostic diagnostic)
    : engine_(engine), diagnostic_(std::move(diagnostic))
{
}

DiagnosticBuilder::~DiagnosticBuilder()
{
    emit();
}

DiagnosticBuilder::DiagnosticBuilder(DiagnosticBuilder &&other) noexcept
    : engine_(other.engine_),
      diagnostic_(std::move(other.diagnostic_)),
      cancelled_(other.cancelled_),
      emitted_(other.emitted_)
{
    other.engine_ = nullptr;
    other.emitted_ = true;
}

DiagnosticBuilder &DiagnosticBuilder::operator=(DiagnosticBuilder &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    emit();
    engine_ = other.engine_;
    diagnostic_ = std::move(other.diagnostic_);
    cancelled_ = other.cancelled_;
    emitted_ = other.emitted_;
    other.engine_ = nullptr;
    other.emitted_ = true;
    return *this;
}

void DiagnosticBuilder::emit()
{
    if (engine_ == nullptr || cancelled_ || emitted_)
    {
        return;
    }

    emitted_ = true;
    DiagnosticEngine *engine = engine_;
    engine_ = nullptr;
    engine->report(std::move(diagnostic_));
}

} // namespace cora::diag
