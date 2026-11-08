#ifndef CORA_CODEGEN_OBJECTINPUT_H
#define CORA_CODEGEN_OBJECTINPUT_H

#include <Cora/CodeGen/TargetSpec.h>
#include <Cora/Core/Digest.h>

#include <string>
#include <vector>

namespace cora::codegen
{
    struct ObjectInput
    {
        std::string codegenUnitId;
        std::vector<cora::core::Digest> cirInputKeys;
        std::vector<cora::core::Digest> interfaceDigests;
        std::vector<cora::core::Digest> implementationDigests;
        std::string semanticVersion = "cora-object-v2";
        TargetSpec target;
        std::string optimizationFlags;
        std::string debugFlags;
        std::string sanitizerFlags;
        std::string relocationFlags;
        std::string unwindFlags;
        std::string ltoFlags;
        std::string backendId = "cora-c-backend-v1";
        std::string toolchainId = "cora-toolchain-v1";
        [[nodiscard]] std::string canonical() const;
        [[nodiscard]] cora::core::Digest key() const;
    };
} // namespace cora::codegen

#endif // CORA_CODEGEN_OBJECTINPUT_H
