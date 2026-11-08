#ifndef CORA_MIR_MIRTYPES_H
#define CORA_MIR_MIRTYPES_H

#include <Cora/AIR/AIRTypes.h>
#include <Cora/CodeGen/TargetSpec.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cora::mir
{
    struct MIRInstruction
    {
        std::string opcode;
        std::vector<std::string> operands;
        std::int32_t resultRegister = -1;
    };
    struct MIRModule
    {
        cora::core::Digest input;
        cora::codegen::TargetSpec target;
        std::shared_ptr<cora::ast::AST> ast;
        std::vector<cora::air::AIRFunction> functions;
        std::vector<MIRInstruction> instructions;
        std::vector<std::int32_t> physicalRegisters;
        bool allocated = false;
    };
}

#endif
