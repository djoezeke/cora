#ifndef CORA_DIAGNOSTIC_DIAGNOSTIC_H
#define CORA_DIAGNOSTIC_DIAGNOSTIC_H

#include <Cora/Core/SourceLocation.h>

// #include "diag/diag_arg.h"
// #include "diag/diagid.h"
// #include "diag/fixit.h"
// #include "diag/span.h"

#include <array>
#include <cstdint>

namespace cora::diag
{

    inline constexpr std::size_t MaxArgs = 4;
    inline constexpr std::size_t MaxSpans = 3;
    inline constexpr std::size_t MaxFixIts = 2;
    inline constexpr std::size_t MaxChildren = 3;

    struct LabeledSpan
    {
        Span span{};
        const char *label = "";
        bool primary = false;
    };

    struct SubNote
    {
        DiagnosticLevel level = DiagnosticLevel::Note;
        SourceLocation loc{};
        const char *msg = "";
    };

    struct Diagnostic
    {
        DiagID id = DiagID::anonymous;
        DiagnosticLevel level = DiagnosticLevel::Error;
        SourceLocation loc{};
        std::uint8_t num_args = 0;
        std::uint8_t num_spans = 0;
        std::uint8_t num_fixits = 0;
        std::uint8_t num_children = 0;

        const char *raw_message = nullptr; // if set, overrides DiagTable[id].format
        std::uint32_t raw_message_len = 0;

        std::array<DiagArg, MaxArgs> args{};
        std::array<LabeledSpan, MaxSpans> spans{};
        std::array<FixItHint, MaxFixIts> fixits{};
        std::array<SubNote, MaxChildren> children{};

        Diagnostic(DiagID i, DiagnosticLevel l, SourceLocation s) : id(i), level(l), loc(s) {}
    };

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_DIAGNOSTIC_H
