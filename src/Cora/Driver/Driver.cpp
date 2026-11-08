#include <Cora/AIR/AIR.h>
#include <Cora/Backend/CBackend.h>
#include <Cora/Cache/CIRCache.h>
#include <Cora/Cache/ObjectCache.h>
#include <Cora/CIR/CIR.h>
#include <Cora/Core/FileHash.h>
#include <Cora/Driver/BuildConfig.h>
#include <Cora/Driver/CompilationSession.h>
#include <Cora/Driver/Driver.h>
#include <Cora/Lexer/Lexer.h>
#include <Cora/Link/Linker.h>
#include <Cora/MIR/MIR.h>
#include <Cora/Parse/Parser.h>

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <system_error>
#include <utility>

namespace cora::driver
{
    namespace
    {
        struct ParsedArguments
        {
            BuildConfig config;
            bool manifestInput = false;
            bool help = false;
            bool version = false;
            bool valid = true;
            std::string error;
            std::string cacheAction;
            std::string cacheKind = "all";
            std::string cacheKey;
            std::string cacheUnit;
            bool dumpTokens = false;
            bool dumpAst = false;
            bool dumpCir = false;
            bool dumpAir = false;
            bool dumpMir = false;
        };

        bool startsWith(std::string_view value, std::string_view prefix)
        {
            return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
        }

        bool parseInteger(std::string_view text, int &value)
        {
            const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
            return result.ec == std::errc{} && result.ptr == text.data() + text.size();
        }

        bool parseSize(std::string_view text, std::size_t &value)
        {
            const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
            return result.ec == std::errc{} && result.ptr == text.data() + text.size();
        }

        std::string trim(std::string value);
        std::string tomlValue(std::string value);

        std::string requireValue(int &index, int argc, const char *const *argv,
                                 std::string_view option, ParsedArguments &parsed)
        {
            if (index + 1 >= argc)
            {
                parsed.valid = false;
                parsed.error = "missing value for " + std::string(option);
                return {};
            }
            return argv[++index];
        }

        bool setEmit(BuildConfig &config, std::string_view value)
        {
            if (value == "obj" || value == "object") config.emit = EmitKind::Object;
            else if (value == "cir") config.emit = EmitKind::CIR;
            else if (value == "c") config.emit = EmitKind::CSource;
            else if (value == "check") config.emit = EmitKind::Check;
            else if (value == "exe" || value == "executable") config.emit = EmitKind::Executable;
            else if (value == "lib" || value == "library") config.emit = EmitKind::Library;
            else return false;
            return true;
        }

        void setFeatures(cora::codegen::TargetSpec &target, std::string_view value)
        {
            std::size_t begin = 0;
            while (begin <= value.size())
            {
                const std::size_t end = value.find(',', begin);
                const std::size_t length = end == std::string_view::npos
                                               ? value.size() - begin
                                               : end - begin;
                if (length != 0)
                {
                    std::string feature(value.substr(begin, length));
                    feature = trim(std::move(feature));
                    if (!feature.empty() && feature.front() == '[')
                        feature.erase(feature.begin());
                    if (!feature.empty() && feature.back() == ']')
                        feature.pop_back();
                    feature = tomlValue(std::move(feature));
                    if (!feature.empty())
                        target.features.push_back(std::move(feature));
                }
                if (end == std::string_view::npos)
                    break;
                begin = end + 1;
            }
        }

        std::string trim(std::string value)
        {
            const auto first = value.find_first_not_of(" \t\r\n");
            if (first == std::string::npos)
                return {};
            const auto last = value.find_last_not_of(" \t\r\n");
            return value.substr(first, last - first + 1);
        }

