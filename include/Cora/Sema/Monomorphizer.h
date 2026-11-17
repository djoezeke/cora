#ifndef CORA_SEMA_MONOMORPHIZER_H
#define CORA_SEMA_MONOMORPHIZER_H

#include <Cora\CIR\CIR.h>

#include <string>
#include <vector>

namespace cora::sema
{
    class Monomorphizer final
    {
    public:
        [[nodiscard]] cir::CIR run(const cir::CIR &module,
                                    std::vector<std::string> *instances = nullptr) const;
    };
} // namespace cora::sema

#endif
