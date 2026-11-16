#ifndef CORA_AST_NODE_H
#define CORA_AST_NODE_H

#include <cstdint>

// ---------------------------------------------------------------------------
// Cora AST node
//
// The layout deliberately mirrors Zig's AST node:
//
//     struct Node {
//         Tag tag;
//         TokenIndex main_token;
//         Data data;          // 12-byte payload union
//     };
//
// Nodes are stored in a structure-of-arrays `NodeList` (tag/main_token/data
// columns), and variable-sized payloads live in a separate `extra_data` array.
// A node is therefore only 20 bytes and the parser can scan just the tag
// column without touching payloads.
// ---------------------------------------------------------------------------

#define CORA_AST_NODE_TAGS(X) \
    X(Root)                   \
    X(ImportDecl)             \
    X(StructDecl)             \
    X(FieldDecl)              \
    X(FnDecl)                 \
    X(Param)                  \
    X(LetDecl)                \
    X(ConstDecl)              \
    X(TypeDecl)               \
    X(Block)                  \
    X(ExprStmt)               \
    X(IfSimple)               \
    X(IfFull)                 \
    X(While)                  \
    X(ForC)                   \
    X(ForRange)               \
    X(Return)                 \
    X(Break)                  \
    X(Continue)               \
    X(Defer)                  \
    X(Identifier)             \
    X(IntLit)                 \
    X(FloatLit)               \
    X(StringLit)              \
    X(CharLit)                \
    X(BoolLit)                \
    X(NullLit)                \
    X(Unary)                  \
    X(Binary)                 \
    X(Assign)                 \
    X(Call)                   \
    X(FieldAccess)            \
    X(IndexExpr)              \
    X(ArrayInit)              \
    X(StructInit)             \
    X(Paren)                  \
    X(TypeName)               \
    X(TypeArray)              \
    X(TypePointer)            \
    X(TypeFn)                 \
    X(Malformed)

namespace cora::ast
{

    struct Node
    {
        // -------- fundamental index types ------------------------------------
        using NIndex = std::uint32_t; // node reference
        using TIndex = std::uint32_t; // token reference
        using EIndex = std::uint32_t; // reference into extra_data

        static constexpr NIndex NNull = 0; // "no node"
        static constexpr EIndex ENull = 0; // "empty list"
        static constexpr TIndex TNull = UINT32_MAX; // "no token"

        enum class Tag : std::uint8_t
        {
#define CORA_AST_TAG_ENUM(name) name,
            CORA_AST_NODE_TAGS(CORA_AST_TAG_ENUM)
#undef CORA_AST_TAG_ENUM
                Count
        };

        // -------- Node payload ------------------------------------------------
        //
        // Every member fits in 12 bytes. Leaf nodes only use `main_token`.
        // Layouts are intentionally generic where Zig uses extra_data:
        // lists of child node indices are stored in `AST::extra_data`.
        union Data
        {
            NIndex lhs;
            NIndex rhs;

            struct
            {
                NIndex lhs;
                NIndex rhs;
            } node_and_node;

            struct
            {
                NIndex lhs;
                TIndex rhs;
            } node_and_token;

            struct
            {
                EIndex at;
                std::uint32_t count;
            } extra_data;

            // LetDecl / ConstDecl / FieldDecl: optional type + optional init.
            struct
            {
                NIndex type;
                NIndex init;
            } let_decl;

            // If / While: condition + body.
            struct
            {
                NIndex cond;
                NIndex then_body;
            } cond_then;

            // IfFull: condition + then body + else body.
            struct
            {
                NIndex cond;
                NIndex then_body;
                NIndex else_body;
            } cond_then_else;

            // Call: callee + arguments in extra_data.
            struct
            {
                NIndex callee;
                EIndex args;
                std::uint32_t arg_count;
            } call;

            // StructInit: type + [name_token, value_node] pairs in extra_data.
            struct
            {
                NIndex type;
                EIndex fields;
                std::uint32_t field_count;
            } struct_init;

            // FnDecl / TypeFn: parameters in extra_data + body/return type.
            struct
            {
                EIndex params;
                std::uint32_t param_count;
                NIndex body;
            } fn_decl;

            // ForC: [init, condition, step] in extra_data + loop body.
            struct
            {
                EIndex parts;
                std::uint32_t part_count;
                NIndex body;
            } for_c;

            // ForRange: binding (or NNull), iterable expression, loop body.
            struct
            {
                NIndex binding;
                NIndex iterable;
                NIndex body;
            } for_range;

            // FieldAccess: base expression + field token.
            struct
            {
                NIndex base;
                TIndex field;
            } field_access;

            // Param: type + optional default value.
            struct
            {
                NIndex type;
                NIndex default_value;
            } param;

            std::uint32_t error;
        };

        Tag tag = Tag::Malformed;
        // (3 bytes tail padding)
        TIndex main_token = TNull;
        Data data{};
    };

    static_assert(sizeof(Node) <= 24, "AST node must stay compact");

    [[nodiscard]] inline const char *tagName(Node::Tag tag) noexcept
    {
        switch (tag)
        {
#define CORA_AST_TAG_CASE(name) \
    case Node::Tag::name:       \
        return #name;
            CORA_AST_NODE_TAGS(CORA_AST_TAG_CASE)
#undef CORA_AST_TAG_CASE
        case Node::Tag::Count:
            return "Count";
        }
        return "?";
    }

} // namespace cora::ast

#undef CORA_AST_NODE_TAGS

#endif // CORA_AST_NODE_H
