#ifndef CORA_CODEGEN_CBACKEND_H
#define CORA_CODEGEN_CBACKEND_H

#include <Cora\AIR\AIR.h>
#include <Cora\CIR\CIR.h>

#include <string>

namespace cora::codegen
{
    class CBackend final
    {
    public:
        [[nodiscard]] std::string generate(const cir::CIR &module,
                                           const air::AIRModule &air) const;
    };
} // namespace cora::codegen

#endif
