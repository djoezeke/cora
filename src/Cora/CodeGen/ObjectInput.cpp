#include <Cora/CodeGen/ObjectInput.h>
#include <Cora/Core/SHA256.h>

#include <algorithm>
#include <sstream>

namespace cora::codegen
{

    std::string TargetSpec::canonical() const
    {
        std::vector<std::string> normalizedFeatures = features;
        std::sort(normalizedFeatures.begin(), normalizedFeatures.end());
        normalizedFeatures.erase(std::unique(normalizedFeatures.begin(), normalizedFeatures.end()),
                                 normalizedFeatures.end());

        std::ostringstream output;
        output << "triple=" << triple << '\n'
               << "object-format=" << objectFormat << '\n'
               << "abi=" << abi << '\n'
               << "cpu=" << cpu << '\n'
               << "features=";
        for (std::size_t i = 0; i < normalizedFeatures.size(); ++i)
        {
            if (i != 0)
                output << ',';
            output << normalizedFeatures[i];
        }
        output << '\n';
        return output.str();
    }

    std::string ObjectInput::canonical() const
    {
        std::ostringstream output;
        output << "semantic=" << semanticVersion << '\n'
               << "unit=" << codegenUnitId << '\n'
               << target.canonical()
               << "optimization=" << optimizationFlags << '\n'
               << "debug=" << debugFlags << '\n'
               << "sanitizer=" << sanitizerFlags << '\n'
               << "relocation=" << relocationFlags << '\n'
               << "unwind=" << unwindFlags << '\n'
               << "lto=" << ltoFlags << '\n'
               << "backend=" << backendId << '\n'
               << "toolchain=" << toolchainId << '\n'
               << "cir=";
        for (const auto &digest : cirInputKeys)
            output << digest.hex() << ',';
        output << '\n' << "interfaces=";
        for (const auto &digest : interfaceDigests)
            output << digest.hex() << ',';
        output << '\n' << "implementations=";
        for (const auto &digest : implementationDigests)
            output << digest.hex() << ',';
        output << '\n';
        return output.str();
    }

    cora::core::Digest ObjectInput::key() const
    {
        return cora::crypto::SHA256::hash("cora-object-key-v2\n" + canonical());
    }

} // namespace cora::codegen
