#pragma once

#include "diag/diagnostic.h"
#include "diag/engine.h"
#include <string>
#include <string_view>

namespace diag
{

    /// Stack-only fluent builder. RAII: emits on destruction unless the
    /// diagnostic was explicitly suppressed (`suppress()`).
    class DiagnosticBuilder
    {
    public:
        DiagnosticBuilder(DiagnosticEngine &eng, DiagID id, SourceLocation loc)
            : engine_(eng), diag_(id, eng.effective_level(id), loc)
        {
            diag_.spans[0] = {Span(loc), "", true};
            diag_.num_spans = 1;
        }

        DiagnosticBuilder(const DiagnosticBuilder &) = delete;
        DiagnosticBuilder &operator=(const DiagnosticBuilder &) = delete;

        DiagnosticBuilder(DiagnosticBuilder &&o) noexcept
            : engine_(o.engine_), diag_(o.diag_), emitted_(o.emitted_)
        {
            o.emitted_ = true;
        }

        ~DiagnosticBuilder()
        {
            if (!emitted_)
                engine_.emit(diag_);
        }

        // ── Arguments ────────────────────────────────────────────
        DiagnosticBuilder &arg(const char *s) { return add(DiagArg::from_string(s)); }
        DiagnosticBuilder &arg(std::uint64_t u) { return add(DiagArg::from_unsigned(u)); }
        DiagnosticBuilder &arg(std::int64_t s) { return add(DiagArg::from_signed(s)); }
        DiagnosticBuilder &arg(int i) { return add(DiagArg::from_signed(i)); }
        DiagnosticBuilder &arg(char c) { return add(DiagArg::from_char(c)); }
        DiagnosticBuilder &arg_id(const char *ident) { return add(DiagArg::from_identifier(ident)); }
        DiagnosticBuilder &arg_interned(std::string_view value)
        {
            return add(DiagArg::from_string(engine_.pool().intern(value)));
        }
        DiagnosticBuilder &arg_identifier(std::string_view value)
        {
            return add(DiagArg::from_identifier(engine_.pool().intern(value)));
        }
        DiagnosticBuilder &arg_loc(SourceLocation loc) { return add(DiagArg::from_loc(loc)); }
        DiagnosticBuilder &arg_type(const void *t) { return add(DiagArg::from_type(t)); }

        DiagnosticBuilder &arg(const std::string &) = delete; // forbid temporaries

        // ── Spans ────────────────────────────────────────────────
        DiagnosticBuilder &span(Span s, const char *label = "", bool primary = false)
        {
            if (diag_.num_spans < MaxSpans)
                diag_.spans[diag_.num_spans++] = {s, label, primary};
            return *this;
        }
        DiagnosticBuilder &span(SourceRange r, const char *label = "", bool primary = false)
        {
            return span(Span(r), label, primary);
        }

        // ── Fix-its / suggestions ────────────────────────────────
        DiagnosticBuilder &fix(FixItHint h)
        {
            if (diag_.num_fixits < MaxFixIts)
                diag_.fixits[diag_.num_fixits++] = h;
            return *this;
        }

        DiagnosticBuilder &suggest(const char *msg, FixItHint h)
        {
            h.message = msg;
            return fix(h);
        }

        // ── Sub-notes ────────────────────────────────────────────
        DiagnosticBuilder &note(SourceLocation loc, const char *msg)
        {
            if (diag_.num_children < MaxChildren)
                diag_.children[diag_.num_children++] = {DiagnosticLevel::Note, loc, msg};
            return *this;
        }
        DiagnosticBuilder &help(const char *msg)
        {
            if (diag_.num_children < MaxChildren)
                diag_.children[diag_.num_children++] = {DiagnosticLevel::Help, {}, msg};
            return *this;
        }

        // ── Manual control ───────────────────────────────────────
        void suppress() noexcept { emitted_ = true; }
        Diagnostic &raw() noexcept { return diag_; }

    private:
        DiagnosticBuilder &add(DiagArg a)
        {
            if (diag_.num_args < MaxArgs)
                diag_.args[diag_.num_args++] = a;
            return *this;
        }

        DiagnosticEngine &engine_;
        Diagnostic diag_;
        bool emitted_ = false;
    };

} // namespace diag
