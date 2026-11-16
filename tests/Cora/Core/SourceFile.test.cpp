#include <Cora/Core/SourceFile.h>

#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <type_traits>

using cora::core::LineCol;
using cora::core::SourceFile;

namespace
{

void check_location(const LineCol &location, uint32_t line, uint32_t column)
{
    REQUIRE(location.line == line);
    REQUIRE(location.column == column);
}

} // namespace

TEST_CASE("SourceFile stores its identity and content", "[core][sourcefile]")
{
    const SourceFile file(7, "src/main.cora", "let x = 1;\n");

    REQUIRE(file.GetID() == 7);
    REQUIRE(file.GetPath() == std::string_view("src/main.cora"));
    REQUIRE(file.GetContent() == std::string_view("let x = 1;\n"));
}

TEST_CASE("SourceFile is non-copyable", "[core][sourcefile]")
{
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<SourceFile>);
    STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<SourceFile>);
}

TEST_CASE("SourceFile maps byte offsets to one-based positions", "[core][sourcefile]")
{
    const SourceFile file(1, "test.cora", "hello\nworld\n");

    check_location(file.GetLineCol(0), 1, 1);
    check_location(file.GetLineCol(4), 1, 5);
    check_location(file.GetLineCol(5), 1, 6);
    check_location(file.GetLineCol(6), 2, 1);
    check_location(file.GetLineCol(10), 2, 5);
    check_location(file.GetLineCol(11), 2, 6);
}

TEST_CASE("SourceFile handles an empty buffer", "[core][sourcefile]")
{
    const SourceFile file(2, "empty.cora", "");

    REQUIRE(file.GetContent().empty());
    check_location(file.GetLineCol(0), 1, 1);
    REQUIRE(file.GetLineText(1).empty());
}

TEST_CASE("SourceFile returns lines without line endings", "[core][sourcefile]")
{
    const SourceFile file(3, "lines.cora", "alpha\n\nbeta\n");

    REQUIRE(file.GetLineText(1) == std::string_view("alpha"));
    REQUIRE(file.GetLineText(2).empty());
    REQUIRE(file.GetLineText(3) == std::string_view("beta"));
    REQUIRE(file.GetLineText(0).empty());
    REQUIRE(file.GetLineText(4).empty());
}

TEST_CASE("SourceFile strips CRLF line endings", "[core][sourcefile]")
{
    const SourceFile file(4, "crlf.cora", "alpha\r\nbeta\r\n");

    REQUIRE(file.GetLineText(1) == std::string_view("alpha"));
    REQUIRE(file.GetLineText(2) == std::string_view("beta"));
    check_location(file.GetLineCol(7), 2, 1);
}

TEST_CASE("SourceFile returns the final line without a trailing newline", "[core][sourcefile]")
{
    const SourceFile file(5, "plain.cora", "one\ntwo");

    REQUIRE(file.GetLineText(1) == std::string_view("one"));
    REQUIRE(file.GetLineText(2) == std::string_view("two"));
    check_location(file.GetLineCol(4), 2, 1);
}
