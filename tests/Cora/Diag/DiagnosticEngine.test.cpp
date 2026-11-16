#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEmitter.h>
#include <Cora/Diag/DiagnosticEngine.h>

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using cora::core::SourceManager;
using cora::core::SourceSpan;
using cora::diag::CallbackEmitter;
using cora::diag::CollectingEmitter;
using cora::diag::DiagID;
using cora::diag::Diagnostic;
using cora::diag::DiagnosticEmitter;
using cora::diag::DiagnosticEngine;
using cora::diag::DiagnosticOptions;
using cora::diag::Severity;

namespace
{

struct FormatProbe
{
    bool *called = nullptr;
};

std::ostream &operator<<(std::ostream &out, const FormatProbe &probe)
{
    if (probe.called != nullptr)
    {
        *probe.called = true;
    }
    return out << "probe";
}

class CountingEmitter final : public DiagnosticEmitter
{
public:
    void emit(const Diagnostic &) override { ++emits; }
    void flush() override { ++flushes; }
    void finish() override { ++finishes; }

    int emits = 0;
    int flushes = 0;
    int finishes = 0;
};

} // namespace

TEST_CASE("DiagnosticEngine submits builders automatically on scope exit",
          "[diag][engine][raii]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("test.cora", "let value = 1;\n");

    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    {
        engine.error("something went wrong")
            .code("E1001")
            .primary(SourceSpan{file_id, 4, 9}, "here")
            .note("a note")
            .help("a hint");
    }

    REQUIRE(engine.buffered_count() == 1);
    REQUIRE(engine.reported_count() == 1);

    engine.finish();

    REQUIRE(engine.error_count() == 1);
    REQUIRE(engine.has_errors());
    REQUIRE(engine.exit_code() == 1);

    const auto diagnostics = collector->diagnostics();
    REQUIRE(diagnostics.size() == 1);
    REQUIRE(diagnostics[0].severity() == Severity::Error);
    REQUIRE(diagnostics[0].code() == std::string("E1001"));
    REQUIRE(diagnostics[0].labels().size() == 1);
    REQUIRE(diagnostics[0].hints().size() == 2);
    REQUIRE(diagnostics[0].hints()[1] == std::string("help: a hint"));
}

TEST_CASE("DiagnosticEngine supports explicit emit and cancellation",
          "[diag][engine][raii]")
{
    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    {
        auto builder = engine.error("explicit");
        builder.emit();
        builder.emit();
    }
    REQUIRE(engine.buffered_count() == 1);

    {
        auto builder = engine.warning("cancelled");
        builder.cancel();
    }
    REQUIRE(engine.buffered_count() == 1);

    engine.finish();
    REQUIRE(collector->count() == 1);
}

TEST_CASE("DiagnosticEngine enforces the error limit", "[diag][engine][limit]")
{
    DiagnosticOptions options;
    options.error_limit = 3;

    DiagnosticEngine engine(options);
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    for (int index = 0; index < 10; ++index)
    {
        engine.error("error " + std::to_string(index));
    }

    REQUIRE(engine.error_count() == 3);
    REQUIRE(engine.limit_hit());
    REQUIRE(engine.buffered_count() == 4);

    engine.report(Diagnostic::error("late error"));
    REQUIRE(engine.suppressed_error_count() == 1);
    REQUIRE(engine.buffered_count() == 4);

    engine.finish();

    const auto diagnostics = collector->diagnostics();
    REQUIRE(diagnostics.size() == 4);
    REQUIRE(diagnostics.back().severity() == Severity::Fatal);
    REQUIRE(diagnostics.back().message() ==
            std::string("too many errors emitted, stopping now"));
    REQUIRE(diagnostics.back().code() == std::string("E0001"));
}

