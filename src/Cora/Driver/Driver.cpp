#include <Cora/Driver/Driver.h>
#include <Cora/Frontend/Frontend.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <filesystem>

namespace cora::driver
{
    int Driver::run(int argc, const char *const *argv,
                    std::ostream &out, std::ostream &err) const
    {
        if (argc < 2)
        {
            err << "usage: cora [--emit c|check|stats] [-o output] <source.cora>\n";
            return 2;
        }
        std::string input_path;
        std::string output_path;
        std::string emit = "stats";
        for (int i = 1; i < argc; ++i)
        {
            const std::string argument(argv[i]);
            if (argument == "--help" || argument == "-h")
            {
                out << "usage: cora [--emit c|check|stats] [-o output] <source.cora>\n";
                return 0;
            }
            if (argument == "--emit" && i + 1 < argc)
                emit = argv[++i];
            else if ((argument == "-o" || argument == "--output") && i + 1 < argc)
                output_path = argv[++i];
            else if (!argument.empty() && argument[0] != '-')
                input_path = argument;
            else
            {
                err << "unknown option: " << argument << '\n';
                return 2;
            }
        }
        if (input_path.empty())
        {
            err << "missing source file\n";
            return 2;
        }
        std::ifstream input(input_path, std::ios::binary);
        if (!input)
        {
            err << "cannot open source file: " << input_path << '\n';
            return 2;
        }
        const std::string source((std::istreambuf_iterator<char>(input)), {});
        frontend::Compiler compiler;
        const frontend::FrontendResult result = compiler.compile(source, input_path);
        if (emit != "c" && emit != "check" && emit != "stats")
        {
            err << "unknown emit kind: " << emit << '\n';
            return 2;
        }
        if (emit == "c")
        {
            if (output_path.empty())
            {
                std::filesystem::path generated_path(input_path);
                generated_path.replace_extension(".c");
                std::ofstream output(generated_path, std::ios::binary);
                if (!output)
                {
                    err << "cannot open output file: " << generated_path.string() << '\n';
                    return 2;
                }
                output << result.c_source;
                out << generated_path.string() << '\n';
            }
            else
            {
                std::ofstream output(output_path, std::ios::binary);
                if (!output)
                {
                    err << "cannot open output file: " << output_path << '\n';
                    return 2;
                }
                output << result.c_source;
            }
        }
        else if (emit == "stats")
            out << "tokens=" << result.ast.tokens.size()
                << " ast_nodes=" << result.ast.nodeCount()
                << " cir_bodies=" << result.cir.bodies.size()
                << " air_instructions=" << result.air.instructions.size() << '\n';
        for (const diag::Diagnostic &diagnostic : result.diagnostics)
            err << diagnostic.code() << ": " << diagnostic.message() << '\n';
        return result.status == frontend::ParseStatus::Success ? 0 : 1;
    }
} // namespace cora::driver
