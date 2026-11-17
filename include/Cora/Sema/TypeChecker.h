#ifndef CORA_SEMA_TYPECHECKER_H
#define CORA_SEMA_TYPECHECKER_H

#include <Cora/CIR/CIR.h>
#include <Cora/Sema/Types.h>

#include <string>
#include <vector>

namespace cora::sema
{
    class TypeChecker final
    {
    public:
        [[nodiscard]] bool check(const cir::CIR &module, Types &types,
                                 std::vector<std::string> &errors) const;
    };
} // namespace cora::sema

#endif