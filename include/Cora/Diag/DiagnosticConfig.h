#ifndef CORA_DIAGNOSTIC_CONFIG_H
#define CORA_DIAGNOSTIC_CONFIG_H

#include <Cora/Diag/DiagnosticCodes.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace cora::diag
{

/// Output format selected by `-fdiagnostics-format=...`.
enum class DiagnosticFormat : std::uint8_t
{
    Terminal,
    Json,
    Lsp,
};

/// User-facing diagnostic options. Display options are read-only while
/// compilation is running; warning/error controls live in `DiagnosticConfig`
/// and use atomics so they may be queried from worker threads.
struct DiagnosticOptions
{
    std::size_t error_limit = 20;     ///< 0 means unlimited.
    bool warnings_as_errors = false;  ///< -Werror
    bool suppress_warnings = false;   ///< -w
    bool sort_on_flush = true;        ///< Deterministic output order.
    bool show_carets = true;
    bool show_column = true;
    bool show_source_ranges = false;
    bool show_fixits = true;
    bool show_summary = true;
    bool show_option = true;
    bool color = true;
    bool verify_mode = false;         ///< Reserved for diagnostic verification harnesses.
    bool fix = false;                 ///< -fix / -fixit; suggestions are kept for the driver.
    bool absolute_paths = false;      ///< Print absolute source paths.
    DiagnosticFormat format = DiagnosticFormat::Terminal;
};

/// Thread-safe warning/error configuration.
///
/// The hot path (`effective_level`) performs only a few relaxed atomic loads.
/// The cold path (flag parsing) mutates the atomics before compilation starts.
class DiagnosticConfig
{
public:
    explicit DiagnosticConfig(DiagnosticOptions options = {})
        : options_(options),
          wall_(false),
          wextra_(false),
          pedantic_(false),
          all_werror_(options.warnings_as_errors),
          suppress_warnings_(options.suppress_warnings),
          error_limit_(options.error_limit)
    {
        for (auto &state : level_override_)
        {
            state.store(0, std::memory_order_relaxed);
        }
        for (auto &state : werror_override_)
        {
            state.store(-1, std::memory_order_relaxed);
        }
    }

    DiagnosticConfig(const DiagnosticConfig &) = delete;
    DiagnosticConfig &operator=(const DiagnosticConfig &) = delete;
    DiagnosticConfig(DiagnosticConfig &&) = delete;
    DiagnosticConfig &operator=(DiagnosticConfig &&) = delete;

    [[nodiscard]] const DiagnosticOptions &options() const noexcept { return options_; }
    [[nodiscard]] DiagnosticOptions &options() noexcept { return options_; }

    /// Resolve the effective severity of a diagnostic, applying -Wall/-Wextra/
    /// -Wpedantic, -Wno-..., -Werror and -w.
    [[nodiscard]] Severity effective_level(DiagID id) const noexcept
    {
        const auto index = static_cast<std::size_t>(id);
        if (!is_valid_diag_id(id))
        {
            return Severity::Ignored;
        }

        const std::uint8_t override_state =
            level_override_[index].load(std::memory_order_relaxed);
        if (override_state != 0)
        {
            const Severity override_level =
                static_cast<Severity>(override_state - 1);
            if (override_level == Severity::Ignored)
            {
                return Severity::Ignored;
            }
            return apply_warning_policy(index, override_level);
        }

        Severity severity = Severity::Ignored;
        switch (DiagTable[index].default_level)
        {
        case DiagDefault::Error:
            severity = Severity::Error;
            break;
        case DiagDefault::Warning:
            severity = Severity::Warning;
            break;
        case DiagDefault::WarningAll:
            severity = wall_.load(std::memory_order_relaxed) ? Severity::Warning
                                                             : Severity::Ignored;
            break;
        case DiagDefault::WarningExtra:
            severity = wextra_.load(std::memory_order_relaxed) ? Severity::Warning
                                                               : Severity::Ignored;
            break;
        case DiagDefault::Pedantic:
            severity = pedantic_.load(std::memory_order_relaxed) ? Severity::Warning
                                                                 : Severity::Ignored;
            break;
        case DiagDefault::Off:
            severity = Severity::Ignored;
            break;
        }

        return apply_warning_policy(index, severity);
    }

    /// Resolve a generic warning/note that has no `DiagID`.
    [[nodiscard]] Severity effective_level(Severity base) const noexcept
    {
        if (base == Severity::Warning)
        {
            if (suppress_warnings_.load(std::memory_order_relaxed))
            {
                return Severity::Ignored;
            }
            if (all_werror_.load(std::memory_order_relaxed))
            {
                return Severity::Error;
            }
        }
        return base;
    }

    [[nodiscard]] bool is_enabled(DiagID id) const noexcept
    {
        return effective_level(id) != Severity::Ignored;
    }

    void set_level(DiagID id, Severity level) noexcept
    {
        if (is_valid_diag_id(id))
        {
            level_override_[static_cast<std::size_t>(id)].store(
                static_cast<std::uint8_t>(level) + 1,
                std::memory_order_relaxed);
        }
    }

    void reset_level(DiagID id) noexcept
    {
        if (is_valid_diag_id(id))
        {
            level_override_[static_cast<std::size_t>(id)].store(
                0, std::memory_order_relaxed);
        }
    }

    void enable_warning(DiagID id) noexcept
    {
        if (is_valid_diag_id(id) &&
            DiagTable[static_cast<std::size_t>(id)].default_level !=
                DiagDefault::Error)
        {
            set_level(id, Severity::Warning);
        }
    }

    void disable_warning(DiagID id) noexcept { set_level(id, Severity::Ignored); }

    void set_werror(DiagID id, bool enabled) noexcept
    {
        if (is_valid_diag_id(id))
        {
            werror_override_[static_cast<std::size_t>(id)].store(
                enabled ? 1 : 0, std::memory_order_relaxed);
        }
    }

    void reset_werror(DiagID id) noexcept
    {
        if (is_valid_diag_id(id))
        {
            werror_override_[static_cast<std::size_t>(id)].store(
                -1, std::memory_order_relaxed);
        }
    }

    void set_all_werror(bool enabled) noexcept
    {
        all_werror_.store(enabled, std::memory_order_relaxed);
        options_.warnings_as_errors = enabled;
    }

    [[nodiscard]] bool all_werror() const noexcept
    {
        return all_werror_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] bool warnings_as_errors() const noexcept { return all_werror(); }
    void set_warnings_as_errors(bool enabled) noexcept { set_all_werror(enabled); }

    void set_wall(bool enabled) noexcept { wall_.store(enabled, std::memory_order_relaxed); }
    [[nodiscard]] bool wall() const noexcept
    {
        return wall_.load(std::memory_order_relaxed);
    }

    void set_wextra(bool enabled) noexcept
    {
        wextra_.store(enabled, std::memory_order_relaxed);
    }
    [[nodiscard]] bool wextra() const noexcept
    {
        return wextra_.load(std::memory_order_relaxed);
    }

    void set_pedantic(bool enabled) noexcept
    {
        pedantic_.store(enabled, std::memory_order_relaxed);
    }
    [[nodiscard]] bool pedantic() const noexcept
    {
        return pedantic_.load(std::memory_order_relaxed);
    }

    void set_suppress_warnings(bool enabled) noexcept
    {
        suppress_warnings_.store(enabled, std::memory_order_relaxed);
        options_.suppress_warnings = enabled;
    }
    [[nodiscard]] bool suppress_warnings() const noexcept
    {
        return suppress_warnings_.load(std::memory_order_relaxed);
    }

    void set_error_limit(std::size_t limit) noexcept
    {
        error_limit_.store(limit, std::memory_order_relaxed);
        options_.error_limit = limit;
    }
    [[nodiscard]] std::size_t error_limit() const noexcept
    {
        return error_limit_.load(std::memory_order_relaxed);
    }

private:
    [[nodiscard]] Severity apply_warning_policy(std::size_t index,
                                                Severity severity) const noexcept
    {
        if (severity != Severity::Warning)
        {
            return severity;
        }
        if (suppress_warnings_.load(std::memory_order_relaxed))
        {
            return Severity::Ignored;
        }

        const std::int8_t override_state =
            werror_override_[index].load(std::memory_order_relaxed);
        if (override_state == 1)
        {
            return Severity::Error;
        }
        if (override_state == 0)
        {
            return Severity::Warning;
        }
        return all_werror_.load(std::memory_order_relaxed) ? Severity::Error
                                                          : Severity::Warning;
    }

    DiagnosticOptions options_;

    std::array<std::atomic<std::uint8_t>, NumDiags> level_override_{};
    std::array<std::atomic<std::int8_t>, NumDiags> werror_override_{};

    std::atomic<bool> wall_;
    std::atomic<bool> wextra_;
    std::atomic<bool> pedantic_;
    std::atomic<bool> all_werror_;
    std::atomic<bool> suppress_warnings_;
    std::atomic<std::size_t> error_limit_;
};

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_CONFIG_H
