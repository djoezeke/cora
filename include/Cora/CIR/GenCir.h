/**
 * CIR — untyped, flat, index-based; one Body per function.
 * Purpose - Type inference, trait resolution, and incremental building.
 * generated after initial parsing, macro expansion, and name resolution.
 */
#ifndef CORA_CIR_GENCIR_H
#define CORA_CIR_GENCIR_H

#include <Cora/CIR/CIR.h>
#include <Cora/CIR/Inst.h>
#include <Cora/CIR/ResLoc.h>
#include <cstdint>

namespace cora::cir
{

    // ============================================================================
    // GenCir — the per-scope generation context
    // ============================================================================
    struct GenCir
    {
        Body *body = nullptr;                  // body we emit into
        uint32_t first_inst = 0;               // index into body->insts where this scope starts
        GenCir *parent = nullptr;              // enclosing scope (for gen.zed scope chain)
        CIR::IIndex break_blk = CIR::INull;    // block to break to (labeled block / loop)
        CIR::IIndex continue_blk = CIR::INull; // loop body (for continue)
        uint32_t loop_depth = 0;               // 0 means break/continue are valid
        ResLoc rl = ResLoc::None();            // result location for the scope
        bool is_comptime = false;              // future: comptime flag

        // Emit an instruction into this body.
        CIR::IIndex emit(Inst i)
        {
            auto at = static_cast<CIR::IIndex>(body->insts.size());
            body->insts.push_back(i);
            return at;
        }

        // Patch a Block's body_len after its sub-tree was emitted.
        void closeBlock(CIR::IIndex blk)
        {
            body->insts[blk].data.block.body_len =
                static_cast<uint32_t>(body->insts.size()) - blk - 1;
        }
    };

} // namespace cora::cir

#endif // CORA_CIR_GENCIR_H
