#ifndef CORA_CIR_CIR_H
#define CORA_CIR_CIR_H

#include <Cora/AST/AST.h>
#include <Cora/Core/Digest.h>
#include <Cora/Parse/Parser.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace cora::cir
{

    struct CIR
    {

        using IIndex = uint32_t; // CIR Instruction reference
        using EIndex = uint32_t; // extra data reference
        using BIndex = uint32_t; // body reference

        static constexpr IIndex INull = 0;
        static constexpr IIndex ENull = 0;
        static constexpr IIndex BNull = 0;

        enum class Tag : uint8_t
        {
        };

        struct Inst
        {
            Tag tag;
            union
            {
            } data;
        };

        struct Body
        {
            std::vector<Inst> inst;
            std::vector<uint32_t> extra;
        };

        std::vector<Body> bodies;
        std::vector<std::string_view> strings;
    };

    struct ResLoc
    {
        enum class Tag : uint8_t
        {
            None,
        };
        Tag tag = Tag::None;
        CIR::IIndex ref = 0;

        static ResLoc None()
        {
            return {Tag::None, 0};
        }
    };

    struct GenCir
    {
        bool is_comptime = false;
        CIR::Body *body = nullptr;
        GenCir *parent = nullptr;
        ResLoc rl = ResLoc::None();
        CIR::IIndex break_blk = CIR::INull;
        CIR::IIndex continue_blk = CIR::INull;
    };

} // namespace cora::cir

#endif // CORA_CIR_CIR_H
