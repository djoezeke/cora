#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Lexer/Lexer.h>
#include <Cora/Lexer/TokenStream.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cstddef>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
    using cora::core::FileID;
    using cora::core::SourceManager;
    using cora::diag::DiagnosticEngine;
    using cora::lexer::Config;
    using cora::lexer::Lexer;
    using cora::lexer::TokenStream;
    using Tag = Token::Tag;

    struct Fixture
    {
        SourceManager sm;
        DiagnosticEngine de;
        FileID id;
        Lexer lexer;
        TokenStream stream;

        explicit Fixture(std::string source, Config config = {},
                         std::string name = "test.cora")
            : id(sm.add_virtual_file(std::move(name), std::move(source))),
              lexer(id, sm, de, config)
        {
            lexer.lex(stream);
        }

        [[nodiscard]] std::vector<Token::Tag> tags() const
        {
            std::vector<Token::Tag> result;
            for (std::size_t index = 0; index < stream.size(); ++index)
            {
                result.push_back(stream.tag(index));
            }
            return result;
        }

        [[nodiscard]] std::vector<Token::Tag> content_tags() const
        {
            auto result = tags();
            if (!result.empty() && result.back() == Tag::T_EOF)
            {
                result.pop_back();
            }
            return result;
        }

        [[nodiscard]] std::size_t content_size() const
        {
            return stream.size() == 0 ? 0 : stream.size() - 1;
        }
    };

    std::vector<Tag> expected(std::initializer_list<Tag> tags)
    {
        return std::vector<Tag>(tags);
    }
} // namespace

TEST_CASE("Lexer handles identifiers, keywords, Unicode and line flags",
          "[lexer][unicode]")
{
    Fixture fixture("let caf\xC3\xA9 = 42;\nconst \xE6\x97\xA5\xE6\x9C\xAC = caf\xC3\xA9 + 1;\n");
    REQUIRE(fixture.de.error_count() == 0);
    REQUIRE(fixture.tags() ==
            expected({Tag::T_LET, Tag::T_IDENT, Tag::T_ASSIGN, Tag::T_INTLIT,
                      Tag::T_SEMI, Tag::T_CONST, Tag::T_IDENT, Tag::T_ASSIGN,
                      Tag::T_IDENT, Tag::T_PLUS, Tag::T_INTLIT, Tag::T_SEMI,
                      Tag::T_EOF}));

    const std::uint32_t cafe_id =
        static_cast<std::uint32_t>(fixture.stream.value(1));
    REQUIRE(fixture.lexer.interner().lookup(cafe_id) == "caf\xC3\xA9");
    REQUIRE(static_cast<std::uint32_t>(fixture.stream.value(8)) == cafe_id);

    const std::uint32_t cjk_id =
        static_cast<std::uint32_t>(fixture.stream.value(6));
    REQUIRE(fixture.lexer.interner().lookup(cjk_id) ==
            "\xE6\x97\xA5\xE6\x9C\xAC");

    REQUIRE(static_cast<std::uint32_t>(fixture.stream.value(0)) ==
            fixture.lexer.interner().find("let"));
    REQUIRE((fixture.stream.flag(0) & TOKEN_AT_LINE_START) != 0);
    REQUIRE((fixture.stream.flag(5) & TOKEN_AT_LINE_START) != 0);
    REQUIRE((fixture.stream.flag(1) & TOKEN_AT_LINE_START) == 0);
}

