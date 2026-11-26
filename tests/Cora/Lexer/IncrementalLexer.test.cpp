#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Lexer/Lexer.h>
#include <Cora/Lexer/TokenNames.h>
#include <Cora/Lexer/TokenStream.h>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

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

        explicit Fixture(std::string source,
                         std::string name = "incremental.cora")
            : id(sm.add_virtual_file(std::move(name), std::move(source))),
              lexer(id, sm, de, Config{})
        {
            lexer.lex(stream);
        }

        [[nodiscard]] std::size_t content_size() const
        {
            return stream.size() == 0 ? 0 : stream.size() - 1;
        }
    };

    std::string read_source_file(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary);
        return std::string(std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>());
    }

    void require_same_tokens(const TokenStream &actual, const TokenStream &full)
    {
        REQUIRE(actual.size() == full.size());
        for (std::size_t index = 0; index < actual.size(); ++index)
        {
            CAPTURE(index);
            REQUIRE(actual.tag(index) == full.tag(index));
            REQUIRE(actual.start(index) == full.start(index));
            REQUIRE(actual.end(index) == full.end(index));
            REQUIRE(actual.flag(index) == full.flag(index));

            const Tag tag = actual.tag(index);
            if (tag == Tag::T_INTLIT || tag == Tag::T_CHARLIT ||
                tag == Tag::T_FLTLIT)
            {
                REQUIRE(actual.value(index) == full.value(index));
            }
        }
    }
} // namespace

TEST_CASE("Incremental lexer reports a no-op update", "[lexer][incremental]")
{
    Fixture fixture("let alpha = 1;\nlet beta = 2;\n");
    const auto result = fixture.lexer.update(fixture.lexer.source(), fixture.stream);

    REQUIRE_FALSE(result.changed);
    REQUIRE_FALSE(result.full_relex);
    REQUIRE(result.prefix_tokens == fixture.content_size());
    REQUIRE(result.suffix_tokens == 0);
    REQUIRE(result.relexed_tokens == 0);
    REQUIRE(result.relexed_bytes == 0);
}

TEST_CASE("Incremental lexer reuses prefix and suffix around an edit",
          "[lexer][incremental]")
{
    Fixture fixture("let alpha = 1;\nlet beta = 2;\n");
    const std::uint32_t beta_id = static_cast<std::uint32_t>(fixture.stream.value(6));

    const std::string updated = "let alpha = 123;\nlet beta = 2;\n";
    const auto result = fixture.lexer.update(updated, fixture.stream);

    REQUIRE(result.changed);
    REQUIRE_FALSE(result.full_relex);
    REQUIRE(result.prefix_tokens > 0);
    REQUIRE(result.suffix_tokens > 0);
    REQUIRE(result.relexed_tokens < fixture.content_size());
    REQUIRE(result.relexed_bytes < updated.size());

    REQUIRE(fixture.stream.token(3).int_val == 123);
    REQUIRE(fixture.stream.start(6) == updated.find("beta"));
    REQUIRE(static_cast<std::uint32_t>(fixture.stream.value(6)) == beta_id);
    REQUIRE((fixture.stream.flag(5) & TOKEN_AT_LINE_START) != 0);

    Fixture full(updated, "full.cora");
    require_same_tokens(fixture.stream, full.stream);
}

TEST_CASE("Incremental lexer handles insertions, removals and Unicode",
          "[lexer][incremental][unicode]")
{
    Fixture fixture("let alpha = 1;\nlet beta = 2;\n");

    std::string updated =
        "let alpha = 1;\n// inserted\nlet beta = 2;\n";
    auto result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    REQUIRE_FALSE(result.full_relex);
    REQUIRE(result.prefix_tokens + result.suffix_tokens > 0);
    {
        Fixture full(updated, "full_insert.cora");
        require_same_tokens(fixture.stream, full.stream);
    }

    updated = "let alpha = 1;\nlet beta = 2;\n";
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    {
        Fixture full(updated, "full_remove.cora");
        require_same_tokens(fixture.stream, full.stream);
    }

    updated = "let caf\xC3\xA9 = 1;\n";
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    {
        Fixture full(updated, "full_unicode.cora");
        require_same_tokens(fixture.stream, full.stream);
        REQUIRE(fixture.stream.identifier(1) == "caf\xC3\xA9");
    }

    updated += "// appended tail\nlet gamma = 3;\n";
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    REQUIRE(result.suffix_tokens == 0);
    {
        Fixture full(updated, "full_append.cora");
        require_same_tokens(fixture.stream, full.stream);
    }

    updated += "/* unterminated tail";
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    {
        Fixture full(updated, "full_append_comment.cora");
        require_same_tokens(fixture.stream, full.stream);
        REQUIRE(full.de.error_count() == 1);
    }
    REQUIRE(fixture.de.error_count() == 1);
}

