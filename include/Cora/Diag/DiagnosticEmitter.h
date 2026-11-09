#ifndef CORA_DIAGNOSTIC_EMITTER_H
#define CORA_DIAGNOSTIC_EMITTER_H

#include <Cora/Diag/Diagnostic.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace cora::diag
{

    class DiagnosticEmitter
    {
    public:
        const SourceManager &sourceManager;

    public:
        virtual ~DiagnosticEmitter() = default;
    };

    class TextEmitter final : public DiagnosticEmitter
    {
    };

    class JsonEmitter final : public DiagnosticEmitter
    {
    };

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_EMITTER_H
