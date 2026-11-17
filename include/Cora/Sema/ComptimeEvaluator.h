#ifndef CORA_SEMA_COMPTIMEEVALUATOR_H
#define CORA_SEMA_COMPTIMEEVALUATOR_H

#include <Cora/CIR/CIR.h>

#include <cstdint>
#include <optional>

namespace cora::sema
{
    class ComptimeEvaluator final
    {
    public:
        [[nodiscard]] std::optional<std::int64_t> evaluate(const cir::CIR &module,
                                                            cir::BIndex body,
                                                            cir::IIndex instruction) const;
    };
} // namespace cora::sema

#endif