TEST_CASE("DiagnosticEngine handles diagnostics from many threads",
          "[diag][engine][threads]")
{
    constexpr int thread_count = 8;
    constexpr int diagnostics_per_thread = 250;

    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("threads.cora", "let x = 1;\n");

    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    std::atomic<int> ready{0};
    std::vector<std::thread> workers;
    workers.reserve(thread_count);

    for (int thread_index = 0; thread_index < thread_count; ++thread_index)
    {
        workers.emplace_back(
            [&engine, &ready, file_id, thread_index]()
            {
                ready.fetch_add(1, std::memory_order_relaxed);
                while (ready.load(std::memory_order_acquire) < thread_count)
                {
                    std::this_thread::yield();
                }
                for (int index = 0; index < diagnostics_per_thread; ++index)
                {
                    const std::uint32_t offset =
                        static_cast<std::uint32_t>(thread_index * 1000 + index);
                    engine.emit(DiagID::warn_unused_variable,
                                SourceSpan{file_id, offset, offset + 1},
                                "value_" + std::to_string(thread_index) + "_" +
                                    std::to_string(index));
                }
            });
    }

    for (auto &worker : workers)
    {
        worker.join();
    }

    const std::size_t expected =
        static_cast<std::size_t>(thread_count) * diagnostics_per_thread;
    REQUIRE(engine.warning_count() == expected);
    REQUIRE(engine.error_count() == 0);
    REQUIRE(engine.buffered_count() == expected);

    engine.finish();

    REQUIRE(collector->count() == expected);
    const auto diagnostics = collector->diagnostics();
    for (const Diagnostic &diagnostic : diagnostics)
    {
        REQUIRE(diagnostic.severity() == Severity::Warning);
        REQUIRE(diagnostic.code() == std::string("unused-variable"));
    }
}

TEST_CASE("DiagnosticEngine avoids formatting disabled diagnostics",
          "[diag][engine][zero-cost]")
{
    DiagnosticOptions options;
    options.suppress_warnings = true;

    DiagnosticEngine engine(options);
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    bool format_called = false;
    auto builder = engine.emit(DiagID::warn_unused_variable, SourceSpan{1, 0, 1},
                               FormatProbe{&format_called});

    REQUIRE_FALSE(builder.active());
    REQUIRE_FALSE(format_called);
    REQUIRE(engine.warning_count() == 0);
    REQUIRE(engine.buffered_count() == 0);

    engine.finish();
    REQUIRE(collector->count() == 0);

    DiagnosticEngine enabled_engine;
    bool enabled_format_called = false;
    enabled_engine.emit(DiagID::warn_unused_variable, SourceSpan{1, 0, 1},
                        FormatProbe{&enabled_format_called});
    REQUIRE(enabled_format_called);
    REQUIRE(enabled_engine.warning_count() == 1);
}

TEST_CASE("DiagnosticEngine fans out to multiple pluggable emitters",
          "[diag][engine][pluggable]")
{
    DiagnosticEngine engine;
    auto first = std::make_shared<CountingEmitter>();
    auto second = std::make_shared<CountingEmitter>();
    engine.add_emitter(first);
    engine.add_emitter(second);

    std::vector<Diagnostic> received;
    auto callback = std::make_shared<CallbackEmitter>(
        [&received](const Diagnostic &diagnostic)
        { received.push_back(diagnostic); });
    engine.add_emitter(callback);

    REQUIRE(engine.emitter_count() == 3);

    engine.error("fan out");
    engine.finish();

    REQUIRE(first->emits == 1);
    REQUIRE(first->flushes == 1);
    REQUIRE(first->finishes == 1);
    REQUIRE(second->emits == 1);
    REQUIRE(second->flushes == 1);
    REQUIRE(second->finishes == 1);
    REQUIRE(received.size() == 1);
    REQUIRE(received[0].message() == std::string("fan out"));

    engine.clear_emitters();
    REQUIRE(engine.emitter_count() == 0);
}

