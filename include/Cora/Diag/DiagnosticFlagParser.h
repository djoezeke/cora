#ifndef CORA_DIAGNOSTIC_FLAGPARSER_H
#define CORA_DIAGNOSTIC_FLAGPARSER_H

#include <Cora/Diag/DiagnosticEngine.h>

#include <string_view>

namespace cora::diag
{

    /// Cold-path parser for -W / -fdiagnostics flags. Accepts argv-style tokens
    /// (without the leading "--" for options that take no value). Returns
    /// true if the token was consumed.
    class FlagParser
    {
    public:
        explicit FlagParser(DiagnosticEngine &eng);

        /// Parse one command-line token. Returns true if it was a diagnostic flag.
        bool parse(std::string_view arg);

        /// Parse a bare "-W..." suffix (arg begins with "W").
        void parse_warning(std::string_view flag);

        /// Parse a "-fdiagnostics-..." flag.
        void parse_fdiagnostic(std::string_view flag);

    private:
        void warn_unknown(std::string_view name);
        DiagnosticEngine &eng_;
    };

} // namespace cora::diag

#endif // CORA_DIAGNOSTIC_FLAGPARSER_H
