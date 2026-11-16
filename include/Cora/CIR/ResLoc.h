#ifndef CORA_CIR_RESLOC_H
#define CORA_CIR_RESLOC_H

#include <Cora/CIR/Inst.h>

#include <cstdint>

namespace cora::cir
{
    /// Result location: where the value of an expression should be written.
    struct ResLoc
    {
        enum class Tag : std::uint8_t
        {
            None,
            Discard,
            Ref,
            Return,
        };

        Tag tag = Tag::None;
        IIndex ref = INull;

        [[nodiscard]] static ResLoc None() { return {Tag::None, INull}; }
        [[nodiscard]] static ResLoc Discard() { return {Tag::Discard, INull}; }
        [[nodiscard]] static ResLoc Return() { return {Tag::Return, INull}; }
        [[nodiscard]] static ResLoc Ref(IIndex value) { return {Tag::Ref, value}; }
    };
} // namespace cora::cir

#endif // CORA_CIR_RESLOC_H