TEST_CASE("Lexer recognizes all keyword tags", "[lexer][keywords]")
{
    Fixture fixture(
        "import struct enum type fn func export let var mut extern const "
        "if else while for in match defer return break continue true false "
        "null bool");
    REQUIRE(fixture.de.error_count() == 0);
    REQUIRE(fixture.content_tags() ==
            expected({Tag::T_IMPORT, Tag::T_STRUCT, Tag::T_ENUM, Tag::T_TYPE,
                      Tag::T_FN, Tag::T_FN, Tag::T_EXPORT, Tag::T_LET,
                      Tag::T_VAR, Tag::T_VAR, Tag::T_EXTERN, Tag::T_CONST,
                      Tag::T_IF, Tag::T_ELSE, Tag::T_WHILE, Tag::T_FOR,
                      Tag::T_IN, Tag::T_MATCH, Tag::T_DEFER, Tag::T_RETURN,
                      Tag::T_BREAK, Tag::T_CONTINUE, Tag::T_TRUE, Tag::T_FALSE,
                      Tag::T_NULLLIT, Tag::T_TBOOL}));
}

TEST_CASE("Lexer recognizes every punctuation tag", "[lexer][operators]")
{
    Fixture fixture(
        "( ) [ ] { } , ; : ? :: . .. .= .== .!= + - * / % += -= *= /= %= "
        "&= |= ^= ++ -- = == != < > <= >= << >> <<= >>= ! ~ & | ^ && || -> =>");
    REQUIRE(fixture.de.error_count() == 0);
    REQUIRE(fixture.content_tags() ==
            expected({Tag::T_LPAREN, Tag::T_RPAREN, Tag::T_LBRACKET,
                      Tag::T_RBRACKET, Tag::T_LCURLY, Tag::T_RCURLY, Tag::T_COMMA,
                      Tag::T_SEMI, Tag::T_COLON, Tag::T_QUESTION,
                      Tag::T_COLONCOLON, Tag::T_DOT, Tag::T_DOTDOT,
                      Tag::T_DOTASSIGN, Tag::T_DOTEQ, Tag::T_DOTNEQ, Tag::T_PLUS,
                      Tag::T_MINUS, Tag::T_MUL, Tag::T_DIV, Tag::T_MOD,
                      Tag::T_PLUSEQ, Tag::T_MINUSEQ, Tag::T_MULEQ, Tag::T_DIVEQ,
                      Tag::T_MODEQ, Tag::T_ANDEQ, Tag::T_OREQ, Tag::T_XOREQ,
                      Tag::T_INC, Tag::T_DEC, Tag::T_ASSIGN, Tag::T_EQ,
                      Tag::T_NEQ, Tag::T_LT, Tag::T_GT, Tag::T_LTEQ, Tag::T_GTEQ,
                      Tag::T_SHL, Tag::T_SHR, Tag::T_SHLEQ, Tag::T_SHREQ,
                      Tag::T_NOT, Tag::T_BITNOT, Tag::T_BITAND, Tag::T_BITOR,
                      Tag::T_XOR, Tag::T_ANDAND, Tag::T_OROR, Tag::T_ARROW,
                      Tag::T_FATARROW}));
}

