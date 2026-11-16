#ifndef CORA_DIAGNOSTIC_CODES_H
#define CORA_DIAGNOSTIC_CODES_H

#include <Cora/Diag/Diagnostic.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace cora::diag
{

/// Compiler stage that owns a diagnostic.
enum class DiagPhase : std::uint8_t
{
    Lexer,
    Parser,
    Sema,
    Codegen,
    Engine,
};

/// Default policy for a diagnostic before command-line overrides are applied.
enum class DiagDefault : std::uint8_t
{
    Error,
    Warning,      ///< Enabled by default.
    WarningAll,   ///< Enabled by -Wall.
    WarningExtra, ///< Enabled by -Wextra.
    Pedantic,     ///< Enabled by -Wpedantic.
    Off,          ///< Only enabled explicitly with -Wname.
};

/// Every diagnostic in the compiler. IDs are stable across builds.
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
    warn_gnu_extension,

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
    warn_unknown_diagnostic_option,
    anonymous,

    num_diags,
};

inline constexpr std::size_t NumDiags = static_cast<std::size_t>(DiagID::num_diags);

struct DiagInfo
{
    const char *name;   ///< Warning name for -W lookups, or "" for errors.
    const char *code;   ///< Stable display code (E#### for errors, warning name for warnings).
    const char *format; ///< Message format using "%0"-style placeholders.
    DiagDefault default_level;
    DiagPhase phase;
    bool has_fixit;
};

