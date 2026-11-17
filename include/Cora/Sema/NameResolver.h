#ifndef CORA_SEMA_NAMERESOLVER_H
#define CORA_SEMA_NAMERESOLVER_H

#include <Cora/CIR/CIR.h>
#include <Cora/Sema/SymbolTable.h>
#include <string>
#include <vector>

namespace cora::sema
{
    class NameResolver final
    {
    public:
        [[nodiscard]] bool resolve(const cir::CIR &module, SymbolTable &symbols,
                                   std::vector<std::string> &errors) const;
    };
} // namespace cora::sema

#endif