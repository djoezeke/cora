#ifndef CORA_AIR_AIR_H
#define CORA_AIR_AIR_H

#include <Cora/CIR/CIR.h>
#include <Cora/Core/Digest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace cora::air
{
    struct AIRInstruction { std::string opcode; std::string operand; };
    struct AIRFunction
    {
        std::string name;
        cora::ast::TypeRef returnType;
        std::size_t parameterCount = 0;
        std::size_t statementCount = 0;
    };
    struct AIRModule
    {
        cora::core::Digest input;
        std::string moduleId;
        std::shared_ptr<cora::ast::AST> ast;
        std::vector<AIRFunction> functions;
        std::vector<AIRInstruction> instructions;
        std::vector<std::string> errors;
        bool verified = false;
    };
    class SemanticAnalyzer final
    {
    public:
        [[nodiscard]] static AIRModule analyze(const cora::cir::CIRArtifact &cir);
    };
    class AIRPassManager final
    {
    public:
        [[nodiscard]] static AIRModule optimize(AIRModule module, int optimizationLevel);
    };
} // namespace cora::air

#endif // CORA_AIR_AIR_H