        std::string tomlValue(std::string value)
        {
            value = trim(std::move(value));
            if (value.size() >= 2 &&
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\'')))
                return value.substr(1, value.size() - 2);
            return value;
        }

        bool applyManifest(BuildConfig &config, ParsedArguments &parsed)
        {
            const auto root = std::filesystem::current_path();
            const auto manifest = root / "cora.toml";
            std::ifstream input(manifest);
            if (!input)
                return true;

            config.projectRoot = root;
            config.manifestPath = manifest;
            std::string section;
            std::string line;
            while (std::getline(input, line))
            {
                const auto comment = line.find('#');
                if (comment != std::string::npos)
                    line.erase(comment);
                line = trim(std::move(line));
                if (line.empty())
                    continue;
                if (line.front() == '[' && line.back() == ']')
                {
                    section = trim(line.substr(1, line.size() - 2));
                    continue;
                }
                const auto equals = line.find('=');
                if (equals == std::string::npos)
                    continue;
                const std::string key = section.empty()
                                            ? trim(line.substr(0, equals))
                                            : section + "." + trim(line.substr(0, equals));
                const std::string value = tomlValue(line.substr(equals + 1));
                if (key == "package.source" || key == "build.source" || key == "source" ||
                    key == "build.input")
                {
                    config.input = config.projectRoot / value;
                    parsed.manifestInput = true;
                }
                else if (key == "build.output") config.output = config.projectRoot / value;
                else if (key == "build.cache-dir") config.cacheRoot = config.projectRoot / value;
                else if (key == "build.profile" || key == "profile") config.profile = value;
                else if (key == "build.target" || key == "target" ||
                         key == "target.triple") config.target.triple = value;
                else if (key == "build.emit" || key == "emit") setEmit(config, value);
                else if (key == "build.cpu" || key == "target.cpu") config.target.cpu = value;
                else if (key == "build.features" || key == "target.features")
                    setFeatures(config.target, value.substr(
                        value.find('[') == std::string::npos ? 0 : 1));
                else if (key == "build.opt-level" || key == "profile.opt-level")
                    parseInteger(value, config.optimizationLevel);
                else if (key == "build.jobs") parseSize(value, config.jobs);
                else if (key == "build.frontend-options") config.frontendOptions = value;
                else if (key == "build.debug") config.debugFlags = value;
                else if (key == "build.sanitize") config.sanitizerFlags = value;
                else if (key == "build.lto") config.ltoFlags = value;
                else if (key == "build.no-cache" && value == "true")
                    config.cachePolicy = cora::cache::CachePolicy::Disabled;
            }
            if (config.input.empty())
            {
                const auto defaultSource = config.projectRoot / "src" / "main.cora";
                if (std::filesystem::exists(defaultSource))
                {
                    config.input = defaultSource;
                    parsed.manifestInput = true;
                }
            }
            return true;
        }

        ParsedArguments parseArguments(int argc, const char *const *argv)
        {
            ParsedArguments parsed;
            applyManifest(parsed.config, parsed);
            int index = 1;
            if (index < argc && argv[index][0] != '-')
            {
                const std::string command = argv[index];
                if (command == "build" || command == "check" || command == "emit" ||
                    command == "cache")
                {
                    parsed.config.command = command;
                    ++index;
                    if (command == "check")
                        parsed.config.emit = EmitKind::Check;
                    if (command == "cache")
                    {
                        if (index < argc && argv[index][0] != '-')
                            parsed.cacheAction = argv[index++];
                        if (parsed.cacheAction.empty())
                        {
                            parsed.valid = false;
                            parsed.error = "cache requires list, show, or clean";
                            return parsed;
                        }
                    }
                }
            }

            for (; index < argc && parsed.valid; ++index)
            {
                const std::string argument = argv[index];
                if (argument == "--help" || argument == "-h") { parsed.help = true; continue; }
                if (argument == "--version") { parsed.version = true; continue; }

                auto optionValue = [&](std::string_view option) {
                    const std::string prefix = std::string(option) + "=";
                    if (startsWith(argument, prefix))
                        return argument.substr(prefix.size());
                    return requireValue(index, argc, argv, option, parsed);
                };

                if (argument == "--no-cache") parsed.config.cachePolicy = cora::cache::CachePolicy::Disabled;
                else if (argument == "--cache-read-only") parsed.config.cachePolicy = cora::cache::CachePolicy::ReadOnly;
                else if (argument == "--explain-cache") parsed.config.explainCache = true;
                else if (argument == "--verbose" || argument == "-v") parsed.config.verbose = true;
                else if (argument == "--dump-tokens") parsed.dumpTokens = true;
                else if (argument == "--dump-ast") parsed.dumpAst = true;
                else if (argument == "--dump-cir") parsed.dumpCir = true;
                else if (argument == "--dump-air") parsed.dumpAir = true;
                else if (argument == "--dump-mir") parsed.dumpMir = true;
                else if (argument == "--pic") parsed.config.relocationFlags = "pic";
                else if (argument == "--pie") parsed.config.relocationFlags = "pie";
                else if (argument == "--cache-dir" || startsWith(argument, "--cache-dir=")) parsed.config.cacheRoot = optionValue("--cache-dir");
                else if (argument == "--target" || startsWith(argument, "--target=")) parsed.config.target.triple = optionValue("--target");
                else if (argument == "--profile" || startsWith(argument, "--profile=")) parsed.config.profile = optionValue("--profile");
                else if (argument == "--emit" || startsWith(argument, "--emit="))
                {
                    const std::string value = optionValue("--emit");
                    if (!parsed.valid || !setEmit(parsed.config, value))
                    {
                        parsed.valid = false;
                        parsed.error = "unknown emit kind: " + value;
                    }
                }
                else if (argument == "--opt-level" || startsWith(argument, "--opt-level=") ||
                         argument == "-O")
                {
                    const std::string value = optionValue(argument == "-O" ? "-O" : "--opt-level");
                    if (!parseInteger(value, parsed.config.optimizationLevel) ||
                        parsed.config.optimizationLevel < 0 || parsed.config.optimizationLevel > 3)
                    {
                        parsed.valid = false;
                        parsed.error = "optimization level must be between 0 and 3";
                    }
                }
                else if (argument.size() == 3 && argument[0] == '-' && argument[1] == 'O' &&
                         argument[2] >= '0' && argument[2] <= '3')
                    parsed.config.optimizationLevel = argument[2] - '0';
                else if (argument == "--cpu" || startsWith(argument, "--cpu=")) parsed.config.target.cpu = optionValue("--cpu");
                else if (argument == "--features" || startsWith(argument, "--features=")) setFeatures(parsed.config.target, optionValue("--features"));
                else if (argument == "--debug" || startsWith(argument, "--debug=")) parsed.config.debugFlags = optionValue("--debug");
                else if (argument == "--sanitize" || startsWith(argument, "--sanitize=")) parsed.config.sanitizerFlags = optionValue("--sanitize");
                else if (argument == "--lto" || startsWith(argument, "--lto=")) parsed.config.ltoFlags = optionValue("--lto");
                else if (argument == "--output" || argument == "-o" || startsWith(argument, "--output=")) parsed.config.output = optionValue(argument == "-o" ? "-o" : "--output");
                else if (argument == "--jobs" || argument == "-j" || startsWith(argument, "--jobs="))
                {
                    const std::string value = optionValue(argument == "-j" ? "-j" : "--jobs");
                    if (!parseSize(value, parsed.config.jobs) || parsed.config.jobs == 0)
                    {
                        parsed.valid = false;
                        parsed.error = "jobs must be a positive integer";
                    }
                }
                else if (argument == "--link-arg" || startsWith(argument, "--link-arg=")) parsed.config.linkArguments.push_back(optionValue("--link-arg"));
                else if (argument == "--kind" || startsWith(argument, "--kind=")) parsed.cacheKind = optionValue("--kind");
                else if (argument == "--key" || startsWith(argument, "--key=")) parsed.cacheKey = optionValue("--key");
                else if (argument == "--unit" || startsWith(argument, "--unit=")) parsed.cacheUnit = optionValue("--unit");
                else if (argument == "--source" || startsWith(argument, "--source=")) parsed.config.input = optionValue("--source");
                else if (!argument.empty() && argument[0] != '-')
                {
                    if (!parsed.config.input.empty() && !parsed.manifestInput)
                    {
                        parsed.valid = false;
                        parsed.error = "only one source input is supported";
                    }
                    else
                    {
                        parsed.config.input = argument;
                        parsed.manifestInput = false;
                    }
                }
                else
                {
                    parsed.valid = false;
                    parsed.error = "unknown option: " + argument;
                }
            }

            if (parsed.config.command == "cache" && parsed.cacheAction != "list" &&
                parsed.cacheAction != "show" && parsed.cacheAction != "clean")
            {
                parsed.valid = false;
                parsed.error = "unknown cache command: " + parsed.cacheAction;
            }
            return parsed;
        }

        std::optional<std::string> readText(const std::filesystem::path &path, std::string &error)
        {
            std::ifstream input(path, std::ios::binary | std::ios::ate);
            if (!input)
            {
                error = "unable to open source file " + path.string();
                return std::nullopt;
            }
            const std::streampos end = input.tellg();
            if (end < 0)
            {
                error = "unable to determine source file size " + path.string();
                return std::nullopt;
            }
            std::string result(static_cast<std::size_t>(end), '\0');
            input.seekg(0);
            if (!result.empty())
                input.read(result.data(), static_cast<std::streamsize>(result.size()));
            if (!input && !input.eof())
            {
                error = "unable to read source file " + path.string();
                return std::nullopt;
            }
            return result;
        }

        bool writeBytes(const std::filesystem::path &path,
                        const std::vector<std::uint8_t> &bytes, std::string &error)
        {
            std::error_code filesystemError;
            if (!path.parent_path().empty())
                std::filesystem::create_directories(path.parent_path(), filesystemError);
            if (filesystemError)
            {
                error = "unable to create output directory: " + filesystemError.message();
                return false;
            }
            std::ofstream output(path, std::ios::binary | std::ios::trunc);
            if (!output)
            {
                error = "unable to create output file " + path.string();
                return false;
            }
            if (!bytes.empty())
                output.write(reinterpret_cast<const char *>(bytes.data()),
                             static_cast<std::streamsize>(bytes.size()));
            if (!output)
            {
                error = "unable to write output file " + path.string();
                return false;
            }
            return true;
        }

        std::string sourceIdentity(const std::filesystem::path &input)
        {
            std::error_code error;
            const auto absolute = std::filesystem::absolute(input, error).lexically_normal();
            const auto current = std::filesystem::current_path(error);
            const auto relative = std::filesystem::relative(absolute, current, error);
            return (error ? absolute : relative).generic_string();
        }

        std::string moduleIdentity(const std::filesystem::path &input)
        {
            std::string result = input.stem().generic_string();
            std::replace(result.begin(), result.end(), '-', '_');
            return result.empty() ? "main" : result;
        }

        std::filesystem::path defaultOutput(const BuildConfig &config,
                                            const std::filesystem::path &input,
                                            std::string_view extension)
        {
            return std::filesystem::path("build") / config.target.triple / config.profile /
                   (input.stem().string() + std::string(extension));
        }

        void printCacheStatus(std::ostream &out, const BuildConfig &config,
                              std::string_view kind, const cora::core::Digest &key,
                              const std::filesystem::path &path, std::string_view reason)
        {
            out << "cache: " << kind << ' ' << reason << "\n"
                << "  key:  " << key.hex() << "\n"
                << "  path: " << path.string() << "\n";
            if (config.verbose)
                out << "  policy: " << cachePolicyName(config.cachePolicy) << "\n";
        }

        bool parseSource(const std::string &source, cora::ParsedUnit &unit,
                         std::ostream &err, const std::filesystem::path &path)
        {
            cora::Lexer lexer(source);
            unit = cora::Parser::parse(lexer.lexAll());
            bool valid = true;
            for (const auto &message : unit.tokens.errors)
            {
                err << path.string() << ": error: LEX-001: " << message << '\n';
                valid = false;
            }
            for (const auto &diagnostic : unit.diagnostics)
            {
                err << path.string() << ':' << diagnostic.range.begin.line << ':'
                    << diagnostic.range.begin.column << ": error: PARSE-001: "
                    << diagnostic.message << '\n';
                valid = false;
            }
            return valid && unit.success();
        }

        void dumpTokens(std::ostream &out, const cora::TokenBuffer &tokens)
        {
            out << "Tokens\n";
            for (const auto &token : tokens.tokens)
            {
                out << token.range.begin.line << ':' << token.range.begin.column << ' '
                    << tokenKindName(token.type) << " text=" << std::quoted(token.text) << '\n';
            }
        }

        void dumpCIR(std::ostream &out, const cora::cir::CIRArtifact &artifact)
        {
            out << "CIR key=" << artifact.key.hex() << " source=" << artifact.input.sourceFileId
                << " tokens=" << artifact.module.tokenCount << " lines=" << artifact.module.lineCount
                << " functions=" << artifact.module.functionCount << " interface="
                << artifact.publicInterfaceDigest.hex()
                << " instructions=" << artifact.module.instructions.size() << '\n';
            for (const auto &instruction : artifact.module.instructions)
            {
                out << "  " << instruction.opcode;
                for (const auto &operand : instruction.operands)
                    out << ' ' << operand;
                out << '\n';
            }
        }

        void dumpAIR(std::ostream &out, const cora::air::AIRModule &module)
        {
            out << "AIR verified=" << (module.verified ? "true" : "false")
                << " functions=" << module.functions.size()
                << " instructions=" << module.instructions.size() << '\n';
            for (const auto &instruction : module.instructions)
                out << "  " << instruction.opcode << ' ' << instruction.operand << '\n';
        }

        void dumpMIR(std::ostream &out, const cora::mir::MIRModule &module)
        {
            out << "MIR allocated=" << (module.allocated ? "true" : "false")
                << " target=" << module.target.triple
                << " instructions=" << module.instructions.size() << '\n';
            for (const auto &instruction : module.instructions)
            {
                out << "  " << instruction.opcode;
                for (const auto &operand : instruction.operands)
                    out << ' ' << operand;
                out << '\n';
            }
        }

        int runCache(const ParsedArguments &parsed, std::ostream &out, std::ostream &err)
        {
            const BuildConfig &config = parsed.config;
            cora::cache::ContentAddressedStorage storage(config.cacheRoot);
            const bool all = parsed.cacheKind == "all";
            const bool cir = all || parsed.cacheKind == "cir";
            const bool object = all || parsed.cacheKind == "object";
            if (!cir && !object)
            {
                err << "error: --kind must be cir, object, or all\n";
                return 2;
            }
            if (parsed.cacheAction == "list")
            {
                if (cir)
                    for (const auto &path : storage.list(cora::cache::CacheNamespace::CIR))
                        out << "cir " << path.string() << '\n';
                if (object)
                    for (const auto &path : storage.list(cora::cache::CacheNamespace::Object))
                        out << "object " << path.string() << '\n';
                return 0;
            }
            if (parsed.cacheAction == "clean")
            {
                std::string error;
                if (cir && !storage.clean(cora::cache::CacheNamespace::CIR, &error))
                {
                    err << "error: " << error << '\n';
                    return 1;
                }
                if (object && !storage.clean(cora::cache::CacheNamespace::Object, &error))
                {
                    err << "error: " << error << '\n';
                    return 1;
                }
                out << "cache: cleaned " << parsed.cacheKind << " entries\n";
                return 0;
            }
            if (parsed.cacheAction == "show" && parsed.cacheKind == "cir")
            {
                if (config.input.empty())
                {
                    err << "error: CIR cache show requires --source <file>\n";
                    return 2;
                }
                std::string error;
                const auto source = readText(config.input, error);
                if (!source)
                {
                    err << "error: " << error << '\n';
                    return 1;
                }
                cora::cir::CIRInput input;
                input.sourceFileId = sourceIdentity(config.input);
                input.moduleId = moduleIdentity(config.input);
                input.sourceDigest = cora::core::hashBytes(*source);
                input.frontendOptions = config.frontendOptions;
                cora::cache::CIRCache cache(config.cacheRoot);
                const auto lookup = cache.lookup(input);
                printCacheStatus(out, config, "CIR", lookup.key, lookup.path,
                                 lookup.hit ? "cache hit" : lookup.reason);
                return 0;
            }
            if (parsed.cacheAction == "show" && parsed.cacheKind == "object")
            {
                const auto key = cora::core::Digest::fromHex(parsed.cacheKey);
                if (!key || parsed.cacheUnit.empty())
                {
                    err << "error: object cache show requires --key <sha256> and --unit <name>\n";
                    return 2;
                }
                const auto lookup = storage.lookup(cora::cache::CacheNamespace::Object, *key,
                                                   parsed.cacheUnit, ".o");
                printCacheStatus(out, config, "object", *key, lookup.path,
                                 lookup.hit ? "cache hit" : lookup.reason);
                return 0;
            }
            err << "error: cache show requires --kind cir or --kind object\n";
            return 2;
        }

        int runBuild(const ParsedArguments &parsed, std::ostream &out, std::ostream &err)
        {
            const BuildConfig &config = parsed.config;
            if (config.input.empty())
            {
                err << "error: a source file is required\n";
                return 2;
            }
            std::string error;
            const auto source = readText(config.input, error);
            if (!source)
            {
                err << "error: " << error << '\n';
                return 1;
            }

            CompilationSession session(config);
            cora::cir::CIRInput cirInput;
            cirInput.sourceFileId = sourceIdentity(config.input);
            cirInput.moduleId = moduleIdentity(config.input);
            cirInput.sourceDigest = cora::core::hashBytes(*source);
            cirInput.frontendOptions = config.frontendOptions;

            const auto cirLookup = session.cirCache().lookup(cirInput);
            cora::cir::CIRArtifact cirArtifact;
            std::optional<cora::ParsedUnit> parsedSource;
            if (cirLookup.hit && cirLookup.artifact)
            {
                cirArtifact = *cirLookup.artifact;
                if (config.explainCache)
                    printCacheStatus(out, config, "CIR", cirLookup.key, cirLookup.path, "cache hit");
            }
            else
            {
                parsedSource.emplace();
                if (!parseSource(*source, *parsedSource, err, config.input))
                    return 1;
                cirArtifact = cora::cir::build(cirInput, *parsedSource);
                if (config.explainCache)
                    printCacheStatus(out, config, "CIR", cirLookup.key, cirLookup.path,
                                     cirLookup.reason.empty() ? "cache miss" : cirLookup.reason);
                if (config.cachePolicy == cora::cache::CachePolicy::Enabled)
                {
                    const auto write = session.cirCache().publish(cirArtifact);
                    if (!write.published)
                    {
                        err << "error: CACHE-001: " << write.error << '\n';
                        return 1;
                    }
                }
            }

            if (cirArtifact.module.ast == nullptr)
            {
                err << "error: CACHE-002: CIR artifact has no AST payload\n";
                return 1;
            }

            if (parsed.dumpTokens || parsed.dumpAst)
            {
                if (!parsedSource)
                {
                    parsedSource.emplace();
                    if (!parseSource(*source, *parsedSource, err, config.input))
                        return 1;
                }
                if (parsed.dumpTokens)
                    dumpTokens(out, parsedSource->tokens);
                if (parsed.dumpAst)
                    parsedSource->ast->dump(out);
            }
            if (parsed.dumpCir)
                dumpCIR(out, cirArtifact);

            if (config.emit == EmitKind::CIR)
            {
                const auto output = config.output.empty() ? defaultOutput(config, config.input, ".cir")
                                                            : config.output;
                const auto bytes = cirArtifact.serialize();
                if (!writeBytes(output, bytes, error))
                {
                    err << "error: " << error << '\n';
                    return 1;
                }
                out << "emit: CIR " << output.string() << '\n';
                return 0;
            }

            const bool mustAnalyze = config.emit == EmitKind::Check || parsed.dumpAir ||
                                     parsed.dumpMir || config.emit == EmitKind::CSource;
            cora::air::AIRModule air;
            cora::mir::MIRModule mir;
            bool haveMir = false;
            if (mustAnalyze)
            {
                air = cora::air::SemanticAnalyzer::analyze(cirArtifact);
                if (!air.verified)
                {
                    for (const auto &message : air.errors)
                        err << "error: SEMA-001: " << message << '\n';
                    return 1;
                }
                air = cora::air::AIRPassManager::optimize(std::move(air), config.optimizationLevel);
                if (parsed.dumpAir)
                    dumpAIR(out, air);
                if (parsed.dumpMir || config.emit == EmitKind::CSource)
                {
                    cora::codegen::TargetSpec target = config.target;
                    mir = cora::mir::TargetLowerer::lower(air, target);
                    mir = cora::mir::MachinePassManager::run(std::move(mir),
                                                             "O" + std::to_string(config.optimizationLevel));
                    haveMir = true;
                    if (parsed.dumpMir)
                        dumpMIR(out, mir);
                }
            }

            if (config.emit == EmitKind::Check)
            {
                out << "check: " << config.input.string() << " (AIR instructions: "
                    << air.instructions.size() << ")\n";
                return 0;
            }

            cora::codegen::ObjectInput objectInput;
            objectInput.codegenUnitId = cirInput.moduleId;
            objectInput.cirInputKeys.push_back(cirArtifact.key);
            objectInput.interfaceDigests.push_back(cirArtifact.publicInterfaceDigest);
            objectInput.target = config.target;
            objectInput.optimizationFlags = "O" + std::to_string(config.optimizationLevel);
            objectInput.debugFlags = config.debugFlags;
            objectInput.sanitizerFlags = config.sanitizerFlags;
            objectInput.relocationFlags = config.relocationFlags;
            objectInput.unwindFlags = config.unwindFlags;
            objectInput.ltoFlags = config.ltoFlags;

            if (config.emit == EmitKind::CSource)
            {
                if (!haveMir)
                {
                    air = cora::air::SemanticAnalyzer::analyze(cirArtifact);
                    if (!air.verified)
                    {
                        for (const auto &message : air.errors)
                            err << "error: SEMA-001: " << message << '\n';
                        return 1;
                    }
                    mir = cora::mir::MachinePassManager::run(
                        cora::mir::TargetLowerer::lower(
                            cora::air::AIRPassManager::optimize(std::move(air), config.optimizationLevel),
                            config.target),
                        objectInput.optimizationFlags);
                }
                const std::string cSource = cora::backend::CBackend::emitC(mir);
                const std::vector<std::uint8_t> bytes(cSource.begin(), cSource.end());
                const auto output = config.output.empty() ? defaultOutput(config, config.input, ".c")
                                                            : config.output;
                if (!writeBytes(output, bytes, error))
                {
                    err << "error: " << error << '\n';
                    return 1;
                }
                out << "emit: C " << output.string() << '\n';
                return 0;
            }

            const auto objectLookup = session.objectCache().lookup(objectInput);
            const bool needCodegen = !objectLookup.hit || parsed.dumpAir || parsed.dumpMir;
            std::vector<std::uint8_t> objectBytes;
            if (objectLookup.hit && !needCodegen)
            {
                if (config.explainCache)
                    printCacheStatus(out, config, "object", objectInput.key(), objectLookup.path, "cache hit");
                const auto cached = session.objectCache().storage().read(objectLookup.path, &error);
                if (!cached)
                {
                    err << "error: CACHE-003: " << error << '\n';
                    return 1;
                }
                objectBytes = *cached;
            }
            else
            {
                if (config.explainCache)
                    printCacheStatus(out, config, "object", objectInput.key(), objectLookup.path,
                                     objectLookup.reason.empty() ? "cache miss" : objectLookup.reason);
                if (!haveMir)
                {
                    air = cora::air::SemanticAnalyzer::analyze(cirArtifact);
                    if (!air.verified)
                    {
                        for (const auto &message : air.errors)
                            err << "error: SEMA-001: " << message << '\n';
                        return 1;
                    }
                    air = cora::air::AIRPassManager::optimize(std::move(air), config.optimizationLevel);
                    if (parsed.dumpAir)
                        dumpAIR(out, air);
                    mir = cora::mir::MachinePassManager::run(
                        cora::mir::TargetLowerer::lower(air, config.target),
                        objectInput.optimizationFlags);
                    haveMir = true;
                    if (parsed.dumpMir)
                        dumpMIR(out, mir);
                }
                const auto object = cora::backend::CBackend::emitObject(mir, config.target);
                objectBytes = object.bytes;
                if (config.cachePolicy == cora::cache::CachePolicy::Enabled)
                {
                    const auto write = session.objectCache().publish(objectInput, objectBytes);
                    if (!write.published)
                    {
                        err << "error: CACHE-004: " << write.error << '\n';
                        return 1;
                    }
                }
            }

            if (config.emit == EmitKind::Executable || config.emit == EmitKind::Library)
            {
                cora::backend::ObjectBuffer object;
                object.bytes = objectBytes;
                object.format = config.target.objectFormat;
                const auto linked = cora::link::Linker::link({object}, config.target.triple,
                                                              config.linkArguments);
                const auto output = config.output.empty()
                                        ? defaultOutput(config, config.input,
                                                        config.emit == EmitKind::Executable ? ".exe" : ".lib")
                                        : config.output;
                if (!writeBytes(output, linked.bytes, error))
                {
                    err << "error: " << error << '\n';
                    return 1;
                }
                out << "link: " << output.string() << '\n';
                return 0;
            }

            const auto output = config.output.empty() ? defaultOutput(config, config.input, ".o")
                                                        : config.output;
            if (!writeBytes(output, objectBytes, error))
            {
                err << "error: " << error << '\n';
                return 1;
            }
            out << "emit: object " << output.string() << '\n';
            return 0;
        }
    } // namespace

    int Driver::run(int argc, const char *const *argv,
                    std::ostream &out, std::ostream &err) const
    {
        const ParsedArguments parsed = parseArguments(argc, argv);
        if (parsed.help || argc == 1)
        {
            out << usage();
            return parsed.valid ? 0 : 2;
        }
        if (parsed.version)
        {
            out << "cora skeleton 0.2\n";
            return 0;
        }
        if (!parsed.valid)
        {
            err << "error: " << parsed.error << "\n\n" << usage();
            return 2;
        }
        if (parsed.config.command == "cache")
            return runCache(parsed, out, err);
        return runBuild(parsed, out, err);
    }

} // namespace cora::driver