/// Table of every diagnostic. Kept in .rodata. MUST be kept in sync with DiagID.
inline constexpr std::array<DiagInfo, NumDiags> DiagTable = {{
    // Lexer
    {"", "E1001", "unterminated string literal", DiagDefault::Error, DiagPhase::Lexer, false},
    {"", "E1002", "unterminated character literal", DiagDefault::Error, DiagPhase::Lexer, false},
    {"", "E1003", "invalid character '%0'", DiagDefault::Error, DiagPhase::Lexer, false},
    {"", "E1004", "invalid UTF-8 sequence", DiagDefault::Error, DiagPhase::Lexer, false},
    {"unknown-escape", "unknown-escape", "unknown escape sequence '\\%0'",
     DiagDefault::Warning, DiagPhase::Lexer, true},
    {"trigraph", "trigraph", "trigraph ignored", DiagDefault::Warning, DiagPhase::Lexer, false},
    {"comment", "comment", "multi-line comment", DiagDefault::Warning, DiagPhase::Lexer, false},
    {"digit-separator", "digit-separator", "digit separator not allowed here",
     DiagDefault::Warning, DiagPhase::Lexer, false},

    // Parser
    {"", "E2001", "expected '%0', found '%1'", DiagDefault::Error, DiagPhase::Parser, false},
    {"", "E2002", "unexpected end of file", DiagDefault::Error, DiagPhase::Parser, false},
    {"", "E2003", "unclosed delimiter '%0'", DiagDefault::Error, DiagPhase::Parser, true},
    {"", "E2004", "extra ';'", DiagDefault::Error, DiagPhase::Parser, true},
    {"missing-semicolon", "missing-semicolon", "expected ';' after expression",
     DiagDefault::WarningAll, DiagPhase::Parser, true},
    {"empty-statement", "empty-statement", "empty statement",
     DiagDefault::WarningExtra, DiagPhase::Parser, false},
    {"gnu-extension", "gnu-extension", "GNU extension '%0'",
     DiagDefault::Pedantic, DiagPhase::Parser, false},

    // AST
    {"unused-label", "unused-label", "unused label '%0'",
     DiagDefault::Warning, DiagPhase::Parser, false},
    {"unreachable", "unreachable", "unreachable statement",
     DiagDefault::Warning, DiagPhase::Parser, false},

    // Sema
    {"", "E3001", "use of undeclared identifier '%0'", DiagDefault::Error, DiagPhase::Sema, false},
    {"", "E3002", "type mismatch: expected '%0', found '%1'",
     DiagDefault::Error, DiagPhase::Sema, false},
    {"", "E3003", "redefinition of '%0'", DiagDefault::Error, DiagPhase::Sema, false},
    {"", "E3004", "no member named '%0'", DiagDefault::Error, DiagPhase::Sema, false},
    {"", "E3005", "wrong number of arguments: expected %0, got %1",
     DiagDefault::Error, DiagPhase::Sema, false},
    {"unused-variable", "unused-variable", "unused variable '%0'",
     DiagDefault::Warning, DiagPhase::Sema, false},
    {"unused-parameter", "unused-parameter", "unused parameter '%0'",
     DiagDefault::WarningExtra, DiagPhase::Sema, false},
    {"unused-function", "unused-function", "unused function '%0'",
     DiagDefault::Warning, DiagPhase::Sema, false},
    {"shadow", "shadow", "declaration shadows a previous declaration",
     DiagDefault::Off, DiagPhase::Sema, false},
    {"implicit-conversion", "implicit-conversion", "implicit conversion from '%0' to '%1'",
     DiagDefault::WarningExtra, DiagPhase::Sema, false},
    {"unreachable-code", "unreachable-code", "unreachable code",
     DiagDefault::Warning, DiagPhase::Sema, false},
    {"dead-code", "dead-code", "'%0' is never used",
     DiagDefault::WarningAll, DiagPhase::Sema, false},

    // MIR
    {"", "E4001", "cannot borrow '%0' as mutable because it is also borrowed as immutable",
     DiagDefault::Error, DiagPhase::Codegen, false},
    {"", "E4002", "use of moved value '%0'", DiagDefault::Error, DiagPhase::Codegen, false},
    {"", "E4003", "use of possibly uninitialized variable '%0'",
     DiagDefault::Error, DiagPhase::Codegen, false},
    {"redundant-clone", "redundant-clone", "redundant clone of '%0'",
     DiagDefault::Off, DiagPhase::Codegen, false},
    {"unreachable-mir", "unreachable-mir", "unreachable MIR block",
     DiagDefault::Off, DiagPhase::Codegen, false},

    // Engine
    {"", "E0001", "too many errors emitted, stopping now",
     DiagDefault::Error, DiagPhase::Engine, false},
    {"unknown-warning-option", "unknown-warning-option", "unknown warning option '-W%0'",
     DiagDefault::Warning, DiagPhase::Engine, false},
    {"unknown-diagnostic-option", "unknown-diagnostic-option",
     "unknown diagnostic option '%0'", DiagDefault::Warning, DiagPhase::Engine, false},
    {"", "", "", DiagDefault::Error, DiagPhase::Engine, false},
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
    {"gnu-extension", DiagID::warn_gnu_extension},
    {"implicit-conversion", DiagID::warn_implicit_conversion},
    {"missing-semicolon", DiagID::warn_missing_semicolon},
    {"redundant-clone", DiagID::warn_redundant_clone},
    {"shadow", DiagID::warn_shadow},
    {"trigraph", DiagID::warn_trigraph_ignored},
    {"unknown-diagnostic-option", DiagID::warn_unknown_diagnostic_option},
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

constexpr bool named_diags_are_sorted() noexcept
{
    for (std::size_t i = 1; i < std::size(NamedDiags); ++i)
    {
        if (std::string_view(NamedDiags[i - 1].name) >=
            std::string_view(NamedDiags[i].name))
        {
            return false;
        }
    }
    return true;
}

static_assert(named_diags_are_sorted(), "NamedDiags must stay sorted");

[[nodiscard]] inline constexpr bool is_valid_diag_id(DiagID id) noexcept
{
    return id != DiagID::anonymous &&
           static_cast<std::size_t>(id) < NumDiags;
}

[[nodiscard]] inline constexpr const DiagInfo &diag_info(DiagID id) noexcept
{
    return DiagTable[static_cast<std::size_t>(id)];
}

[[nodiscard]] inline std::optional<DiagID> lookup_warning(std::string_view name) noexcept
{
    auto lo = std::begin(NamedDiags);
    auto hi = std::end(NamedDiags);
    auto it = std::lower_bound(lo, hi, name,
                               [](const NamedDiag &named, std::string_view value)
                               { return std::string_view(named.name) < value; });
    if (it != hi && std::string_view(it->name) == name)
    {
        return it->id;
    }
    return std::nullopt;
}

namespace detail
{

inline void append_diag_arg(std::string &out, std::string_view value)
{
    if (!value.empty())
    {
        out.append(value.data(), value.size());
    }
}

inline void append_diag_arg(std::string &out, const std::string &value)
{
    out.append(value);
}

inline void append_diag_arg(std::string &out, const char *value)
{
    if (value != nullptr)
    {
        out.append(value);
    }
}

inline void append_diag_arg(std::string &out, char value)
{
    out.push_back(value);
}

inline void append_diag_arg(std::string &out, bool value)
{
    out.append(value ? "true" : "false");
}

template <typename T,
          std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
inline void append_diag_arg(std::string &out, T value)
{
    out.append(std::to_string(value));
}

template <typename T,
          std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
inline void append_diag_arg(std::string &out, T value)
{
    std::ostringstream stream;
    stream << value;
    out.append(stream.str());
}

template <typename T,
          std::enable_if_t<!std::is_arithmetic_v<T>, int> = 0>
inline void append_diag_arg(std::string &out, const T &value)
{
    std::ostringstream stream;
    stream << value;
    out.append(stream.str());
}

} // namespace detail

/// Format a diagnostic message using its table format and the supplied
/// arguments. This function is only called on the cold path, after a
/// diagnostic has been determined to be enabled.
template <typename... Args>
[[nodiscard]] std::string format_diag(DiagID id, Args &&...args)
{
    const auto index = static_cast<std::size_t>(id);
    if (index >= NumDiags)
    {
        return {};
    }

    const std::string_view format = DiagTable[index].format;
    if constexpr (sizeof...(Args) == 0)
    {
        return std::string(format);
    }
    else
    {
        std::array<std::string, sizeof...(Args)> arguments;
        std::size_t argument_index = 0;
        auto append = [&](auto &&argument)
        {
            std::string &text = arguments[argument_index++];
            using Arg = std::decay_t<decltype(argument)>;
            if constexpr (std::is_same_v<Arg, std::string>)
            {
                text = argument;
            }
            else if constexpr (std::is_same_v<Arg, std::string_view>)
            {
                if (!argument.empty())
                {
                    text.assign(argument.data(), argument.size());
                }
            }
            else
            {
                detail::append_diag_arg(
                    text, std::forward<decltype(argument)>(argument));
            }
        };
        (append(std::forward<Args>(args)), ...);

        // Substitute in one pass so placeholders inside argument text are
        // never expanded a second time.
        std::string result;
        result.reserve(format.size());
        for (std::size_t position = 0; position < format.size();)
        {
            if (format[position] == '%' && position + 1 < format.size() &&
                format[position + 1] >= '0' && format[position + 1] <= '9')
            {
                const std::size_t argument =
                    static_cast<std::size_t>(format[position + 1] - '0');
                if (argument < arguments.size())
                {
                    result.append(arguments[argument]);
                    position += 2;
                    continue;
                }
            }
            result.push_back(format[position]);
            ++position;
        }
        return result;
    }
}

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_CODES_H
