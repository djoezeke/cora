/**
 * CIR — untyped, flat, index-based; one Body per function.
 * Purpose - Type inference, trait resolution, and incremental building.
 * generated after initial parsing, macro expansion, and name resolution.
 */
#ifndef CORA_CIR_CIR_H
#define CORA_CIR_CIR_H

#include <Cora/AST/AST.h>
#include <Cora/CIR/Inst.h>
#include <Cora/CIR/ResLoc.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace cora::cir
{

    // ============================================================================
    // CIR — untyped, flat, index-based; one Body per function
    // ============================================================================

    struct CIR
    {

        using IIndex = uint32_t; // CIR Instruction reference
        using EIndex = uint32_t; // extra data reference
        using BIndex = uint32_t; // body reference

        static constexpr IIndex INull = 0;
        static constexpr EIndex ENull = 0;
        static constexpr BIndex BNull = 0xFFFFFFFF;

        enum class Tag : uint8_t
        { // Control flow
            Return,
            ReturnVoid,
            Break,
            BreakVoid,
            Block,  // body_len payload; children follow inline
            CondBr, // cond, then_block, else_block (all Block insts)
            // Arithmetic / comparison
            Add,
            Sub,
            Mul,
            Div,
            Mod,
            CmpEq,
            CmpNe,
            CmpLt,
            CmpLtEq,
            CmpGt,
            CmpGtEq,
            // Unary
            Neg,
            Not,
            // Values
            Int,
            Float,
            Str,
            Bool,
            NullLit,
            // References
            Ref,   // name → Sema resolves to local/decl
            Load,  // deref a Ref
            Store, // write through a Ref
            // Aggregates / calls
            Call,      // callee + args_extra
            ArrayInit, // elems_extra
            ElemPtr,   // base + idx
            FieldPtr,  // base + field token
            // Declarations (root body only)
            LetDecl, // name, init
            FnDecl,  // name, body_idx, params_extra
            // Error
            ErrTok, // poisoned; carries err_tag
        };

        std::vector<Body> bodies;
        std::vector<std::string_view> strings;

        // ---- helpers ----
        EIndex addList(Body &b, std::span<const uint32_t> items)
        {
            if (items.empty())
                return ENull;
            EIndex at = static_cast<EIndex>(b.extra.size());
            b.extra.push_back(static_cast<uint32_t>(items.size()));
            for (auto i : items)
                b.extra.push_back(i);
            return at;
        }
        std::span<const uint32_t> getList(const Body &b, EIndex at) const
        {
            if (at == ENull)
                return {};
            return {&b.extra[at + 1], b.extra[at]};
        }

        uint32_t internString(std::string_view s)
        {
            for (size_t i = 1; i < strings.size(); ++i)
                if (strings[i] == s)
                    return static_cast<uint32_t>(i);
            strings.push_back(s);
            return static_cast<uint32_t>(strings.size() - 1);
        }
    };

    void dumpCir(const CIR &z)
    {
        for (size_t bi = 0; bi < z.bodies.size(); ++bi)
        {
            std::cout << "Body[" << bi << "]:\n";
            const auto &b = z.bodies[bi];
            for (size_t i = 0; i < b.insts.size(); ++i)
            {
                const auto &inst = b.insts[i];
                std::cout << "  [" << i << "] " << cirTagName(inst.tag);
                switch (inst.tag)
                {
                case Tag::Int:
                    std::cout << " " << inst.data.int_value;
                    break;
                case Tag::Ref:
                    std::cout << " \"" << z.strings[inst.data.name_idx] << "\"";
                    break;
                case Tag::Add:
                case Tag::Sub:
                case Tag::Mul:
                case Tag::Div:
                    std::cout << " " << inst.data.bin.lhs
                              << " " << inst.data.bin.rhs;
                    break;
                case Tag::Store:
                    std::cout << " " << inst.data.bin.lhs
                              << " <- " << inst.data.bin.rhs;
                    break;
                case Tag::Load:
                    std::cout << " " << inst.data.unary_op.target;
                    break;
                case Tag::Return:
                    std::cout << " " << inst.data.unary_op.target;
                    break;
                case Tag::Block:
                    std::cout << " body_len=" << inst.data.block.body_len;
                    break;
                case Tag::CondBr:
                    std::cout << " cond=" << inst.data.condbr.cond
                              << " then=" << inst.data.condbr.then_blk
                              << " else=" << inst.data.condbr.else_blk;
                    break;
                case Tag::Call:
                    std::cout << " callee=" << inst.data.call.callee
                              << " args=[";
                    {
                        auto list = z.getList(b, inst.data.call.args);
                        for (size_t k = 0; k < list.size(); ++k)
                            std::cout << (k ? "," : "") << list[k];
                    }
                    std::cout << "]";
                    break;
                case Tag::LetDecl:
                    std::cout << " " << z.strings[inst.data.let_decl.name]
                              << " = " << inst.data.let_decl.init;
                    break;
                case Tag::FnDecl:
                    std::cout << " " << z.strings[inst.data.fn_decl.name]
                              << " body=" << inst.data.fn_decl.body
                              << " params=" << inst.data.fn_decl.param_count;
                    break;
                case Tag::ErrTok:
                    std::cout << " err=" << inst.data.err_tag;
                    break;
                default:
                    break;
                }
                std::cout << "\n";
            }
        }
    }
} // namespace cora::cir

#endif // CORA_CIR_CIR_H