TEST_CASE("Incremental lexer resynchronizes after comments and broken literals",
          "[lexer][incremental][comments][errors]")
{
    Fixture fixture("let a = 1; /* old */\nlet b = \"x\";\n");

    std::string updated = "let a = 1; /* new comment */\nlet b = \"x\";\n";
    auto result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    {
        Fixture full(updated, "full_comment.cora");
        require_same_tokens(fixture.stream, full.stream);
        REQUIRE(full.de.error_count() == 0);
    }
    REQUIRE(fixture.de.error_count() == 0);

    updated = "let a = 1; /* new comment */\nlet b = \"oops;\nlet c = 3;\n";
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    {
        Fixture full(updated, "full_broken.cora");
        require_same_tokens(fixture.stream, full.stream);
        REQUIRE(full.de.error_count() == 1);
    }
    REQUIRE(fixture.de.error_count() == 1);
}

TEST_CASE("Incremental lexer preserves line-start flags across edited comments",
          "[lexer][incremental][comments]")
{
    Fixture fixture("let x = 1;\n/* old */ let a = 2;\nlet b = 3;\n");
    REQUIRE((fixture.stream.flag(5) & TOKEN_AT_LINE_START) != 0);

    const std::string updated =
        "let x = 1;\n/* new comment */ let a = 2;\nlet b = 3;\n";
    const auto result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    REQUIRE(result.suffix_tokens > 0);

    Fixture full(updated, "full_comment_line.cora");
    require_same_tokens(fixture.stream, full.stream);
    REQUIRE((fixture.stream.flag(5) & TOKEN_AT_LINE_START) != 0);
}

TEST_CASE("Incremental lexing of syntax fixture matches a full re-lex",
          "[lexer][incremental][syntax]")
{
    const std::string path =
        std::string(CORA_SOURCE_DIR) + "/tests/fixtures/syntax.cora";
    const std::string initial = read_source_file(path);
    REQUIRE_FALSE(initial.empty());

    // The sample kept in the repository evolves, so grow it with a known
    // fixture before applying the incremental edits.
    const std::string sample =
        "\n// incremental fixture\n"
        "func incremental_sample() -> u32 {\n"
        "    println(\"Hello World\");\n"
        "    const values = [1, 2, 3, 4, 5];\n"
        "    return 0;\n"
        "}\n";
    std::string current = initial + sample;

    Fixture fixture(current, "syntax_incremental.cora");
    REQUIRE(fixture.de.error_count() == 0);

    const auto edits = [&current](std::string from, std::string to)
    {
        const auto position = current.find(from);
        REQUIRE(position != std::string::npos);
        current.replace(position, from.size(), to);
        return current;
    };

    std::string updated = edits("Hello World", "Hello Cora!");
    auto result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    REQUIRE_FALSE(result.full_relex);
    REQUIRE(result.relexed_bytes < updated.size());
    {
        INFO("edit 1: replace string");
        Fixture full(updated, "syntax_full_1.cora");
        require_same_tokens(fixture.stream, full.stream);
    }

    updated = edits("func incremental_sample()",
                    "// inserted\nfunc incremental_sample()");
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    REQUIRE(result.prefix_tokens + result.suffix_tokens > 0);
    {
        INFO("edit 2: insert comment");
        Fixture full(updated, "syntax_full_2.cora");
        require_same_tokens(fixture.stream, full.stream);
    }

    updated = edits("// incremental fixture", "");
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    {
        INFO("edit 3: remove line comment");
        Fixture full(updated, "syntax_full_3.cora");
        require_same_tokens(fixture.stream, full.stream);
    }

    updated = edits("[1, 2, 3, 4, 5]", "[9, 8, 7]");
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    {
        INFO("edit 4: shrink array");
        Fixture full(updated, "syntax_full_4.cora");
        require_same_tokens(fixture.stream, full.stream);
    }

    updated = edits("\"Hello Cora!\"", "\"caf\xC3\xA9 \xF0\x9D\x84\x9E\"");
    result = fixture.lexer.update(updated, fixture.stream);
    REQUIRE(result.changed);
    {
        INFO("edit 5: replace string with Unicode");
        Fixture full(updated, "syntax_full_5.cora");
        require_same_tokens(fixture.stream, full.stream);
    }

    REQUIRE(fixture.de.error_count() == 0);
}
