#ifndef CORA_SEMA_BORROWCHECKER_H
#define CORA_SEMA_BORROWCHECKER_H

#include <Cora\CIR\CIR.h>

#include <string>
#include <vector>

namespace cora::sema
{
    struct BorrowError
    {
        cir::IIndex instruction = cir::INull;
        std::string message;
    };

    class BorrowChecker final
    {
    public:
        [[nodiscard]] std::vector<BorrowError> check(const cir::CIR &module) const;
    };
} // namespace cora::sema

#endif
