#include <Cora/Diag/DiagnosticConfig.h>

#include <catch2/catch_test_macros.hpp>

using cora::diag::DiagID;
using cora::diag::DiagnosticConfig;
using cora::diag::DiagnosticOptions;
using cora::diag::Severity;

TEST_CASE("DiagnosticConfig applies default warning groups", "[diag][config]")
{
    DiagnosticConfig config;

    REQUIRE(config.effective_level(DiagID::err_type_mismatch) == Severity::Error);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Warning);
    REQUIRE(config.effective_level(DiagID::warn_missing_semicolon) ==
            Severity::Ignored);
    REQUIRE(config.effective_level(DiagID::warn_unused_parameter) ==
            Severity::Ignored);
    REQUIRE(config.effective_level(DiagID::warn_gnu_extension) ==
            Severity::Ignored);
    REQUIRE(config.effective_level(DiagID::warn_shadow) == Severity::Ignored);
}

TEST_CASE("DiagnosticConfig implements -Wall, -Wextra and -Wpedantic",
          "[diag][config]")
{
    DiagnosticConfig config;
    config.set_wall(true);
    REQUIRE(config.effective_level(DiagID::warn_missing_semicolon) ==
            Severity::Warning);
    REQUIRE(config.effective_level(DiagID::warn_dead_code) == Severity::Warning);
    REQUIRE(config.effective_level(DiagID::warn_unused_parameter) ==
            Severity::Ignored);

    config.set_wextra(true);
    REQUIRE(config.effective_level(DiagID::warn_unused_parameter) ==
            Severity::Warning);
    REQUIRE(config.effective_level(DiagID::warn_implicit_conversion) ==
            Severity::Warning);

    config.set_pedantic(true);
    REQUIRE(config.effective_level(DiagID::warn_gnu_extension) ==
            Severity::Warning);

    config.set_wall(false);
    config.set_wextra(false);
    config.set_pedantic(false);
    REQUIRE(config.effective_level(DiagID::warn_missing_semicolon) ==
            Severity::Ignored);
    REQUIRE(config.effective_level(DiagID::warn_unused_parameter) ==
            Severity::Ignored);
    REQUIRE(config.effective_level(DiagID::warn_gnu_extension) ==
            Severity::Ignored);
}

TEST_CASE("DiagnosticConfig supports per-warning enable and disable",
          "[diag][config]")
{
    DiagnosticConfig config;

    config.enable_warning(DiagID::warn_shadow);
    REQUIRE(config.effective_level(DiagID::warn_shadow) == Severity::Warning);

    config.disable_warning(DiagID::warn_unused_variable);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Ignored);

    config.reset_level(DiagID::warn_unused_variable);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Warning);

    config.enable_warning(DiagID::err_type_mismatch);
    REQUIRE(config.effective_level(DiagID::err_type_mismatch) == Severity::Error);
}

TEST_CASE("DiagnosticConfig implements global and per-warning -Werror",
          "[diag][config]")
{
    DiagnosticConfig config;

    config.set_all_werror(true);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Error);

    config.set_werror(DiagID::warn_unused_variable, false);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Warning);

    config.set_all_werror(false);
    config.set_werror(DiagID::warn_unused_variable, true);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Error);

    config.reset_werror(DiagID::warn_unused_variable);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Warning);
}

TEST_CASE("DiagnosticConfig implements -w and generic warning policy",
          "[diag][config]")
{
    DiagnosticConfig config;
    REQUIRE(config.effective_level(Severity::Warning) == Severity::Warning);

    config.set_suppress_warnings(true);
    REQUIRE(config.effective_level(Severity::Warning) == Severity::Ignored);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Ignored);

    config.set_suppress_warnings(false);
    config.set_all_werror(true);
    REQUIRE(config.effective_level(Severity::Warning) == Severity::Error);
    REQUIRE(config.effective_level(DiagID::warn_unused_variable) ==
            Severity::Error);
}

TEST_CASE("DiagnosticConfig exposes error limit and options", "[diag][config]")
{
    DiagnosticOptions options;
    options.error_limit = 7;
    options.color = false;

    DiagnosticConfig config(options);
    REQUIRE(config.error_limit() == 7);
    REQUIRE_FALSE(config.options().color);

    config.set_error_limit(3);
    REQUIRE(config.error_limit() == 3);
    REQUIRE(config.options().error_limit == 3);
}
