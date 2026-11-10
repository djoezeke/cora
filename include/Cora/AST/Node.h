#ifndef CORA_AST_NODES_H
#define CORA_AST_NODES_H

#include <cstdint>

#define NODES_EXPRESSION

#define NODES_STATEMENT

namespace cora::ast
{

    // -------- Node: 20 bytes, tag + main_token + 12-byte union ---------------
    struct Node
    {
        // -------- fundamental index types ----------------------------------------

        using NIndex = uint32_t; // node reference
        using TIndex = uint32_t; // token reference
        using EIndex = uint32_t; // reference into extra_data

        static constexpr NIndex NNull = 0; // "no node"
        static constexpr EIndex ENull = 0; // "empty list"

        enum class Tag : uint8_t
        {
#define F()

            Root,

            // Statements
            Block,
            ExprStmt,
            LetDecl,
            ConstDecl,
            IfSimple,
            IfFull,
            While,
            Return,
            Break,
            Continue,
            FnDecl,

            // Expressions
            Identifier,
            Number,
            String,
            BoolLit,
            NullLit,
            Unary,
            Binary,
            Assign,
            Call,
            FieldAccess,
            IndexExpr,
            ArrayInit,
            Paren,
#undef F
        };

        union Data
        {
            // Unary: -expr, !expr
            NIndex unary_operand;

            // Binary / Assign: lhs op rhs (op = main_token)
            struct
            {
                NIndex lhs;
                NIndex rhs;
            } binary;

            // Call: callee(args...)    open paren = main_token
            struct
            {
                NIndex callee;
                EIndex args_extra;
                uint32_t arg_count;
            } call;

            // Field access: base.field
            struct
            {
                NIndex base;
                TIndex field_tok;
            } field;

            // NIndex expression: base[index]
            struct
            {
                NIndex base;
                NIndex idx;
            } index_expr;

            // Block: { stmts_extra, count }
            struct
            {
                EIndex stmts_extra;
                uint32_t stmt_count;
            } block;

            // If: cond/then/(else)
            struct
            {
                NIndex cond;
                NIndex then_body;
            } if_simple;
            struct
            {
                NIndex cond;
                NIndex then_body;
                NIndex else_body;
            } if_full;

            // While: cond / body
            struct
            {
                NIndex cond;
                NIndex body;
            } while_stmt;

            // LetDecl / ConstDecl: name = main_token, init = opt
            struct
            {
                NIndex init;
            } let_decl;

            // Return: optional value (Null if void)
            NIndex ret_value;

            // FnDecl: name = main_token; params in extra; body
            struct
            {
                EIndex params_extra;
                uint32_t param_count;
                NIndex body;
            } fn_decl;

            // ArrayInit: [a, b, c]
            struct
            {
                EIndex elems_extra;
                uint32_t elem_count;
            } array_init;

            EIndex block_stmts;

            // Paren / ExprStmt: inner expr
            NIndex inner;
        };

        Tag tag;
        // (3 bytes padding)
        TIndex token;
        Data data;
    };

    static const char *tagName(Node::Tag t)
    {
#define F()

        switch (t)
        {
        case Node::Tag::Root:
            return "Root";
        case Node::Tag::Block:
            return "Block";
        case Node::Tag::ExprStmt:
            return "ExprStmt";
        case Node::Tag::LetDecl:
            return "LetDecl";
        case Node::Tag::ConstDecl:
            return "ConstDecl";
        case Node::Tag::IfSimple:
            return "IfSimple";
        case Node::Tag::IfFull:
            return "IfFull";
        case Node::Tag::While:
            return "While";
        case Node::Tag::Return:
            return "Return";
        case Node::Tag::Break:
            return "Break";
        case Node::Tag::Continue:
            return "Continue";
        case Node::Tag::FnDecl:
            return "FnDecl";
        case Node::Tag::Identifier:
            return "Identifier";
        case Node::Tag::Number:
            return "Number";
        case Node::Tag::String:
            return "String";
        case Node::Tag::BoolLit:
            return "BoolLit";
        case Node::Tag::NullLit:
            return "NullLit";
        case Node::Tag::Unary:
            return "Unary";
        case Node::Tag::Binary:
            return "Binary";
        case Node::Tag::Assign:
            return "Assign";
        case Node::Tag::Call:
            return "Call";
        case Node::Tag::FieldAccess:
            return "FieldAccess";
        case Node::Tag::IndexExpr:
            return "IndexExpr";
        case Node::Tag::ArrayInit:
            return "ArrayInit";
        case Node::Tag::Paren:
            return "Paren";
        }
        return "?";

#undef F
    }
}

#endif // CORA_AST_NODES_H