TEST_CASE("DiagnosticEngine sorts buffered diagnostics by source position",
          "[diag][engine][sorting]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("sort.cora", "abcdefghijklmnop\n");

    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    engine.error("second").primary(SourceSpan{file_id, 10, 11});
    engine.error("first").primary(SourceSpan{file_id, 5, 6});
    engine.finish();

    const auto diagnostics = collector->diagnostics();
    REQUIRE(diagnostics.size() == 2);
    REQUIRE(diagnostics[0].message() == std::string("first"));
    REQUIRE(diagnostics[1].message() == std::string("second"));
}

TEST_CASE("DiagnosticEngine promotes warnings with warnings_as_errors",
          "[diag][engine][werror]")
{
    DiagnosticOptions options;
    options.warnings_as_errors = true;

    DiagnosticEngine engine(options);
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    engine.warning("careful");
    engine.finish();

    REQUIRE(engine.error_count() == 1);
    REQUIRE(engine.warning_count() == 0);
    REQUIRE(collector->last().has_value());
    REQUIRE(collector->last()->severity() == Severity::Error);
}

TEST_CASE("DiagnosticEngine suppresses warnings with -w", "[diag][engine][w]")
{
    DiagnosticOptions options;
    options.suppress_warnings = true;

    DiagnosticEngine engine(options);
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    engine.warning("hidden");
    engine.emit(DiagID::warn_unused_variable, SourceSpan{1, 0, 1}, "value");
    engine.finish();

    REQUIRE(engine.warning_count() == 0);
    REQUIRE(collector->count() == 0);
}

TEST_CASE("DiagnosticEngine applies warning flags to direct reports",
          "[diag][engine][werror]")
{
    DiagnosticEngine engine;
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    engine.config().disable_warning(DiagID::warn_unused_variable);
    engine.report(Diagnostic::warning("unused", "unused-variable"));
    REQUIRE(engine.skipped_count() == 1);
    engine.finish();
    REQUIRE(collector->count() == 0);

    DiagnosticEngine werror_engine;
    auto werror_collector = std::make_shared<CollectingEmitter>();
    werror_engine.add_emitter(werror_collector);
    werror_engine.config().set_all_werror(true);
    werror_engine.config().set_werror(DiagID::warn_unused_variable, false);

    werror_engine.report(Diagnostic::warning("unused", "unused-variable"));
    werror_engine.finish();

    REQUIRE(werror_collector->count() == 1);
    REQUIRE(werror_collector->last()->severity() == Severity::Warning);
}

TEST_CASE("DiagnosticEngine keeps the error-limit notice after located errors",
          "[diag][engine][limit][sorting]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("limit.cora", "let value = 1;\n");

    DiagnosticOptions options;
    options.error_limit = 1;

    DiagnosticEngine engine(options);
    auto collector = std::make_shared<CollectingEmitter>();
    engine.add_emitter(collector);

    engine.error("first").primary(SourceSpan{file_id, 4, 9}, "value");
    engine.error("second").primary(SourceSpan{file_id, 0, 3}, "let");
    engine.finish();

    const auto diagnostics = collector->diagnostics();
    REQUIRE(diagnostics.size() == 2);
    REQUIRE(diagnostics.front().message() == std::string("first"));
    REQUIRE(diagnostics.back().severity() == Severity::Fatal);
    REQUIRE(diagnostics.back().message() ==
            std::string("too many errors emitted, stopping now"));
}

TEST_CASE("DiagnosticEngine honors show_option for terminal output",
          "[diag][engine][output]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("options.cora", "let value = 1;\n");

    DiagnosticOptions options;
    options.color = false;
    options.show_summary = false;
    options.show_option = false;

    DiagnosticEngine engine(options);
    std::ostringstream output;
    engine.add_output(cora::diag::DiagnosticFormat::Terminal, source_manager,
                      output);

    engine.emit(DiagID::warn_unused_variable, SourceSpan{file_id, 4, 9},
                "value");
    engine.finish();

    REQUIRE(output.str().find("unused variable 'value'") != std::string::npos);
    REQUIRE(output.str().find("[-Wunused-variable]") == std::string::npos);
}
