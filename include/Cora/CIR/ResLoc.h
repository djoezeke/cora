/**
 * CIR — untyped, flat, index-based; one Body per function.
 * Purpose - Type inference, trait resolution, and incremental building.
 * generated after initial parsing, macro expansion, and name resolution.
 */
#ifndef CORA_CIR_RESLOC_H
#define CORA_CIR_RESLOC_H

#include <Cora/CIR/CIR.h>
#include <cstdint>

namespace cora::cir
{

    // ============================================================================
    // ResLoc — Result Location. Answers: "where should this expression's value go?"
    // Lets us elide temporaries (e.g. `let x = f()` writes straight into x's slot).
    // ============================================================================

    struct ResLoc
    {
        enum class Tag : uint8_t
        {
            None,    // no preference; caller will Load if needed
            Discard, // value is thrown away (e.g. expression statement)
            Ref,     // write through a specific Ref inst index
            Return,  // write to the enclosing function's return slot
        };

        Tag tag = Tag::None;
        CIR::IIndex ref = 0;

        static ResLoc None()
        {
            return {Tag::None, 0};
        };

        static ResLoc Discard()
        {
            return {Tag::Discard, 0};
        };

        static ResLoc Return()
        {
            return {Tag::Return, 0};
        };

        static ResLoc Ref(CIR::IIndex r)
        {
            return {Tag::Ref, r};
        };
    };

} // namespace cora::cir

#endif // CORA_CIR_RESLOC_H
