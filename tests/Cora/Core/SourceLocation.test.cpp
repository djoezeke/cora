#include <Cora/Core/SourceLocation.h>

#include <catch2/catch_test_macros.hpp>

using cora::core::INVALID_FILE_ID;
using cora::core::LineCol;
using cora::core::SourceSpan;

TEST_CASE("SourceSpan defaults to an invalid span", "[core][sourcelocation]")
{
    constexpr SourceSpan span{};

    STATIC_REQUIRE(span.file_id == INVALID_FILE_ID);
    STATIC_REQUIRE_FALSE(span.is_valid());
    STATIC_REQUIRE(span.start_offset == 0);
    STATIC_REQUIRE(span.end_offset == 0);
    STATIC_REQUIRE(span.length() == 0);
}

TEST_CASE("SourceSpan reports validity and length", "[core][sourcelocation]")
{
    constexpr SourceSpan span{7, 4, 10};

    STATIC_REQUIRE(span.is_valid());
    STATIC_REQUIRE(span.length() == 6);
}

TEST_CASE("SourceSpan merges overlapping spans", "[core][sourcelocation]")
{
    constexpr SourceSpan first{1, 10, 20};
    constexpr SourceSpan second{1, 5, 12};

    constexpr SourceSpan merged = SourceSpan::merge(first, second);

    STATIC_REQUIRE(merged.file_id == 1);
    STATIC_REQUIRE(merged.start_offset == 5);
    STATIC_REQUIRE(merged.end_offset == 20);
    STATIC_REQUIRE(merged.length() == 15);
}

TEST_CASE("SourceSpan merge keeps the valid span", "[core][sourcelocation]")
{
    constexpr SourceSpan valid{2, 4, 8};
    constexpr SourceSpan invalid{};

    constexpr SourceSpan keep_right = SourceSpan::merge(invalid, valid);
    STATIC_REQUIRE(keep_right.file_id == valid.file_id);
    STATIC_REQUIRE(keep_right.start_offset == valid.start_offset);
    STATIC_REQUIRE(keep_right.end_offset == valid.end_offset);

    constexpr SourceSpan keep_left = SourceSpan::merge(valid, invalid);
    STATIC_REQUIRE(keep_left.file_id == valid.file_id);
    STATIC_REQUIRE(keep_left.start_offset == valid.start_offset);
    STATIC_REQUIRE(keep_left.end_offset == valid.end_offset);
}

TEST_CASE("LineCol stores one-based line and column", "[core][sourcelocation]")
{
    constexpr LineCol location{3, 17};

    STATIC_REQUIRE(location.line == 3);
    STATIC_REQUIRE(location.column == 17);
}
