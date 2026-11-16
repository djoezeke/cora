#ifndef CORA_DIAGNOSTIC_FLAGPARSER_H
#define CORA_DIAGNOSTIC_FLAGPARSER_H

#include <Cora/Diag/DiagnosticEngine.h>

#include <string_view>

namespace cora::diag
{

/// Cold-path parser for clang-style diagnostic flags.
///
/// Supported examples:
///   -Wall -Wextra -Wpedantic -Werror -Wno-error
///   -Wunused-variable -Wno-unused-variable
///   -Werror=unused-variable -Wno-error=unused-variable
///   -w -fix -ferror-limit=20 -fdiagnostics-format=json
///   -fdiagnostics-color=never -fdiagnostics-show-caret
class FlagParser
{
public:
    explicit FlagParser(DiagnosticEngine &engine) : engine_(engine) {}

    /// Parse one command-line token. Returns true if it was a diagnostic flag.
    bool parse(std::string_view argument);

    /// Parse a bare "-W..." suffix (the argument begins after "W").
    void parse_warning(std::string_view flag);

    /// Parse a "-fdiagnostics-..." or "-fno-diagnostics-..." flag.
    void parse_fdiagnostic(std::string_view flag);

private:
    void warn_unknown(std::string_view name);
    void warn_unknown_diagnostic(std::string_view flag);

    DiagnosticEngine &engine_;
};

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_FLAGPARSER_H
