#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEmitter.h>
#include <Cora/Diag/DiagnosticEngine.h>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

using cora::core::SourceManager;
using cora::core::SourceSpan;
using cora::diag::CallbackEmitter;
using cora::diag::CollectingEmitter;
using cora::diag::Diagnostic;
using cora::diag::DiagnosticEngine;
using cora::diag::DiagnosticFormat;
using cora::diag::JsonEmitter;
using cora::diag::LspEmitter;
using cora::diag::TextEmitter;

TEST_CASE("TextEmitter renders clang-like source snippets", "[diag][emitter][text]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("test.cora", "let value = 1;\n");

    Diagnostic diagnostic = Diagnostic::error("type mismatch", "E3002");
    diagnostic.with_primary(SourceSpan{file_id, 4, 9}, "value")
        .with_hint("declare a type")
        .with_suggestion(SourceSpan{file_id, 4, 9}, "other", "rename the value");

    std::ostringstream output;
    TextEmitter emitter(source_manager, output, false);
    emitter.emit(diagnostic);
    emitter.finish();

    const std::string text = output.str();
    REQUIRE(text.find("test.cora:1:5: error: type mismatch [E3002]") !=
            std::string::npos);
    REQUIRE(text.find("let value = 1;") != std::string::npos);
    REQUIRE(text.find("^^^^^ value") != std::string::npos);
    REQUIRE(text.find("note: declare a type") != std::string::npos);
    REQUIRE(text.find("replace with: other") != std::string::npos);
    REQUIRE(text.find("1 error, 0 warnings generated.") != std::string::npos);
}

TEST_CASE("TextEmitter prints clang-style warning option codes",
          "[diag][emitter][text]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("warn.cora", "let unused = 1;\n");

    Diagnostic diagnostic = Diagnostic::warning("unused variable 'unused'",
                                                "unused-variable");
    diagnostic.with_primary(SourceSpan{file_id, 4, 10}, "unused");

    std::ostringstream output;
    TextEmitter emitter(source_manager, output, false, true, true, false, true,
                        false);
    emitter.emit(diagnostic);
    emitter.finish();

    REQUIRE(output.str().find("[-Wunused-variable]") != std::string::npos);
}

TEST_CASE("TextEmitter can hide warning option codes", "[diag][emitter][text]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("warn.cora", "let unused = 1;\n");

    Diagnostic diagnostic =
        Diagnostic::warning("unused variable 'unused'", "unused-variable");
    diagnostic.with_primary(SourceSpan{file_id, 4, 10}, "unused");

    std::ostringstream output;
    TextEmitter emitter(source_manager, output, false, true, true, false, true,
                        false, false);
    emitter.emit(diagnostic);
    emitter.finish();

    REQUIRE(output.str().find("unused variable 'unused'") != std::string::npos);
    REQUIRE(output.str().find("[-Wunused-variable]") == std::string::npos);
}

TEST_CASE("JsonEmitter produces escaped machine-readable output",
          "[diag][emitter][json]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("json.cora", "let value = 1;\n");

    Diagnostic diagnostic = Diagnostic::error("quote \" slash \\ newline\n",
                                              "E1001");
    diagnostic.with_primary(SourceSpan{file_id, 4, 9}, "label")
        .with_hint("hint");

    std::ostringstream output;
    JsonEmitter emitter(output, &source_manager);
    emitter.emit(diagnostic);
    emitter.finish();

    const std::string json = output.str();
    REQUIRE(json.front() == '[');
    REQUIRE(json.find("\"severity\":\"error\"") != std::string::npos);
    REQUIRE(json.find("\"code\":\"E1001\"") != std::string::npos);
    REQUIRE(json.find("quote \\\" slash \\\\ newline\\n") !=
            std::string::npos);
    REQUIRE(json.find("\"file\":\"json.cora\"") != std::string::npos);
    REQUIRE(json.find("\"line\":1") != std::string::npos);
    REQUIRE(json.find("\"labels\":[") != std::string::npos);
    REQUIRE(json.find("\"hints\":[\"hint\"]") != std::string::npos);
}

TEST_CASE("JsonEmitter emits an empty array when nothing was reported",
          "[diag][emitter][json]")
{
    SourceManager source_manager;
    std::ostringstream output;
    JsonEmitter emitter(output, &source_manager);
    emitter.finish();
    emitter.finish();

    REQUIRE(output.str() == std::string("[]\n"));
}