TEST_CASE("Lexer decodes numeric and character literals", "[lexer][literals]")
{
    Fixture fixture(
        "0 42 1_000 0xFF 0b1010 0o755 3.14 .5 6.02e23 1e-3 7f "
        "\"hello\\nworld\" 'A' '\\n' \"\xF0\x9D\x84\x9E\" '\xC3\xA9' '\\x41'");
    REQUIRE(fixture.de.error_count() == 0);

    const std::vector<Tag> tags = fixture.content_tags();
    REQUIRE(tags.size() == 17);
    for (std::size_t index = 0; index < 6; ++index)
    {
        REQUIRE(tags[index] == Tag::T_INTLIT);
    }
    for (std::size_t index = 6; index < 11; ++index)
    {
        REQUIRE(tags[index] == Tag::T_FLTLIT);
    }
    REQUIRE(tags[11] == Tag::T_STRLIT);
    REQUIRE(tags[12] == Tag::T_CHARLIT);
    REQUIRE(tags[13] == Tag::T_CHARLIT);
    REQUIRE(tags[14] == Tag::T_STRLIT);
    REQUIRE(tags[15] == Tag::T_CHARLIT);
    REQUIRE(tags[16] == Tag::T_CHARLIT);

    REQUIRE(fixture.stream.token(0).int_val == 0);
    REQUIRE(fixture.stream.token(1).int_val == 42);
    REQUIRE(fixture.stream.token(2).int_val == 1000);
    REQUIRE(fixture.stream.token(3).int_val == 255);
    REQUIRE(fixture.stream.token(4).int_val == 10);
    REQUIRE(fixture.stream.token(5).int_val == 493);

    REQUIRE_THAT(fixture.stream.token(6).float_val,
                 Catch::Matchers::WithinAbs(3.14, 1e-12));
    REQUIRE_THAT(fixture.stream.token(7).float_val,
                 Catch::Matchers::WithinAbs(0.5, 1e-12));
    REQUIRE_THAT(fixture.stream.token(8).float_val,
                 Catch::Matchers::WithinRel(6.02e23, 1e-12));
    REQUIRE_THAT(fixture.stream.token(9).float_val,
                 Catch::Matchers::WithinAbs(0.001, 1e-15));
    REQUIRE_THAT(fixture.stream.token(10).float_val,
                 Catch::Matchers::WithinAbs(7.0, 1e-12));

    const auto string_id = static_cast<std::uint32_t>(fixture.stream.value(11));
    REQUIRE(fixture.lexer.interner().lookup(string_id) == "hello\nworld");
    const auto clef_id = static_cast<std::uint32_t>(fixture.stream.value(14));
    REQUIRE(fixture.lexer.interner().lookup(clef_id) == "\xF0\x9D\x84\x9E");

    REQUIRE(fixture.stream.token(12).int_val == 'A');
    REQUIRE(fixture.stream.token(13).int_val == '\n');
    REQUIRE(fixture.stream.token(15).int_val == 0xE9);
    REQUIRE(fixture.stream.token(16).int_val == 0x41);
}

TEST_CASE("Lexer saturates out-of-range floating literals", "[lexer][literals]")
{
    Fixture fixture("1e400 1e-400");
    REQUIRE(fixture.de.error_count() == 0);
    REQUIRE(std::isinf(fixture.stream.token(0).float_val));
    REQUIRE(fixture.stream.token(0).float_val > 0.0);
    REQUIRE(fixture.stream.token(1).float_val == 0.0);
}

TEST_CASE("Lexer skips comments, Unicode whitespace and CRLF", "[lexer][comments]")
{
    Fixture fixture(
        "// line\n"
        "/// doc\n"
        "//! doc\n"
        "/** block **/\n"
        "/*! doc */\n"
        "/* outer /* nested */ end */\n"
        "let\xC2\xA0x\xE3\x80\x80=\xE2\x80\xA8\xC2\x85 1;\r\n");
    REQUIRE(fixture.de.error_count() == 0);
    REQUIRE(fixture.content_tags() ==
            expected({Tag::T_LET, Tag::T_IDENT, Tag::T_ASSIGN, Tag::T_INTLIT,
                      Tag::T_SEMI}));
    REQUIRE((fixture.stream.flag(0) & TOKEN_AT_LINE_START) != 0);
}

TEST_CASE("Lexer can disable comments and warn on multiline comments",
          "[lexer][comments][config]")
{
    Config no_comments;
    no_comments.enableComments = false;
    Fixture fixture("//x\nlet x", no_comments);
    REQUIRE(fixture.content_tags() ==
            expected({Tag::T_DIV, Tag::T_DIV, Tag::T_IDENT, Tag::T_LET,
                      Tag::T_IDENT}));

    Config warn;
    warn.warnOnMultilineComments = true;
    Fixture warned("/* a\nb */\nlet x", warn);
    REQUIRE(warned.de.warning_count() == 1);
    REQUIRE(warned.content_tags() ==
            expected({Tag::T_LET, Tag::T_IDENT}));
}

