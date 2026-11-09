#ifndef CORA_DIAGNOSTIC_ENGINE_H
#define CORA_DIAGNOSTIC_ENGINE_H

#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>

namespace cora::diag
{

    enum class Phase : std::uint8_t
    {
        Init,
        Lexing,
        Parsing,
        AstValidate,
        HirLowering,
        Sema,
        MirLowering,
        MirAnalysis,
        Codegen,
    };

    struct DiagnosticOptions
    {
        bool show_carets = true;
        bool show_column = true;
        bool show_source_ranges = false;
        bool show_fixits = true;
        bool show_summary = true;
        bool color = true;
        bool verify_mode = false;
        unsigned error_limit = 20;
    };

    class DiagnosticEngine
    {
    public:
        // DiagnosticEngine(DiagnosticOptions opts,
        //                  DiagnosticConsumer *consumer,
        //                  InternPool &pool,
        //                  ExpansionTable &expansions);

        //     void addEmitter(std::shared_ptr<DiagnosticEmitter> emitter);
        //     void report(const Diagnostic &diag);

        //     // ── Phase management ────────────────────────────────────
        //     void enter_phase(Phase p);
        //     Phase current_phase() const noexcept { return phase_; }
        //     void set_phase_budget(unsigned b) noexcept
        //     {
        //         phase_budget_ = b;
        //         phase_budget_overridden_ = true;
        //     }
        //     bool should_suppress_cascade() const noexcept;

        //     // ── Level queries ───────────────────────────────────────
        //     DiagnosticLevel effective_level(DiagID id) const noexcept;

        //     // ── Suppression ─────────────────────────────────────────
        //     SuppressionStack &suppression() noexcept { return suppression_; }
        //     const SuppressionStack &suppression() const noexcept { return suppression_; }
        //     bool is_allowed(DiagID id) const noexcept;

        //     // ── Emission ────────────────────────────────────────────
        //     void emit(Diagnostic &d);
        //     void emit(Diagnostic &&d) { emit(d); }
        //     void emit_simple(DiagID id, Span span, std::uint32_t payload = 0);
        //     void emit_raw(DiagnosticLevel lvl, SourceLocation loc, const char *msg);

        //     // ── Deferred ────────────────────────────────────────────
        //     DeferredDiagPool &deferred() noexcept { return deferred_; }
        //     void flush_deferred();

        //     // ── Flag parsing (cold path) ────────────────────────────
        //     void set_level(DiagID id, DiagnosticLevel lvl) noexcept;
        //     void set_werror_override(DiagID id, int mode) noexcept; // 0/1/2
        //     void set_all_werror(bool on) noexcept { all_werror_ = on; }
        //     void set_wall(bool on) noexcept { wall_ = on; }
        //     void set_wextra(bool on) noexcept { wextra_ = on; }
        //     void set_pedantic(bool on) noexcept { pedantic_ = on; }
        //     void disable_phase(DiagPhase p) noexcept { phase_disabled_ |= (1u << static_cast<int>(p)); }
        //     void enable_phase(DiagPhase p) noexcept { phase_disabled_ &= ~(1u << static_cast<int>(p)); }

        //     // ── Counters / summary ──────────────────────────────────
        //     unsigned error_count() const noexcept { return error_count_.load(); }
        //     unsigned warning_count() const noexcept { return warning_count_.load(); }
        //     bool limit_hit() const noexcept { return limit_hit_.load(); }
        //     void finish();
        //     int exit_code() const noexcept { return error_count() > 0 ? 1 : 0; }

        //     // ── Accessors used by builder / renderer ────────────────
        //     const DiagnosticOptions &options() const noexcept { return opts_; }
        //     DiagnosticConsumer *consumer() const noexcept { return consumer_; }
        //     InternPool &pool() const noexcept { return pool_; }
        //     ExpansionTable &expansions() const noexcept { return expansions_; }
        //     SourceLocation current_decl_loc() const noexcept { return current_decl_loc_; }
        //     void set_current_decl_loc(SourceLocation l) noexcept { current_decl_loc_ = l; }

        //     void set_show_carets(bool on) noexcept { opts_.show_carets = on; }
        //     void set_show_column(bool on) noexcept { opts_.show_column = on; }
        //     void set_show_source_ranges(bool on) noexcept { opts_.show_source_ranges = on; }
        //     void set_show_fixits(bool on) noexcept { opts_.show_fixits = on; }
        //     void set_color(bool on) noexcept { opts_.color = on; }
        //     void set_show_summary(bool on) noexcept { opts_.show_summary = on; }

        // private:
        //     DiagnosticLevel default_level(DiagID id) const noexcept;
        //     DiagnosticLevel apply_werror(DiagID id, DiagnosticLevel lvl) const noexcept;
        //     void populate_name_ids() noexcept;

        //     DiagnosticOptions opts_;
        //     DiagnosticConsumer *consumer_;
        //     InternPool &pool_;
        //     ExpansionTable &expansions_;

        //     // 2 bits per diagnostic for level override; 2 for -Werror override
        //     static constexpr std::size_t StateBytes = (NumDiags + 3) / 4;
        //     std::array<std::uint8_t, StateBytes> level_state_{};
        //     std::array<std::uint8_t, StateBytes> err_override_{};
        //     std::array<std::uint32_t, NumDiags> name_ids_{};

        //     bool all_werror_ = false;
        //     bool wall_ = false;
        //     bool wextra_ = false;
        //     bool pedantic_ = false;
        //     std::uint8_t phase_disabled_ = 0;

        //     Phase phase_ = Phase::Init;
        //     std::atomic<unsigned> phase_errors_{0};
        //     unsigned phase_budget_ = 100;
        //     bool phase_budget_overridden_ = false;

        //     std::atomic<unsigned> error_count_{0};
        //     std::atomic<unsigned> warning_count_{0};
        //     std::atomic_bool limit_hit_{false};
        //     std::atomic_bool cascade_notice_emitted_{false};

        //     SuppressionStack suppression_;
        //     DeferredDiagPool deferred_;
        //     SourceLocation current_decl_loc_{};
        //     mutable std::recursive_mutex emit_mutex_;

        void emit_limit_notice();
    };

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_ENGINE_H
