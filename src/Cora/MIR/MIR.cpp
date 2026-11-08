#include <Cora/MIR/MIR.h>
#include <Cora/Core/SHA256.h>

#include <sstream>

namespace cora::mir
{

    MIRModule TargetLowerer::lower(const cora::air::AIRModule &air,
                                   const cora::codegen::TargetSpec &target)
    {
        MIRModule result;
        result.target = target;
        result.ast = air.ast;
        result.functions = air.functions;
        result.input = cora::crypto::SHA256::hash(air.input.hex() + "\n" + target.canonical());
        for (const auto &instruction : air.instructions)
            result.instructions.push_back({"cora." + instruction.opcode, {instruction.operand}});
        return result;
    }

    MIRModule MachinePassManager::run(MIRModule module, const std::string &codegenFlags)
    {
        module.instructions.push_back({"machine.flags", {codegenFlags}});
        module.instructions.push_back({"machine.end", {module.target.triple}});
        module.allocated = true;
        return module;
    }

} // namespace cora::mir
