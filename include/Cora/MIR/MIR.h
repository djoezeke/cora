#ifndef CORA_MIR_MIR_H
#define CORA_MIR_MIR_H

#include <Cora/AIR/AIR.h>
#include <Cora/CodeGen/TargetSpec.h>
#include <Cora/Core/Digest.h>

#include <memory>
#include <string>
#include <vector>

namespace cora::mir
{
    struct MIRInstruction { std::string opcode; std::vector<std::string> operands; };
    struct MIRModule
    {
        cora::core::Digest input;
        cora::codegen::TargetSpec target;
        std::shared_ptr<cora::ast::AST> ast;
        std::vector<cora::air::AIRFunction> functions;
        std::vector<MIRInstruction> instructions;
        bool allocated = false;
    };
    class TargetLowerer final
    {
    public:
        [[nodiscard]] static MIRModule lower(const cora::air::AIRModule &air,
                                             const cora::codegen::TargetSpec &target);
    };
    class MachinePassManager final
    {
    public:
        [[nodiscard]] static MIRModule run(MIRModule module,
                                           const std::string &codegenFlags);
    };
} // namespace cora::mir

#endif // CORA_MIR_MIR_H
