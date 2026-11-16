#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Parser/Parser.h>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    using cora::core::FileID;
    using cora::core::SourceManager;
    using cora::diag::DiagnosticEngine;
    using cora::parser::Config;
    using cora::parser::Parser;

    std::string read_source_file(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            return {};
        }
        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    struct Fixture
    {
        SourceManager sm;
        DiagnosticEngine de;
        Parser parser;

        Fixture() : parser(sm, de, Config{}) {}

        const cora::ast::AST &parse(std::string source,
                                    std::string name = "test.cora")
        {
            const FileID id =
                sm.add_virtual_file(std::move(name), std::move(source));
            return parser.parse(id);
        }
    };
} // namespace

TEST_CASE("Parsing syntax.cora produces no errors", "[parser][syntax]")
{
    const std::string path = std::string(CORA_SOURCE_DIR) + "/syntax.cora";
    const std::string source = read_source_file(path);
    INFO("source path: " << path);
    REQUIRE_FALSE(source.empty());

    Fixture fixture;
    const cora::ast::AST &ast = fixture.parse(source, "syntax.cora");

    std::ostringstream failures;
    for (const auto &error : ast.errors)
    {
        failures << "[" << error.span.start_offset << ','
                 << error.span.end_offset << "] "
                 << cora::diag::diag_info(error.id).code << ": "
                 << error.message << '\n';
    }
    INFO("parser errors:\n" << failures.str());
    INFO("nodes: " << ast.nodes.size() << " decls: " << ast.decls.size());
    REQUIRE(ast.errors.empty());
}
