#ifndef CORA_CIR_GENCIR_H
#define CORA_CIR_GENCIR_H

#include <Cora/CIR/CIR.h>
#include <Cora/CIR/Inst.h>
#include <Cora/CIR/ResLoc.h>

#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

namespace cora::cir
{
    /// Small generation context shared by ASTGen and the AIR lowerer.
    struct GenCir
    {
        CIR *cir = nullptr;
        Body *body = nullptr;
        GenCir *parent = nullptr;
        BIndex break_body = BNull;
        BIndex continue_body = BNull;
        ResLoc result = ResLoc::None();
        bool comptime = false;

        [[nodiscard]] IIndex emit(Inst inst)
        {
            return body != nullptr ? body->add(inst) : INull;
        }

        [[nodiscard]] EIndex addList(const std::vector<std::uint32_t> &items)
        {
            return body != nullptr ? body->addList(items) : ENull;
        }

        [[nodiscard]] std::uint32_t intern(std::string_view text)
        {
            return cir != nullptr ? cir->internString(text) : TNull;
        }
    };
} // namespace cora::cir

#endif // CORA_CIR_GENCIR_H
