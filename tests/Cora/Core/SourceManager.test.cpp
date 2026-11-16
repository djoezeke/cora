#include <Cora/Core/SourceManager.h>

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>

using cora::core::INVALID_FILE_ID;
using cora::core::SourceManager;
using cora::core::SourceSpan;

TEST_CASE("SourceManager assigns sequential ids to virtual files", "[core][sourcemanager]")
{
    SourceManager manager;

    REQUIRE(manager.get_file(INVALID_FILE_ID) == nullptr);

    const auto first = manager.add_virtual_file("first.cora", "first");
    const auto second = manager.add_virtual_file("second.cora", "second");

    REQUIRE(first == 1);
    REQUIRE(second == 2);

    const auto *first_file = manager.get_file(first);
    const auto *second_file = manager.get_file(second);

    REQUIRE(first_file != nullptr);
    REQUIRE(second_file != nullptr);
    REQUIRE(first_file->GetID() == first);
    REQUIRE(second_file->GetID() == second);
    REQUIRE(first_file->GetPath() == std::string_view("first.cora"));
    REQUIRE(second_file->GetContent() == std::string_view("second"));
}

TEST_CASE("SourceManager deduplicates virtual files by name", "[core][sourcemanager]")
{
    SourceManager manager;

    const auto first = manager.add_virtual_file("same.cora", "original");
    const auto second = manager.add_virtual_file("same.cora", "replacement");

    REQUIRE(first == second);

    const auto *file = manager.get_file(first);
    REQUIRE(file != nullptr);
    REQUIRE(file->GetContent() == std::string_view("original"));
}

TEST_CASE("SourceManager rejects unknown file ids", "[core][sourcemanager]")
{
    SourceManager manager;

    manager.add_virtual_file("known.cora", "content");

    REQUIRE(manager.get_file(INVALID_FILE_ID) == nullptr);
    REQUIRE(manager.get_file(9999) == nullptr);
}

TEST_CASE("SourceManager resolves spans to locations", "[core][sourcemanager]")
{
    SourceManager manager;
    const auto id = manager.add_virtual_file("virtual.cora", "line one\nline two\n");

    const auto location = manager.resolve_location(SourceSpan{id, 9, 12});

    REQUIRE(location.has_value());
    REQUIRE(location->line == 2);
    REQUIRE(location->column == 1);

    REQUIRE_FALSE(manager.resolve_location(SourceSpan{}).has_value());
    REQUIRE_FALSE(manager.resolve_location(SourceSpan{9999, 0, 1}).has_value());
}

TEST_CASE("SourceManager loads files from disk and caches them", "[core][sourcemanager]")
{
    const auto path = std::filesystem::temp_directory_path() / "cora_source_manager_test.cora";

    std::error_code error;
    std::filesystem::remove(path, error);

    {
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        REQUIRE(output.is_open());
        output << "let answer = 42;\n";
    }

    SourceManager manager;

    const auto first_load = manager.load_file(path.string());
    REQUIRE(first_load.has_value());
    REQUIRE(*first_load != INVALID_FILE_ID);

    const auto *file = manager.get_file(*first_load);
    REQUIRE(file != nullptr);
    REQUIRE(std::string(file->GetPath()) == path.string());
    REQUIRE(file->GetContent() == std::string_view("let answer = 42;\n"));

    const auto second_load = manager.load_file(path.string());
    REQUIRE(second_load.has_value());
    REQUIRE(*second_load == *first_load);

    std::filesystem::remove(path, error);
}

TEST_CASE("SourceManager reports missing files", "[core][sourcemanager]")
{
    const auto path = std::filesystem::temp_directory_path() / "cora_source_manager_missing.cora";

    std::error_code error;
    std::filesystem::remove(path, error);

    SourceManager manager;

    REQUIRE_FALSE(manager.load_file(path.string()).has_value());
}
