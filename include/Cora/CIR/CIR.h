#ifndef CORA_CIR_CIR_H
#define CORA_CIR_CIR_H

#include <Cora/AST/AST.h>
#include <Cora/Core/Digest.h>
#include <Cora/Parse/Parser.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace cora::cir
{
    struct CIRInstruction
    {
        std::string opcode;
        std::vector<std::string> operands;
        std::uint32_t opcodeId = 0;
        std::uint32_t extraOffset = 0;
        std::uint32_t extraCount = 0;
        std::uint32_t stringOffset = 0;
        std::uint32_t sourceLocation = 0;
        cora::ast::SourceRange range{};
    };

    struct CIRInstructionArray
    {
        std::vector<CIRInstruction> values;
    };

    struct CIRSourceLocation
    {
        cora::ast::SourceRange range{};
        std::uint32_t instruction = 0;
    };

    struct CIRInput
    {
        std::string sourceFileId;
        std::string moduleId;
        cora::core::Digest sourceDigest;
        std::string frontendVersion = "cora-cir-v2";
        std::string frontendOptions;
        std::vector<cora::core::Digest> generatedInputDigests;
        [[nodiscard]] std::string canonical() const;
        [[nodiscard]] cora::core::Digest key() const;
    };
    struct CIRModule
    {
        std::size_t tokenCount = 0;
        std::size_t lineCount = 1;
        std::size_t functionCount = 0;
        std::shared_ptr<cora::ast::AST> ast;
        std::vector<CIRInstruction> instructions;
        std::vector<std::uint32_t> extra;
        std::vector<std::uint8_t> stringBytes;
        std::vector<CIRSourceLocation> sourceLocations;
    };

    struct CIRArtifact
    {
        cora::core::Digest key;
        CIRInput input;
        CIRModule module;
        cora::core::Digest publicInterfaceDigest;
        [[nodiscard]] std::vector<std::uint8_t> serialize() const;
        [[nodiscard]] static std::optional<CIRArtifact> deserialize(
            const std::vector<std::uint8_t> &bytes, std::string *error = nullptr);
    };
    [[nodiscard]] CIRArtifact build(const CIRInput &input,
                                    const cora::ParsedUnit &parsed);
} // namespace cora::cir

#endif // CORA_CIR_CIR_H
