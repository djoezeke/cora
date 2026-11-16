#include <Cora/Diag/Diagnostic.h>
#include <Cora/Diag/DiagnosticCodes.h>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

using cora::diag::Applicability;
using cora::diag::DiagID;
using cora::diag::Diagnostic;
using cora::diag::LabelStyle;
using cora::diag::Severity;
using cora::diag::SourceSpan;
using cora::diag::format_diag;
using cora::diag::lookup_warning;

TEST_CASE("Diagnostic stores code, message, severity, labels and hints",
          "[diag][diagnostic]")
{
    const SourceSpan primary{1, 2, 5};
    const SourceSpan secondary{1, 6, 9};

    Diagnostic diagnostic = Diagnostic::error("something failed", "E1001");
    diagnostic.with_primary(primary, "primary label")
        .with_secondary(secondary, "secondary label")
        .with_hint("try declaring it first")
        .with_suggestion(primary, "replacement", "replace the expression",
                         Applicability::MaybeIncorrect);

    REQUIRE(diagnostic.severity() == Severity::Error);
    REQUIRE(diagnostic.code() == std::string("E1001"));
    REQUIRE(diagnostic.message() == std::string("something failed"));
    REQUIRE(diagnostic.has_code());
    REQUIRE(diagnostic.is_error());
    REQUIRE_FALSE(diagnostic.is_warning());

    REQUIRE(diagnostic.labels().size() == 2);
    REQUIRE(diagnostic.labels()[0].span.start_offset == 2);
    REQUIRE(diagnostic.labels()[0].message == std::string("primary label"));
    REQUIRE(diagnostic.labels()[0].style == LabelStyle::Primary);
    REQUIRE(diagnostic.labels()[1].style == LabelStyle::Secondary);

    REQUIRE(diagnostic.hints().size() == 1);
    REQUIRE(diagnostic.hints()[0] == std::string("try declaring it first"));
    REQUIRE(diagnostic.notes().size() == 1);

    REQUIRE(diagnostic.suggestions().size() == 1);
    REQUIRE(diagnostic.suggestions()[0].replacement == std::string("replacement"));
    REQUIRE(diagnostic.suggestions()[0].applicability ==
            Applicability::MaybeIncorrect);
}

TEST_CASE("Diagnostic primary span prefers a primary label", "[diag][diagnostic]")
{
    Diagnostic diagnostic = Diagnostic::warning("careful");
    diagnostic.with_secondary(SourceSpan{3, 10, 20}, "secondary")
        .with_primary(SourceSpan{3, 2, 4}, "primary");

    const SourceSpan span = diagnostic.primary_span();
    REQUIRE(span.file_id == 3);
    REQUIRE(span.start_offset == 2);
    REQUIRE(span.end_offset == 4);
    REQUIRE(diagnostic.span().file_id == 3);

    Diagnostic explicit_span = Diagnostic::error("explicit");
    explicit_span.with_span(SourceSpan{4, 7, 9});
    REQUIRE(explicit_span.primary_span().file_id == 4);
    REQUIRE(explicit_span.primary_span().start_offset == 7);

    Diagnostic empty;
    REQUIRE_FALSE(empty.primary_span().is_valid());
}

TEST_CASE("Diagnostic factories create the requested severity", "[diag][diagnostic]")
{
    REQUIRE(Diagnostic::error("x").severity() == Severity::Error);
    REQUIRE(Diagnostic::warning("x").severity() == Severity::Warning);
    REQUIRE(Diagnostic::remark("x").severity() == Severity::Remark);
    REQUIRE(Diagnostic::note("x").severity() == Severity::Note);
    REQUIRE(Diagnostic::help("x").severity() == Severity::Help);
    REQUIRE(Diagnostic::fatal("x").severity() == Severity::Fatal);

    REQUIRE(cora::diag::is_error_severity(Severity::Fatal));
    REQUIRE(cora::diag::is_error_severity(Severity::Error));
    REQUIRE_FALSE(cora::diag::is_error_severity(Severity::Warning));
    REQUIRE(std::string_view(cora::diag::severity_name(Severity::Warning)) ==
            std::string_view("warning"));
}

