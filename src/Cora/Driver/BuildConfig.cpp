#include <Cora/Driver/BuildConfig.h>

#include <sstream>

namespace cora::driver
{

    const char *emitName(EmitKind emit) noexcept
    {
        switch (emit)
        {
        case EmitKind::Object: return "obj";
        case EmitKind::CIR: return "cir";
        case EmitKind::CSource: return "c";
        case EmitKind::Check: return "check";
        case EmitKind::Executable: return "exe";
        case EmitKind::Library: return "lib";
        }
        return "unknown";
    }

    const char *cachePolicyName(cora::cache::CachePolicy policy) noexcept
    {
        switch (policy)
        {
        case cora::cache::CachePolicy::Enabled: return "enabled";
        case cora::cache::CachePolicy::ReadOnly: return "read-only";
        case cora::cache::CachePolicy::Disabled: return "disabled";
        }
        return "unknown";
    }

    std::string usage()
    {
        std::ostringstream text;
        text << "Cora compiler skeleton\n\n"
             << "Usage:\n"
             << "  cora build [options] <source.cora>\n"
             << "  cora check [options] <source.cora>\n"
             << "  cora emit --emit cir|c|obj [options] <source.cora>\n"
             << "  cora cache list|show|clean [options]\n\n"
             << "Pipeline options:\n"
             << "  --emit <cir|c|obj|exe|lib>  requested output (default: obj)\n"
             << "  --target <triple>           target triple (default: native)\n"
             << "  --profile <name>            build profile (default: debug)\n"
             << "  -O0..-O3, --opt-level <n>  object optimization input\n"
             << "  --cpu <name>                target CPU\n"
             << "  --features <a,b,...>        target features\n"
             << "  --debug <none|line|full>    debug information mode\n"
             << "  --sanitize <list>           sanitizer flags\n"
             << "  --pic, --pie                relocation flags\n"
             << "  --lto <off|thin|full>       LTO flags\n"
             << "  -o, --output <path>         output path\n"
             << "  -j, --jobs <n>              worker count (reserved)\n\n"
             << "Cache options:\n"
             << "  --cache-dir <path>          cache root (default: .cora-cache)\n"
             << "  --no-cache                  disable cache reads and writes\n"
             << "  --cache-read-only           permit hits, do not publish misses\n"
             << "  --explain-cache             print keys and hit/miss reasons\n"
             << "  --dump-tokens               print lexer tokens\n"
             << "  --dump-ast                  print the parsed AST\n"
             << "  --dump-cir                  print the CIR artifact summary\n"
             << "  --dump-air                  print AIR on the semantic path\n"
             << "  --dump-mir                  print MIR on the codegen path\n\n"
             << "Cache commands:\n"
             << "  cora cache list [--kind cir|object]\n"
             << "  cora cache show --kind cir --source <file>\n"
             << "  cora cache show --kind object --key <sha256> --unit <name>\n"
             << "  cora cache clean [--kind cir|object|all]\n";
        return text.str();
    }

} // namespace cora::driver
