/**
 * CIR — untyped, flat, index-based; one Body per function.
 * Purpose - Type inference, trait resolution, and incremental building.
 * generated after initial parsing, macro expansion, and name resolution.
 */
#ifndef CORA_CIR_INST_H
#define CORA_CIR_INST_H

#include <cstdint>

namespace cora::cir
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

    struct Inst
    {
        Tag tag;
        union
        {
            IIndex un; // unary
            struct
            {
                IIndex lhs, rhs;
            } bin; // binary
            struct
            {
                uint32_t body_len;
            } block; // body is *inline*
            struct
            {
                IIndex block;
            } brk; // Break target
            struct
            {
                IIndex cond, then_blk, else_blk;
            } condbr;
            struct
            {
                IIndex callee;
                EIndex args;
            } call;
            int64_t int_value;
            double float_value;
            bool bool_value;
            uint32_t str_idx;  // into CIR.strings
            uint32_t name_idx; // into CIR.strings
            struct
            {
                IIndex target;
            } unary_op; // for Load/Store/Neg/Not
            struct
            {
                IIndex base, idx;
            } elem;
            struct
            {
                IIndex base;
                uint32_t field;
            } field_ptr;
            struct
            {
                uint32_t name;
                IIndex init;
            } let_decl;
            struct
            {
                uint32_t name;
                IIndex body;
                EIndex params;
                uint32_t param_count;
            } fn_decl;
            uint16_t err_tag;

        } data;
    };

    struct Body
    {
        std::vector<Inst> insts;
        std::vector<uint32_t> extra;
    };

    static const char *cirTagName(Tag t)
    {
        switch (t)
        {
        case Tag::Return:
            return "Return";
        case Tag::ReturnVoid:
            return "ReturnVoid";
        case Tag::Break:
            return "Break";
        case Tag::BreakVoid:
            return "BreakVoid";
        case Tag::Block:
            return "Block";
        case Tag::CondBr:
            return "CondBr";
        case Tag::Add:
            return "Add";
        case Tag::Sub:
            return "Sub";
        case Tag::Mul:
            return "Mul";
        case Tag::Div:
            return "Div";
        case Tag::Int:
            return "Int";
        case Tag::Ref:
            return "Ref";
        case Tag::Load:
            return "Load";
        case Tag::Store:
            return "Store";
        case Tag::Call:
            return "Call";
        case Tag::LetDecl:
            return "LetDecl";
        case Tag::FnDecl:
            return "FnDecl";
        case Tag::ErrTok:
            return "ErrTok";
        default:
            return "?";
        }
    }

} // namespace cora::cir

#endif // CORA_CIR_INST_H