TEST_CASE("LspEmitter groups diagnostics by file", "[diag][emitter][lsp]")
{
    SourceManager source_manager;
    const auto first_file =
        source_manager.add_virtual_file("first.cora", "let value = 1;\n");
    const auto second_file =
        source_manager.add_virtual_file("second.cora", "let other = 2;\n");

    Diagnostic first = Diagnostic::warning("careful", "unused-variable");
    first.with_primary(SourceSpan{first_file, 4, 9}, "value");

    Diagnostic second = Diagnostic::error("bad", "E3001");
    second.with_primary(SourceSpan{second_file, 4, 9}, "other");

    std::ostringstream output;
    LspEmitter emitter(source_manager, output);
    emitter.emit(first);
    emitter.emit(second);
    emitter.finish();

    const std::string lsp = output.str();
    REQUIRE(lsp.find("\"method\":\"textDocument/publishDiagnostics\"") !=
            std::string::npos);
    REQUIRE(lsp.find("\"uri\":\"file://first.cora\"") != std::string::npos);
    REQUIRE(lsp.find("\"uri\":\"file://second.cora\"") != std::string::npos);
    REQUIRE(lsp.find("\"line\":0") != std::string::npos);
    REQUIRE(lsp.find("\"character\":4") != std::string::npos);
    REQUIRE(lsp.find("\"severity\":2") != std::string::npos);
    REQUIRE(lsp.find("\"severity\":1") != std::string::npos);
    REQUIRE(lsp.find("\"source\":\"cora\"") != std::string::npos);

    std::size_t notifications = 0;
    std::size_t position = 0;
    while ((position = lsp.find("\"method\":\"textDocument/publishDiagnostics\"",
                                position)) != std::string::npos)
    {
        ++notifications;
        ++position;
    }
    REQUIRE(notifications == 2);
}

TEST_CASE("LspEmitter publishes pending diagnostics on flush",
          "[diag][emitter][lsp]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("flush.cora", "let value = 1;\n");

    Diagnostic diagnostic = Diagnostic::error("bad", "E3001");
    diagnostic.with_primary(SourceSpan{file_id, 4, 9}, "value");

    std::ostringstream output;
    LspEmitter emitter(source_manager, output);
    emitter.emit(diagnostic);
    emitter.flush();

    const std::string first = output.str();
    REQUIRE(first.find("\"method\":\"textDocument/publishDiagnostics\"") !=
            std::string::npos);
    REQUIRE(first.find("\"message\":\"bad\"") != std::string::npos);

    emitter.finish();
    REQUIRE(output.str() == first);
}

TEST_CASE("CollectingEmitter stores diagnostics safely",
          "[diag][emitter][collecting]")
{
    CollectingEmitter emitter;
    emitter.emit(Diagnostic::error("one"));
    emitter.emit(Diagnostic::warning("two"));

    REQUIRE(emitter.count() == 2);
    REQUIRE(emitter.last().has_value());
    REQUIRE(emitter.last()->message() == std::string("two"));

    const auto diagnostics = emitter.take();
    REQUIRE(diagnostics.size() == 2);
    REQUIRE(emitter.count() == 0);

    emitter.emit(Diagnostic::note("three"));
    emitter.clear();
    REQUIRE(emitter.count() == 0);
}

TEST_CASE("CallbackEmitter forwards diagnostics to a callback",
          "[diag][emitter][callback]")
{
    std::vector<std::string> messages;
    CallbackEmitter emitter(
        [&messages](const Diagnostic &diagnostic)
        { messages.push_back(diagnostic.message()); });

    emitter.emit(Diagnostic::error("first"));
    emitter.emit(Diagnostic::warning("second"));

    REQUIRE(messages.size() == 2);
    REQUIRE(messages[0] == std::string("first"));
    REQUIRE(messages[1] == std::string("second"));
}

TEST_CASE("DiagnosticEngine can attach built-in JSON output",
          "[diag][engine][output]")
{
    SourceManager source_manager;
    const auto file_id =
        source_manager.add_virtual_file("engine.cora", "let value = 1;\n");

    DiagnosticEngine engine;
    std::ostringstream output;
    engine.add_output(DiagnosticFormat::Json, source_manager, output);

    engine.error("json error").primary(SourceSpan{file_id, 4, 9}, "value");
    engine.finish();

    REQUIRE(output.str().find("\"severity\":\"error\"") != std::string::npos);
    REQUIRE(output.str().find("\"message\":\"json error\"") !=
            std::string::npos);
}
