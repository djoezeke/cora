#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEmitter.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Diag/DiagnosticFlagParser.h>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>

using cora::core::SourceManager;
using cora::core::SourceSpan;
using cora::diag::CollectingEmitter;
using cora::diag::DiagID;
using cora::diag::DiagnosticEngine;
using cora::diag::DiagnosticFormat;
using cora::diag::FlagParser;
using cora::diag::Severity;

TEST_CASE("FlagParser accepts clang-style diagnostic flags",
          "[diag][flags]")
{
    DiagnosticEngine engine;
    FlagParser parser(engine);

    REQUIRE(parser.parse("-Wall"));
    REQUIRE(engine.config().wall());
    REQUIRE(parser.parse("-Wextra"));
    REQUIRE(engine.config().wextra());
    REQUIRE(parser.parse("-Wpedantic"));
    REQUIRE(engine.config().pedantic());
    REQUIRE(parser.parse("-Werror"));
    REQUIRE(engine.config().all_werror());
    REQUIRE(parser.parse("-Wno-error"));
    REQUIRE_FALSE(engine.config().all_werror());
    REQUIRE(parser.parse("-w"));
    REQUIRE(engine.config().suppress_warnings());
    REQUIRE(parser.parse("-fix"));
    REQUIRE(engine.config().options().fix);
    REQUIRE(parser.parse("-ferror-limit=5"));
    REQUIRE(engine.config().error_limit() == 5);
    REQUIRE(parser.parse("-fdiagnostics-format=json"));
    REQUIRE(engine.config().options().format == DiagnosticFormat::Json);
    REQUIRE(parser.parse("-fdiagnostics-color=never"));
    REQUIRE_FALSE(engine.config().options().color);
    REQUIRE(parser.parse("-fno-diagnostics-show-caret"));
    REQUIRE_FALSE(engine.config().options().show_carets);
    REQUIRE(parser.parse("-fdiagnostics-show-source-ranges"));
    REQUIRE(engine.config().options().show_source_ranges);
    REQUIRE(parser.parse("-fno-diagnostics-show-option"));
    REQUIRE_FALSE(engine.config().options().show_option);
    REQUIRE_FALSE(parser.parse("--not-a-diagnostic-flag"));
    REQUIRE_FALSE(parser.parse("-Wl,--as-needed"));
    REQUIRE_FALSE(parser.parse("-Wp,-DDEBUG"));
}

TEST_CASE("FlagParser implements -Wno-warning and -Wwarning",
          "[diag][flags]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("flags.cora", "let value = 1;\n");
    const SourceSpan span{file_id, 4, 9};

    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);
    FlagParser parser(engine);

    REQUIRE(parser.parse("-Wno-unused-variable"));
    {
        auto builder = engine.emit(DiagID::warn_unused_variable, span, "value");
        REQUIRE_FALSE(builder.active());
    }
    engine.flush();
    REQUIRE(collector->count() == 0);

    REQUIRE(parser.parse("-Wunused-variable"));
    {
        auto builder = engine.emit(DiagID::warn_unused_variable, span, "value");
        REQUIRE(builder.active());
    }
    engine.flush();

    REQUIRE(collector->count() == 1);
    REQUIRE(collector->last()->severity() == Severity::Warning);
    REQUIRE(collector->last()->code() == std::string("unused-variable"));
    engine.finish();
}

TEST_CASE("FlagParser implements global and per-warning -Werror",
          "[diag][flags]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("werror.cora", "let value = 1;\n");
    const SourceSpan span{file_id, 4, 9};

    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);
    FlagParser parser(engine);

    REQUIRE(parser.parse("-Werror"));
    engine.emit(DiagID::warn_unused_variable, span, "value");
    engine.flush();
    REQUIRE(collector->last()->severity() == Severity::Error);
    collector->clear();

    REQUIRE(parser.parse("-Wno-error=unused-variable"));
    engine.emit(DiagID::warn_unused_variable, span, "value");
    engine.flush();
    REQUIRE(collector->last()->severity() == Severity::Warning);
    collector->clear();

    REQUIRE(parser.parse("-Werror=unused-variable"));
    engine.emit(DiagID::warn_unused_variable, span, "value");
    engine.flush();
    REQUIRE(collector->last()->severity() == Severity::Error);

    engine.finish();
}

TEST_CASE("FlagParser enables -Wall, -Wextra and -Wpedantic groups",
          "[diag][flags]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("groups.cora", "let value = 1;\n");
    const SourceSpan span{file_id, 4, 9};

    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);
    FlagParser parser(engine);

    {
        auto builder = engine.emit(DiagID::warn_missing_semicolon, span);
        REQUIRE_FALSE(builder.active());
    }

    REQUIRE(parser.parse("-Wall"));
    {
        auto builder = engine.emit(DiagID::warn_missing_semicolon, span);
        REQUIRE(builder.active());
    }

    REQUIRE(parser.parse("-Wextra"));
    {
        auto builder = engine.emit(DiagID::warn_unused_parameter, span, "value");
        REQUIRE(builder.active());
    }

    REQUIRE(parser.parse("-Wpedantic"));
    {
        auto builder = engine.emit(DiagID::warn_gnu_extension, span, "statement");
        REQUIRE(builder.active());
    }

    engine.finish();
    REQUIRE(collector->count() == 3);
}