TEST_CASE("Lexer is error tolerant for bad input", "[lexer][errors]")
{
    Fixture invalid_utf8("caf\xC3\xA9 \xFF = 'A';");
    REQUIRE(invalid_utf8.de.error_count() == 1);
    REQUIRE(invalid_utf8.content_tags() ==
            expected({Tag::T_IDENT, Tag::T_ASSIGN, Tag::T_CHARLIT,
                      Tag::T_SEMI}));

    Fixture invalid_char("let `x` = 1;");
    REQUIRE(invalid_char.de.error_count() == 2);
    REQUIRE(invalid_char.content_tags() ==
            expected({Tag::T_LET, Tag::T_IDENT, Tag::T_ASSIGN, Tag::T_INTLIT,
                      Tag::T_SEMI}));

    Fixture unterminated("\"abc\nlet x = 'y");
    REQUIRE(unterminated.de.error_count() == 2);
    REQUIRE(unterminated.content_tags() ==
            expected({Tag::T_STRLIT, Tag::T_LET, Tag::T_IDENT, Tag::T_ASSIGN,
                      Tag::T_CHARLIT}));

    Fixture unterminated_comment("let x = 1; /* never closed");
    REQUIRE(unterminated_comment.de.error_count() == 1);
    REQUIRE(unterminated_comment.content_tags() ==
            expected({Tag::T_LET, Tag::T_IDENT, Tag::T_ASSIGN, Tag::T_INTLIT,
                      Tag::T_SEMI}));

    Fixture escapes("\"\\q\" 1__0 1_");
    REQUIRE(escapes.de.warning_count() == 4);
}

TEST_CASE("Lexer produces exact source spans", "[lexer][spans]")
{
    Fixture fixture("let x = 1;\n\tx += 2;");
    REQUIRE(fixture.de.error_count() == 0);

    REQUIRE(fixture.stream.start(0) == 0);
    REQUIRE(fixture.stream.end(0) == 3);
    REQUIRE(fixture.stream.start(1) == 4);
    REQUIRE(fixture.stream.end(1) == 5);
    REQUIRE(fixture.stream.start(3) == 8);
    REQUIRE(fixture.stream.end(3) == 9);
    REQUIRE(fixture.stream.start(5) == 12);
    REQUIRE(fixture.stream.end(5) == 13);
    REQUIRE(fixture.stream.start(fixture.stream.size() - 1) ==
            fixture.lexer.source().size());
    REQUIRE(fixture.stream.start(fixture.stream.size() - 1) ==
            fixture.stream.end(fixture.stream.size() - 1));

    const auto resolved = fixture.sm.resolve_location(fixture.stream.span(5));
    REQUIRE(resolved.has_value());
    REQUIRE(resolved->line == 2);
    REQUIRE(resolved->column == 2);
}

TEST_CASE("Lexer scales to large sources and reuses token storage",
          "[lexer][performance]")
{
    constexpr std::size_t line_count = 20000;
    const std::string unit = "let value = 0x2A + 3.5e2; // comment\n";
    std::string source;
    source.reserve(unit.size() * line_count);
    for (std::size_t line = 0; line < line_count; ++line)
    {
        source.append(unit);
    }

    Fixture fixture(std::move(source), {}, "large.cora");
    REQUIRE(fixture.de.error_count() == 0);

    const std::size_t expected_tokens = line_count * 7 + 1;
    REQUIRE(fixture.stream.size() == expected_tokens);

    const std::size_t capacity = fixture.stream.capacity();
    const auto start = std::chrono::steady_clock::now();
    fixture.lexer.lex(fixture.stream);
    const auto elapsed = std::chrono::duration<double, std::milli>(
                             std::chrono::steady_clock::now() - start)
                             .count();

    REQUIRE(fixture.stream.size() == expected_tokens);
    REQUIRE(fixture.stream.capacity() == capacity);
    REQUIRE(fixture.de.error_count() == 0);

    INFO("lexed " << expected_tokens << " tokens in " << elapsed << " ms");
    REQUIRE(elapsed < 5000.0);
}
