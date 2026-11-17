#ifndef CORA_AIR_AIR_H
#define CORA_AIR_AIR_H

#include <Cora/CIR/CIR.h>

#include <cstdint>
#include <string>
#include <vector>

namespace cora::air
{
    struct AIRInstruction
    {
        std::uint32_t id = 0;
        cir::IIndex cir_index = cir::INull;
        cir::Tag tag = cir::Tag::Invalid;
        cir::IIndex a = cir::INull;
        cir::IIndex b = cir::INull;
        cir::IIndex c = cir::INull;
        cora::Token::Tag op = cora::Token::Tag::T_EOF;
        std::int64_t int_value = 0;
        double float_value = 0.0;
        bool bool_value = false;
        std::uint32_t name = cir::TNull;
        cir::TIndex type_ref = cir::TNull;
        cir::EIndex extra = cir::ENull;
        std::uint32_t extra_count = 0;
        std::uint32_t flags = 0;
        core::SourceSpan span{};
    };

    struct AIRFunction
    {
        std::uint32_t body = cir::BNull;
        std::uint32_t first_instruction = 0;
        std::uint32_t instruction_count = 0;
    };

    /// Analysis IR keeps references into the immutable CIR and stores hot
    /// instruction columns contiguously. It is intentionally free of AST
    /// ownership so cached CIR can be lowered repeatedly.
    struct AIRModule
    {
        core::Digest input{};
        std::string moduleId;
        std::vector<AIRInstruction> instructions;
        std::vector<AIRFunction> functions;
        std::vector<std::string> errors;
        bool verified = false;
    };

    class SemanticAnalyzer final
    {
    public:
        [[nodiscard]] AIRModule analyze(const cir::CIRArtifact &artifact) const;
        [[nodiscard]] AIRModule analyze(const cir::CIR &module) const;
    };

    class AIRPassManager final
    {
    public:
        [[nodiscard]] AIRModule optimize(AIRModule module,
                                         int optimizationLevel) const;
    };
} // namespace cora::air

#endif // CORA_AIR_AIR_H
