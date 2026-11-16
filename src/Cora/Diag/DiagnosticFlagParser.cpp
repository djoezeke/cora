#include <Cora/Diag/DiagnosticFlagParser.h>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <string>
#include <string_view>
#include <system_error>

namespace cora::diag
{
namespace
{

[[nodiscard]] bool starts_with(std::string_view value, std::string_view prefix)
{
    return value.size() >= prefix.size() &&
           value.compare(0, prefix.size(), prefix) == 0;
}

[[nodiscard]] bool parse_size(std::string_view text, std::size_t &value)
{
    if (text.empty())
    {
        return false;
    }
    const auto result =
        std::from_chars(text.data(), text.data() + text.size(), value);
    return result.ec == std::errc{} &&
           result.ptr == text.data() + text.size();
}

/// Apply a group flag to its members. Enabling resets earlier per-warning
/// overrides so that the most recent flag wins; disabling installs an
/// explicit "off" override that a later -Wname can still re-enable.
void set_group(DiagnosticConfig &config, DiagDefault group, bool enabled)
{
    for (std::size_t index = 0; index < NumDiags; ++index)
    {
        if (DiagTable[index].default_level != group)
        {
            continue;
        }

        const auto id = static_cast<DiagID>(index);
        if (enabled)
        {
            config.reset_level(id);
        }
        else
        {
            config.disable_warning(id);
        }
    }
}

} // namespace

bool FlagParser::parse(std::string_view argument)
{
    if (argument == "-w" || argument == "--suppress-warnings")
    {
        engine_.config().set_suppress_warnings(true);
        return true;
    }

    if (argument == "-W")
    {
        engine_.config().set_wextra(true);
        return true;
    }

    // These are driver flags (linker, preprocessor and assembler), not
    // diagnostic warning options.
    if (starts_with(argument, "-Wl,") || starts_with(argument, "-Wp,") ||
        starts_with(argument, "-Wa,"))
    {
        return false;
    }

    if (starts_with(argument, "-W"))
    {
        parse_warning(argument.substr(2));
        return true;
    }

    if (argument == "-pedantic")
    {
        DiagnosticConfig &config = engine_.config();
        config.set_pedantic(true);
        set_group(config, DiagDefault::Pedantic, true);
        return true;
    }

    if (argument == "-pedantic-errors")
    {
        DiagnosticConfig &config = engine_.config();
        config.set_pedantic(true);
        set_group(config, DiagDefault::Pedantic, true);
        for (std::size_t index = 0; index < NumDiags; ++index)
        {
            if (DiagTable[index].default_level == DiagDefault::Pedantic)
            {
                config.set_werror(static_cast<DiagID>(index), true);
            }
        }
        return true;
    }

    if (argument == "-fix" || argument == "-fixit" || argument == "--fix")
    {
        engine_.config().options().fix = true;
        return true;
    }

    if (argument == "-fcolor-diagnostics")
    {
        engine_.config().options().color = true;
        return true;
    }

    if (argument == "-fno-color-diagnostics")
    {
        engine_.config().options().color = false;
        return true;
    }

    if (argument == "-fshow-column")
    {
        engine_.config().options().show_column = true;
        return true;
    }

    if (argument == "-fno-show-column")
    {
        engine_.config().options().show_column = false;
        return true;
    }

    if (argument == "-fcaret-diagnostics")
    {
        engine_.config().options().show_carets = true;
        return true;
    }

    if (argument == "-fno-caret-diagnostics")
    {
        engine_.config().options().show_carets = false;
        return true;
    }

    if (starts_with(argument, "-ferror-limit=") ||
        starts_with(argument, "-fmax-errors="))
    {
        const std::size_t equals = argument.find('=');
        std::size_t limit = 0;
        if (equals != std::string_view::npos &&
            parse_size(argument.substr(equals + 1), limit))
        {
            engine_.config().set_error_limit(limit);
        }
        else
        {
            warn_unknown_diagnostic(argument);
        }
        return true;
    }

    if (starts_with(argument, "-fdiagnostics-") ||
        starts_with(argument, "-fno-diagnostics-"))
    {
        parse_fdiagnostic(argument);
        return true;
    }

    return false;
}

void FlagParser::parse_warning(std::string_view flag)
{
    DiagnosticConfig &config = engine_.config();

    if (flag.empty() || flag == "extra")
    {
        config.set_wextra(true);
        set_group(config, DiagDefault::WarningExtra, true);
        return;
    }

    if (flag == "all")
    {
        config.set_wall(true);
        set_group(config, DiagDefault::WarningAll, true);
        return;
    }
    if (flag == "no-all")
    {
        config.set_wall(false);
        set_group(config, DiagDefault::WarningAll, false);
        return;
    }
    if (flag == "no-extra")
    {
        config.set_wextra(false);
        set_group(config, DiagDefault::WarningExtra, false);
        return;
    }
    if (flag == "pedantic")
    {
        config.set_pedantic(true);
        set_group(config, DiagDefault::Pedantic, true);
        return;
    }
    if (flag == "no-pedantic")
    {
        config.set_pedantic(false);
        set_group(config, DiagDefault::Pedantic, false);
        return;
    }
    if (flag == "error")
    {
        config.set_all_werror(true);
        return;
    }
    if (flag == "no-error")
    {
        config.set_all_werror(false);
        return;
    }
    if (flag == "everything")
    {
        config.set_wall(true);
        config.set_wextra(true);
        config.set_pedantic(true);
        set_group(config, DiagDefault::WarningAll, true);
        set_group(config, DiagDefault::WarningExtra, true);
        set_group(config, DiagDefault::Pedantic, true);
        for (const NamedDiag &named : NamedDiags)
        {
            config.enable_warning(named.id);
        }
        return;
    }
    if (flag == "no-everything")
    {
        config.set_wall(false);
        config.set_wextra(false);
        config.set_pedantic(false);
        set_group(config, DiagDefault::WarningAll, false);
        set_group(config, DiagDefault::WarningExtra, false);
        set_group(config, DiagDefault::Pedantic, false);
        for (const NamedDiag &named : NamedDiags)
        {
            config.disable_warning(named.id);
        }
        return;
    }
    if (flag == "fatal-errors")
    {
        config.set_error_limit(1);
        return;
    }

    if (starts_with(flag, "error="))
    {
        const std::string_view name = flag.substr(6);
        if (const auto id = lookup_warning(name))
        {
            config.enable_warning(*id);
            config.set_werror(*id, true);
        }
        else
        {
            warn_unknown(flag);
        }
        return;
    }

    if (starts_with(flag, "no-error="))
    {
        const std::string_view name = flag.substr(9);
        if (const auto id = lookup_warning(name))
        {
            config.set_werror(*id, false);
        }
        else
        {
            warn_unknown(flag);
        }
        return;
    }

    if (starts_with(flag, "no-"))
    {
        const std::string_view name = flag.substr(3);
        if (const auto id = lookup_warning(name))
        {
            config.disable_warning(*id);
        }
        else
        {
            warn_unknown(flag);
        }
        return;
    }

    if (const auto id = lookup_warning(flag))
    {
        config.enable_warning(*id);
        return;
    }

    warn_unknown(flag);
}

void FlagParser::parse_fdiagnostic(std::string_view flag)
{
    DiagnosticOptions &options = engine_.config().options();

    if (flag == "-fdiagnostics-color" || flag == "-fdiagnostics-color=always" ||
        flag == "-fdiagnostics-color=auto")
    {
        options.color = true;
        return;
    }
    if (flag == "-fdiagnostics-color=never" ||
        flag == "-fno-diagnostics-color")
    {
        options.color = false;
        return;
    }

    if (flag == "-fdiagnostics-show-column")
    {
        options.show_column = true;
        return;
    }
    if (flag == "-fno-diagnostics-show-column")
    {
        options.show_column = false;
        return;
    }

    if (flag == "-fdiagnostics-show-caret")
    {
        options.show_carets = true;
        return;
    }
    if (flag == "-fno-diagnostics-show-caret")
    {
        options.show_carets = false;
        return;
    }

    if (flag == "-fdiagnostics-show-source-ranges")
    {
        options.show_source_ranges = true;
        return;
    }
    if (flag == "-fno-diagnostics-show-source-ranges")
    {
        options.show_source_ranges = false;
        return;
    }

    if (flag == "-fdiagnostics-show-fixits" ||
        flag == "-fdiagnostics-fixit-info" ||
        flag == "-fdiagnostics-parseable-fixits")
    {
        options.show_fixits = true;
        return;
    }
    if (flag == "-fno-diagnostics-show-fixits" ||
        flag == "-fno-diagnostics-fixit-info")
    {
        options.show_fixits = false;
        return;
    }

    if (flag == "-fdiagnostics-show-summary")
    {
        options.show_summary = true;
        return;
    }
    if (flag == "-fno-diagnostics-show-summary")
    {
        options.show_summary = false;
        return;
    }

    if (flag == "-fdiagnostics-show-option")
    {
        options.show_option = true;
        return;
    }
    if (flag == "-fno-diagnostics-show-option")
    {
        options.show_option = false;
        return;
    }

    if (flag == "-fdiagnostics-sort")
    {
        options.sort_on_flush = true;
        return;
    }
    if (flag == "-fno-diagnostics-sort")
    {
        options.sort_on_flush = false;
        return;
    }

    if (flag == "-fdiagnostics-absolute-paths")
    {
        options.absolute_paths = true;
        return;
    }
    if (flag == "-fno-diagnostics-absolute-paths")
    {
        options.absolute_paths = false;
        return;
    }

    if (flag == "-fdiagnostics-verify")
    {
        options.verify_mode = true;
        return;
    }
    if (flag == "-fno-diagnostics-verify")
    {
        options.verify_mode = false;
        return;
    }

    if (flag == "-fdiagnostics-format=terminal")
    {
        options.format = DiagnosticFormat::Terminal;
        return;
    }
    if (flag == "-fdiagnostics-format=json")
    {
        options.format = DiagnosticFormat::Json;
        return;
    }
    if (flag == "-fdiagnostics-format=lsp")
    {
        options.format = DiagnosticFormat::Lsp;
        return;
    }

    warn_unknown_diagnostic(flag);
}

void FlagParser::warn_unknown(std::string_view name)
{
    engine_.emit(DiagID::warn_unknown_warning_option, SourceSpan{}, name);
}

void FlagParser::warn_unknown_diagnostic(std::string_view flag)
{
    engine_.emit(DiagID::warn_unknown_diagnostic_option, SourceSpan{}, flag);
}

} // namespace cora::diag
