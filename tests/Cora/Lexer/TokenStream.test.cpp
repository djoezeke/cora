#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Lexer/Lexer.h>
#include <Cora/Lexer/TokenPool.h>
#include <Cora/Lexer/TokenStream.h>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace
{
    using cora::core::FileID;
    using cora::core::SourceManager;
    using cora::diag::DiagnosticEngine;
    using cora::lexer::Config;
    using cora::lexer::Lexer;
    using cora::lexer::TokenPool;
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
                         std::string name = "token_stream.cora")
            : id(sm.add_virtual_file(std::move(name), std::move(source))),
              lexer(id, sm, de, Config{})
        {
            lexer.lex(stream);
        }
    };

    std::vector<Tag> expected(std::initializer_list<Tag> tags)
    {
        return std::vector<Tag>(tags);
    }

    std::vector<Tag> tags_of(const TokenStream &stream)
    {
        std::vector<Tag> result;
        for (std::size_t index = 0; index < stream.size(); ++index)
        {
            result.push_back(stream.tag(index));
        }
        return result;
    }
} // namespace

TEST_CASE("TokenStream supports peek, lookahead, consume and backtrack",
          "[lexer][stream]")
{
    Fixture fixture("let x = 1;");
    TokenStream &stream = fixture.stream;

    REQUIRE(stream.cursor() == 0);
    REQUIRE(stream.check(Tag::T_LET));
    REQUIRE(stream.peek().tag == Tag::T_LET);
    REQUIRE(stream.peek(1).tag == Tag::T_IDENT);
    REQUIRE(stream.peek(2).tag == Tag::T_ASSIGN);
    REQUIRE(stream.span(0).start_offset == 0);
    REQUIRE(stream.span(0).end_offset == 3);

    Token consumed = stream.consume();
    REQUIRE(consumed.tag == Tag::T_LET);
    REQUIRE(stream.cursor() == 1);

    stream.consume(2);
    REQUIRE(stream.cursor() == 3);
    const auto mark = stream.mark();
    stream.consume(2);
    REQUIRE(stream.cursor() == 5);
    REQUIRE(stream.reset(mark));
    REQUIRE(stream.cursor() == 3);

    stream.backtrack(2);
    REQUIRE(stream.cursor() == 1);
    stream.backtrack(100);
    REQUIRE(stream.cursor() == 0);

    Token assigned{};
    REQUIRE_FALSE(stream.consume_if(Tag::T_ASSIGN, &assigned));
    REQUIRE(stream.consume_if(Tag::T_LET, &assigned));
    REQUIRE(assigned.tag == Tag::T_LET);
    REQUIRE(stream.cursor() == 1);

    REQUIRE(stream.peek(999).tag == Tag::T_EOF);
    REQUIRE(stream.peek(999).loc.start == fixture.lexer.source().size());
    REQUIRE_FALSE(stream.reset(stream.size() + 1));

    stream.consume(stream.size());
    REQUIRE(stream.at_end());
    REQUIRE(stream.remaining() == 0);
    REQUIRE(stream.consume().tag == Tag::T_EOF);
}

TEST_CASE("TokenStream exposes interned identifiers and spellings",
          "[lexer][stream][interning]")
{
    Fixture fixture("let caf\xC3\xA9 = \"s\"; caf\xC3\xA9");
    TokenStream &stream = fixture.stream;

    REQUIRE(stream.spelling(0) == "let");
    REQUIRE(stream.spelling(1) == "caf\xC3\xA9");
    REQUIRE(stream.identifier(1) == "caf\xC3\xA9");
    REQUIRE(stream.spelling(3) == "\"s\"");
    REQUIRE(stream.identifier(1) == stream.identifier(5));
    REQUIRE(stream.interner() == &fixture.lexer.interner());
    REQUIRE(stream.spelling(stream.size() - 1).empty());
}

TEST_CASE("TokenPool and TokenStream reuse contiguous storage",
          "[lexer][stream][pool]")
{
    TokenPool pool;
    pool.reserve(8);
    REQUIRE(pool.capacity() >= 8);

    const auto first = pool.allocate(Tag::T_PLUS, 10, 11, 1, 7);
    const auto second = pool.allocate(Tag::T_INTLIT, 12, 13, 0, 42);
    REQUIRE(first == 0);
    REQUIRE(second == 1);
    REQUIRE(pool.size() == 2);
    REQUIRE(pool.token(1).int_val == 42);

    pool.deallocate(1);
    REQUIRE(pool.size() == 1);
    pool.clear();
    REQUIRE(pool.empty());

    Fixture fixture("let x = 1;");
    TokenStream &stream = fixture.stream;
    const std::size_t original_size = stream.size();
    const std::size_t original_capacity = stream.capacity();

    REQUIRE(stream.size() == 6);
    REQUIRE(stream.capacity() >= original_size);
    REQUIRE(stream.pool().tags().size() == stream.pool().starts().size());
    REQUIRE(stream.pool().ends().size() == stream.pool().starts().size());

    stream.truncate(2);
    REQUIRE(stream.size() == 2);
    REQUIRE(stream.cursor() == 0);
    stream.clear();
    REQUIRE(stream.empty());
    REQUIRE(stream.capacity() >= original_capacity);

    fixture.lexer.lex(stream);
    REQUIRE(stream.size() == original_size);
    REQUIRE(stream.capacity() >= original_capacity);
    REQUIRE(tags_of(stream) ==
            expected({Tag::T_LET, Tag::T_IDENT, Tag::T_ASSIGN, Tag::T_INTLIT,
                      Tag::T_SEMI, Tag::T_EOF}));
}
