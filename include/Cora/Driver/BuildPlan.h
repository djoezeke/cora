#ifndef CORA_DRIVER_BUILDPLAN_H
#define CORA_DRIVER_BUILDPLAN_H

#include <Cora/Core/Digest.h>

#include <filesystem>
#include <string>
#include <vector>

namespace cora::driver
{
    struct SourceUnit
    {
        std::filesystem::path path;
        std::string sourceFileId;
        cora::core::Digest sourceDigest;
    };
    struct CodegenUnit
    {
        std::string id;
        std::vector<std::string> sourceFileIds;
        std::vector<std::string> imports;
    };
    struct BuildPlan
    {
        std::vector<SourceUnit> sourceUnits;
        std::vector<CodegenUnit> codegenUnits;
    };
} // namespace cora::driver

#endif // CORA_DRIVER_BUILDPLAN_H
