#ifndef CORA_DRIVER_BUILDCONFIG_H
#define CORA_DRIVER_BUILDCONFIG_H

#include <Cora/Cache/ContentAddressedStorage.h>
#include <Cora/CodeGen/TargetSpec.h>

#include <filesystem>
#include <string>
#include <vector>

namespace cora::driver
{
    enum class EmitKind
    {
        Object,
        CIR,
        CSource,
        Check,
        Executable,
        Library
    };

    struct BuildConfig
    {
        std::string command = "build";
        std::filesystem::path input;
        std::filesystem::path output;
        std::filesystem::path cacheRoot = ".cora-cache";
        std::string profile = "debug";
        cora::codegen::TargetSpec target;
        EmitKind emit = EmitKind::Object;
        cora::cache::CachePolicy cachePolicy = cora::cache::CachePolicy::Enabled;
        int optimizationLevel = 0;
        std::string debugFlags = "none";
        std::string sanitizerFlags;
        std::string relocationFlags;
        std::string unwindFlags;
        std::string ltoFlags;
        std::string frontendOptions = "frontend-skeleton-v1";
        std::vector<std::string> linkArguments;
        std::size_t jobs = 1;
        bool explainCache = false;
        bool verbose = false;
    };
    [[nodiscard]] const char *emitName(EmitKind emit) noexcept;
    [[nodiscard]] const char *cachePolicyName(cora::cache::CachePolicy policy) noexcept;
    [[nodiscard]] std::string usage();
} // namespace cora::driver

#endif // CORA_DRIVER_BUILDCONFIG_H
