#ifndef CORA_DIAGNOSTIC_CODES_H
#define CORA_DIAGNOSTIC_CODES_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

namespace cora::diag
{

    enum class DiagnosticLevel : std::uint8_t
    {
        Ignored,
        Remark,
        Note,
        Help,
        Warning,
        Error,
        Fatal,
    };

    enum class DiagPhase : std::uint8_t
    {
        Lexer,
        Parser,
        Sema,
        Codegen,
    };

    enum class DiagDefault : std::uint8_t
    {
        Error,
        Warning,
        WarningAll,
        WarningExtra,
        Pedantic,
        Off,
    };

    /// Every diagnostic in the compiler. Keep sorted per-phase for readability.
    /// IDs are stable across builds (do not reorder).
    enum class DiagID : std::uint16_t
    {
        // ── Lexer ─────────────────────────────────────────────
        err_unterminated_string,
        err_unterminated_char,
        err_invalid_char,
        err_invalid_utf8,

        warn_unknown_escape,
        warn_trigraph_ignored,
        warn_multiline_comment,
        warn_digit_separator,

        // ── Parser ────────────────────────────────────────────
        err_expected_token,
        err_unexpected_eof,
        err_unclosed_delimiter,
        err_extra_semicolon,
        warn_missing_semicolon,
        warn_empty_statement,

        // ── AST validation ────────────────────────────────────
        warn_unused_label,
        warn_unreachable_statement,

        // ── Sema ──────────────────────────────────────────────
        err_undeclared_identifier,
        err_type_mismatch,
        err_redefinition,
        err_no_such_member,
        err_wrong_arity,
        warn_unused_variable,
        warn_unused_parameter,
        warn_unused_function,
        warn_shadow,
        warn_implicit_conversion,
        warn_unreachable_code,
        warn_dead_code,

        // ── MIR dataflow ──────────────────────────────────────
        err_borrow_conflict,
        err_use_after_move,
        err_maybe_uninitialized,
        warn_redundant_clone,
        warn_unreachable_mir_block,

        // ── Engine-internal ───────────────────────────────────
        err_too_many_errors,
        warn_unknown_warning_option,
        anonymous,

        num_diags,
    };

    inline constexpr std::size_t NumDiags = static_cast<std::size_t>(DiagID::num_diags);

    struct DiagInfo
    {
        const char *name;   // "" if not a warning
        const char *format; // "%0"-style placeholders
        DiagDefault default_level;
        DiagPhase phase;
        bool has_fixit;
    };