TEST_CASE("Diagnostic can be cleared and reused", "[diag][diagnostic]")
{
    Diagnostic diagnostic = Diagnostic::error("bad", "E0001");
    diagnostic.with_primary(SourceSpan{1, 0, 1}, "here").with_hint("hint");
    diagnostic.clear();

    REQUIRE(diagnostic.severity() == Severity::Error);
    REQUIRE(diagnostic.code().empty());
    REQUIRE(diagnostic.message().empty());
    REQUIRE(diagnostic.labels().empty());
    REQUIRE(diagnostic.hints().empty());
    REQUIRE(diagnostic.suggestions().empty());
}

TEST_CASE("Diagnostic accepts empty string views", "[diag][diagnostic]")
{
    Diagnostic diagnostic =
        Diagnostic::error(std::string_view{}, std::string_view{});
    diagnostic.add_label(SourceSpan{1, 0, 1}, std::string_view{});
    diagnostic.add_hint(std::string_view{});
    diagnostic.add_suggestion(SourceSpan{1, 0, 1}, std::string_view{},
                              std::string_view{});

    REQUIRE(diagnostic.code().empty());
    REQUIRE(diagnostic.message().empty());
    REQUIRE(diagnostic.labels().size() == 1);
    REQUIRE(diagnostic.labels()[0].message.empty());
    REQUIRE(diagnostic.hints().size() == 1);
    REQUIRE(diagnostic.hints()[0].empty());
    REQUIRE(diagnostic.suggestions().size() == 1);
    REQUIRE(diagnostic.suggestions()[0].replacement.empty());
}

TEST_CASE("Diagnostic code table is internally consistent", "[diag][codes]")
{
    STATIC_REQUIRE(cora::diag::DiagTable.size() == cora::diag::NumDiags);
    STATIC_REQUIRE(cora::diag::is_valid_diag_id(DiagID::err_type_mismatch));
    STATIC_REQUIRE_FALSE(cora::diag::is_valid_diag_id(DiagID::anonymous));

    for (std::size_t index = 0; index < cora::diag::NumDiags; ++index)
    {
        const auto id = static_cast<DiagID>(index);
        const auto &info = cora::diag::diag_info(id);
        if (id == DiagID::anonymous)
        {
            REQUIRE(std::string_view(info.code).empty());
            continue;
        }
        REQUIRE(info.code != nullptr);
        REQUIRE_FALSE(std::string_view(info.code).empty());
        REQUIRE(info.format != nullptr);
        REQUIRE_FALSE(std::string_view(info.format).empty());
    }
}

TEST_CASE("Diagnostic formatting substitutes positional arguments",
          "[diag][codes]")
{
    REQUIRE(format_diag(DiagID::warn_unused_variable, "value") ==
            std::string("unused variable 'value'"));
    REQUIRE(format_diag(DiagID::err_invalid_char, '?') ==
            std::string("invalid character '?'"));
    REQUIRE(format_diag(DiagID::err_expected_token, ";", "identifier") ==
            std::string("expected ';', found 'identifier'"));
    REQUIRE(format_diag(DiagID::err_wrong_arity, 2, 3) ==
            std::string("wrong number of arguments: expected 2, got 3"));
    REQUIRE(format_diag(DiagID::err_unexpected_eof) ==
            std::string("unexpected end of file"));
    REQUIRE(format_diag(DiagID::err_type_mismatch, "%1", "actual") ==
            std::string("type mismatch: expected '%1', found 'actual'"));
    REQUIRE(format_diag(DiagID::warn_unused_variable, std::string_view("x%0")) ==
            std::string("unused variable 'x%0'"));
    REQUIRE(format_diag(DiagID::warn_unused_variable, std::string_view{}) ==
            std::string("unused variable ''"));
}

TEST_CASE("Warning lookup finds named diagnostics", "[diag][codes]")
{
    const auto unused = lookup_warning("unused-variable");
    REQUIRE(unused.has_value());
    REQUIRE(*unused == DiagID::warn_unused_variable);

    const auto pedantic = lookup_warning("gnu-extension");
    REQUIRE(pedantic.has_value());
    REQUIRE(*pedantic == DiagID::warn_gnu_extension);

    REQUIRE_FALSE(lookup_warning("does-not-exist").has_value());
}
