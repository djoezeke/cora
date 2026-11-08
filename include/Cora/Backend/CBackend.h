#ifndef CORA_BACKEND_CBACKEND_H
#define CORA_BACKEND_CBACKEND_H

#include <Cora/CodeGen/TargetSpec.h>
#include <Cora/MIR/MIR.h>

#include <cstdint>
#include <string>
#include <vector>

namespace cora::backend
{
    struct ObjectBuffer
    {
        std::vector<std::uint8_t> bytes;
        std::string format;
    };

    class CBackend final
    {
    public:
        [[nodiscard]] static std::string emitC(const cora::mir::MIRModule &mir);
        [[nodiscard]] static ObjectBuffer emitObject(const cora::mir::MIRModule &mir,
                                                     const cora::codegen::TargetSpec &target);
    };
} // namespace cora::backend

#endif // CORA_BACKEND_CBACKEND_H
