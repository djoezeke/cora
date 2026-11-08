#ifndef CORA_AIR_AIRPASSES_H
#define CORA_AIR_AIRPASSES_H

#include <Cora/AIR/AIRTypes.h>

namespace cora::air
{
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
}

#endif
