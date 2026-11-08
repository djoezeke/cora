#ifndef CORA_MIR_MIRPASSES_H
#define CORA_MIR_MIRPASSES_H

#include <Cora/MIR/MIRTypes.h>

namespace cora::mir
{
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
}

#endif