    /// Table of every diagnostic. Kept in .rodata. MUST be kept in sync with DiagID.
    inline constexpr std::array<DiagInfo, NumDiags> DiagTable = {{
        // Lexer
        {"", "unterminated string literal", DiagDefault::Error, DiagPhase::Lexer, false},
        {"", "unterminated character literal", DiagDefault::Error, DiagPhase::Lexer, false},
        {"", "invalid character '%0'", DiagDefault::Error, DiagPhase::Lexer, false},
        {"", "invalid UTF-8 sequence", DiagDefault::Error, DiagPhase::Lexer, false},
        {"unknown-escape", "unknown escape sequence '\\%0'", DiagDefault::Warning, DiagPhase::Lexer, true},
        {"trigraph", "trigraph ignored", DiagDefault::Warning, DiagPhase::Lexer, false},
        {"comment", "multi-line comment", DiagDefault::Warning, DiagPhase::Lexer, false},
        {"digit-separator", "digit separator not allowed here", DiagDefault::Warning, DiagPhase::Lexer, false},

        // Parser
        {"", "expected '%0', found '%1'", DiagDefault::Error, DiagPhase::Parser, false},
        {"", "unexpected end of file", DiagDefault::Error, DiagPhase::Parser, false},
        {"", "unclosed delimiter '%0'", DiagDefault::Error, DiagPhase::Parser, true},
        {"", "extra ';'", DiagDefault::Error, DiagPhase::Parser, true},
        {"missing-semicolon", "expected ';' after expression", DiagDefault::WarningAll, DiagPhase::Parser, true},
        {"empty-statement", "empty statement", DiagDefault::WarningExtra, DiagPhase::Parser, false},

        // AST
        {"unused-label", "unused label '%0'", DiagDefault::Warning, DiagPhase::Parser, false},
        {"unreachable", "unreachable statement", DiagDefault::Warning, DiagPhase::Parser, false},

        // Sema
        {"", "use of undeclared identifier '%0'", DiagDefault::Error, DiagPhase::Sema, false},
        {"", "type mismatch: expected '%0', found '%1'", DiagDefault::Error, DiagPhase::Sema, false},
        {"", "redefinition of '%0'", DiagDefault::Error, DiagPhase::Sema, false},
        {"", "no member named '%0'", DiagDefault::Error, DiagPhase::Sema, false},
        {"", "wrong number of arguments: expected %0, got %1",
         DiagDefault::Error, DiagPhase::Sema, false},
        {"unused-variable", "unused variable '%0'", DiagDefault::Warning, DiagPhase::Sema, false},
        {"unused-parameter", "unused parameter '%0'", DiagDefault::WarningExtra, DiagPhase::Sema, false},
        {"unused-function", "unused function '%0'", DiagDefault::Warning, DiagPhase::Sema, false},
        {"shadow", "declaration shadows a previous declaration",
         DiagDefault::Off, DiagPhase::Sema, false},
        {"implicit-conversion", "implicit conversion from '%0' to '%1'", DiagDefault::WarningExtra, DiagPhase::Sema, false},
        {"unreachable-code", "unreachable code", DiagDefault::Warning, DiagPhase::Sema, false},
        {"dead-code", "'%0' is never used", DiagDefault::WarningAll, DiagPhase::Sema, false},

        // MIR
        {"", "cannot borrow '%0' as mutable because it is also borrowed as immutable",
         DiagDefault::Error, DiagPhase::Codegen, false},
        {"", "use of moved value '%0'", DiagDefault::Error, DiagPhase::Codegen, false},
        {"", "use of possibly uninitialized variable '%0'",
         DiagDefault::Error, DiagPhase::Codegen, false},
        {"redundant-clone", "redundant clone of '%0'", DiagDefault::Off, DiagPhase::Codegen, false},
        {"unreachable-mir", "unreachable MIR block", DiagDefault::Off, DiagPhase::Codegen, false},

        // Engine
        {"", "too many errors emitted, stopping now", DiagDefault::Error, DiagPhase::Lexer, false},
        {"unknown-warning-option", "unknown warning option '-W%0'", DiagDefault::Warning, DiagPhase::Lexer, false},
        {"", "", DiagDefault::Error, DiagPhase::Lexer, false},
    }};

    static_assert(DiagTable.size() == NumDiags);

    struct NamedDiag
    {
        const char *name;
        DiagID id;
    };

    /// Sorted name table for -W flag lookup. Keep sorted alphabetically.
    inline constexpr NamedDiag NamedDiags[] = {
        {"comment", DiagID::warn_multiline_comment},
        {"dead-code", DiagID::warn_dead_code},
        {"digit-separator", DiagID::warn_digit_separator},
        {"empty-statement", DiagID::warn_empty_statement},
        {"implicit-conversion", DiagID::warn_implicit_conversion},
        {"missing-semicolon", DiagID::warn_missing_semicolon},
        {"redundant-clone", DiagID::warn_redundant_clone},
        {"shadow", DiagID::warn_shadow},
        {"trigraph", DiagID::warn_trigraph_ignored},
        {"unknown-escape", DiagID::warn_unknown_escape},
        {"unknown-warning-option", DiagID::warn_unknown_warning_option},
        {"unreachable", DiagID::warn_unreachable_statement},
        {"unreachable-code", DiagID::warn_unreachable_code},
        {"unreachable-mir", DiagID::warn_unreachable_mir_block},
        {"unused-function", DiagID::warn_unused_function},
        {"unused-label", DiagID::warn_unused_label},
        {"unused-parameter", DiagID::warn_unused_parameter},
        {"unused-variable", DiagID::warn_unused_variable},
    };

    consteval bool named_diags_are_sorted() noexcept
    {
        for (std::size_t i = 1; i < std::size(NamedDiags); ++i)
            if (std::string_view(NamedDiags[i - 1].name) >=
                std::string_view(NamedDiags[i].name))
                return false;
        return true;
    }

    static_assert(named_diags_are_sorted(), "NamedDiags must stay sorted");

    [[nodiscard]] inline constexpr bool is_valid_diag_id(DiagID id) noexcept
    {
        return static_cast<std::size_t>(id) < NumDiags;
    }

    inline std::optional<DiagID> lookup_warning(std::string_view name) noexcept
    {
        auto lo = std::begin(NamedDiags);
        auto hi = std::end(NamedDiags);
        auto it = std::lower_bound(lo, hi, name,
                                   [](const NamedDiag &n, std::string_view v)
                                   { return std::string_view(n.name) < v; });
        if (it != hi && std::string_view(it->name) == name)
            return it->id;
        return std::nullopt;
    }

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_CODES_H