TEST_CASE("FlagParser reports unknown warning options", "[diag][flags]")
{
    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);
    FlagParser parser(engine);

    REQUIRE(parser.parse("-Wdoes-not-exist"));
    engine.finish();

    REQUIRE(collector->count() == 1);
    REQUIRE(collector->last()->code() == std::string("unknown-warning-option"));
    REQUIRE(collector->last()->message() ==
            std::string("unknown warning option '-Wdoes-not-exist'"));
}

TEST_CASE("FlagParser reports unknown diagnostic options", "[diag][flags]")
{
    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);
    FlagParser parser(engine);

    REQUIRE(parser.parse("-fdiagnostics-bogus"));
    engine.finish();

    REQUIRE(collector->count() == 1);
    REQUIRE(collector->last()->code() ==
            std::string("unknown-diagnostic-option"));
    REQUIRE(collector->last()->message() ==
            std::string("unknown diagnostic option '-fdiagnostics-bogus'"));
}

TEST_CASE("FlagParser supports -Weverything and -Wno-everything",
          "[diag][flags]")
{
    DiagnosticEngine engine;
    FlagParser parser(engine);

    REQUIRE(parser.parse("-Wall"));
    REQUIRE(parser.parse("-Wextra"));
    REQUIRE(parser.parse("-Wpedantic"));
    REQUIRE(parser.parse("-Weverything"));
    REQUIRE(engine.config().effective_level(DiagID::warn_shadow) ==
            Severity::Warning);
    REQUIRE(engine.config().effective_level(DiagID::warn_redundant_clone) ==
            Severity::Warning);

    REQUIRE(parser.parse("-Wno-everything"));
    REQUIRE_FALSE(engine.config().wall());
    REQUIRE_FALSE(engine.config().wextra());
    REQUIRE_FALSE(engine.config().pedantic());
    REQUIRE(engine.config().effective_level(DiagID::warn_shadow) ==
            Severity::Ignored);
    REQUIRE(engine.config().effective_level(DiagID::warn_unused_variable) ==
            Severity::Ignored);
    REQUIRE(engine.config().effective_level(DiagID::warn_missing_semicolon) ==
            Severity::Ignored);
    REQUIRE(engine.config().effective_level(DiagID::warn_unused_parameter) ==
            Severity::Ignored);
    REQUIRE(engine.config().effective_level(DiagID::warn_gnu_extension) ==
            Severity::Ignored);
}

TEST_CASE("FlagParser applies warning group flags in order", "[diag][flags]")
{
    DiagnosticEngine engine;
    FlagParser parser(engine);

    REQUIRE(parser.parse("-Wno-everything"));
    REQUIRE(engine.config().effective_level(DiagID::warn_missing_semicolon) ==
            Severity::Ignored);

    REQUIRE(parser.parse("-Wall"));
    REQUIRE(engine.config().effective_level(DiagID::warn_missing_semicolon) ==
            Severity::Warning);
    REQUIRE(engine.config().effective_level(DiagID::warn_dead_code) ==
            Severity::Warning);
    // -Wall does not re-enable warnings outside its group.
    REQUIRE(engine.config().effective_level(DiagID::warn_unused_variable) ==
            Severity::Ignored);

    REQUIRE(parser.parse("-Wno-all"));
    REQUIRE(engine.config().effective_level(DiagID::warn_missing_semicolon) ==
            Severity::Ignored);
    REQUIRE(engine.config().effective_level(DiagID::warn_dead_code) ==
            Severity::Ignored);

    REQUIRE(parser.parse("-Wmissing-semicolon"));
    REQUIRE(engine.config().effective_level(DiagID::warn_missing_semicolon) ==
            Severity::Warning);
}

TEST_CASE("FlagParser limits -pedantic-errors to pedantic diagnostics",
          "[diag][flags]")
{
    DiagnosticEngine engine;
    FlagParser parser(engine);

    REQUIRE(parser.parse("-pedantic-errors"));
    REQUIRE(engine.config().effective_level(DiagID::warn_gnu_extension) ==
            Severity::Error);
    REQUIRE(engine.config().effective_level(DiagID::warn_unused_variable) ==
            Severity::Warning);

    REQUIRE(parser.parse("-Wno-error=gnu-extension"));
    REQUIRE(engine.config().effective_level(DiagID::warn_gnu_extension) ==
            Severity::Warning);
}

TEST_CASE("FlagParser promotes generic warnings with -Werror",
          "[diag][flags]")
{
    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);
    FlagParser parser(engine);

    REQUIRE(parser.parse("-Werror"));
    {
        auto builder = engine.warning("generic warning");
        REQUIRE(builder.active());
        REQUIRE(builder.diagnostic().severity() == Severity::Error);
    }
    engine.finish();

    REQUIRE(collector->count() == 1);
    REQUIRE(collector->last()->severity() == Severity::Error);
}

TEST_CASE("FlagParser suppresses generic warnings with -w", "[diag][flags]")
{
    DiagnosticEngine engine;
    FlagParser parser(engine);

    REQUIRE(parser.parse("-w"));
    {
        auto builder = engine.warning("generic warning");
        REQUIRE_FALSE(builder.active());
    }
    REQUIRE(engine.warning_count() == 0);
    REQUIRE(engine.buffered_count() == 0);
}
