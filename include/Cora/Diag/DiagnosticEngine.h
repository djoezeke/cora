#ifndef CORA_DIAGNOSTIC_ENGINE_H
#define CORA_DIAGNOSTIC_ENGINE_H

#include <Cora/Diag/Diagnostic.h>
#include <Cora/Diag/DiagnosticBuilder.h>
#include <Cora/Diag/DiagnosticConfig.h>

#include <atomic>
#include <cstddef>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace cora::core
{
class SourceManager;
}

namespace cora::diag
{

class DiagnosticEmitter;

/// Thread-safe coordinator shared by all compiler worker threads.
///
/// Diagnostics are buffered and emitted on `flush()`/`finish()` so that
/// parallel workers never contend on the output stream. Multiple emitters can
/// be attached; each diagnostic is forwarded to every emitter in order.
class DiagnosticEngine
{
public:
    explicit DiagnosticEngine(DiagnosticOptions options = {});
    ~DiagnosticEngine() = default;

    DiagnosticEngine(const DiagnosticEngine &) = delete;
    DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;

    [[nodiscard]] DiagnosticConfig &config() noexcept { return config_; }
    [[nodiscard]] const DiagnosticConfig &config() const noexcept { return config_; }
    [[nodiscard]] const DiagnosticOptions &options() const noexcept
    {
        return config_.options();
    }
    [[nodiscard]] DiagnosticOptions &options() noexcept { return config_.options(); }

    // ── Zero-boilerplate builder factories ──────────────────────────────
    DiagnosticBuilder error(std::string_view message)
    {
        return make_generic(Severity::Error, message);
    }
    DiagnosticBuilder warning(std::string_view message)
    {
        return make_generic(Severity::Warning, message);
    }
    DiagnosticBuilder remark(std::string_view message)
    {
        return make_generic(Severity::Remark, message);
    }
    DiagnosticBuilder note(std::string_view message)
    {
        return make_generic(Severity::Note, message);
    }
    DiagnosticBuilder help(std::string_view message)
    {
        return make_generic(Severity::Help, message);
    }
    DiagnosticBuilder fatal(std::string_view message)
    {
        return make_generic(Severity::Fatal, message);
    }

    /// Build a diagnostic from the stable code table without formatting.
    DiagnosticBuilder diag(DiagID id);
    DiagnosticBuilder diag(DiagID id, SourceSpan span);

    /// Build and format a diagnostic from the stable code table.
    ///
    /// Formatting happens only when the diagnostic is enabled, which keeps
    /// filtered diagnostics allocation-free.
    template <typename... Args>
    DiagnosticBuilder emit(DiagID id, SourceSpan span, Args &&...args)
    {
        return emit_impl(id, span, std::forward<Args>(args)...);
    }

    template <typename First, typename... Rest,
              std::enable_if_t<
                  !std::is_same_v<std::decay_t<First>, SourceSpan>, int> = 0>
    DiagnosticBuilder emit(DiagID id, First &&first, Rest &&...rest)
    {
        return emit_impl(id, SourceSpan{}, std::forward<First>(first),
                         std::forward<Rest>(rest)...);
    }

    DiagnosticBuilder emit(DiagID id) { return diag(id); }

    // ── Policy queries ───────────────────────────────────────────────────
    [[nodiscard]] bool is_enabled(DiagID id) const noexcept;
    [[nodiscard]] bool is_enabled(Severity severity) const noexcept;

    // ── Submission / output ──────────────────────────────────────────────
    /// Thread-safe ingestion. Usually called by `DiagnosticBuilder::~...`.
    void report(Diagnostic diagnostic);

    void add_emitter(std::shared_ptr<DiagnosticEmitter> emitter);

    template <typename Emitter, typename... Args>
    std::shared_ptr<Emitter> emplace_emitter(Args &&...args)
    {
        auto emitter = std::make_shared<Emitter>(std::forward<Args>(args)...);
        add_emitter(emitter);
        return emitter;
    }

    /// Attach one of the built-in outputs. Additional outputs can be added by
    /// implementing `DiagnosticEmitter` and calling `add_emitter`.
    void add_output(DiagnosticFormat format, const cora::core::SourceManager &source_manager,
                    std::ostream &out);

    [[nodiscard]] std::size_t emitter_count() const;
    void clear_emitters();

    /// Emit all buffered diagnostics. Emitters remain open for another batch.
    void flush();

    /// Flush and finalize all emitters. Idempotent.
    void finish();

    // ── Counters / summary ───────────────────────────────────────────────
    [[nodiscard]] bool has_errors() const noexcept
    {
        return error_count_.load(std::memory_order_relaxed) > 0;
    }
    [[nodiscard]] std::size_t error_count() const noexcept
    {
        return error_count_.load(std::memory_order_relaxed);
    }
    [[nodiscard]] std::size_t warning_count() const noexcept
    {
        return warning_count_.load(std::memory_order_relaxed);
    }
    [[nodiscard]] std::size_t suppressed_error_count() const noexcept
    {
        return suppressed_error_count_.load(std::memory_order_relaxed);
    }
    [[nodiscard]] std::size_t reported_count() const noexcept
    {
        return reported_count_.load(std::memory_order_relaxed);
    }
    [[nodiscard]] std::size_t skipped_count() const noexcept
    {
        return skipped_count_.load(std::memory_order_relaxed);
    }
    [[nodiscard]] bool limit_hit() const noexcept
    {
        return limit_hit_.load(std::memory_order_relaxed);
    }
    [[nodiscard]] bool finished() const noexcept
    {
        return finished_.load(std::memory_order_relaxed);
    }
    [[nodiscard]] std::size_t buffered_count() const;
    [[nodiscard]] int exit_code() const noexcept { return has_errors() ? 1 : 0; }

private:
    DiagnosticBuilder make_generic(Severity severity, std::string_view message);
    DiagnosticBuilder make_builder(DiagID id, SourceSpan span,
                                   std::string_view message);

    template <typename... Args>
    DiagnosticBuilder emit_impl(DiagID id, SourceSpan span, Args &&...args)
    {
        if (!is_enabled(id))
        {
            return DiagnosticBuilder{};
        }
        return make_builder(id, span,
                            format_diag(id, std::forward<Args>(args)...));
    }

    [[nodiscard]] Diagnostic make_limit_notice() const;
    [[nodiscard]] std::vector<std::shared_ptr<DiagnosticEmitter>>
    emitter_snapshot() const;
    void flush_locked();

    DiagnosticConfig config_;

    mutable std::mutex buffer_mutex_;
    std::vector<Diagnostic> buffer_;

    mutable std::mutex emitter_mutex_;
    std::vector<std::shared_ptr<DiagnosticEmitter>> emitters_;

    std::mutex flush_mutex_;

    std::atomic<std::size_t> error_count_{0};
    std::atomic<std::size_t> warning_count_{0};
    std::atomic<std::size_t> suppressed_error_count_{0};
    std::atomic<std::size_t> reported_count_{0};
    std::atomic<std::size_t> skipped_count_{0};
    std::atomic<bool> limit_hit_{false};
    std::atomic<bool> limit_notice_added_{false};
    std::atomic<bool> finished_{false};
};

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_ENGINE_H
