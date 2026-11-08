#ifndef CORA_CODEGEN_TARGETSPEC_H
#define CORA_CODEGEN_TARGETSPEC_H

#include <string>
#include <vector>

namespace cora::codegen
{
    struct TargetSpec
    {
        std::string triple = "native";
        std::string objectFormat = "cora-object";
        std::string abi = "default";
        std::string cpu = "generic";
        std::vector<std::string> features;
        [[nodiscard]] std::string canonical() const;
    };
} // namespace cora::codegen

#endif // CORA_CODEGEN_TARGETSPEC_H
