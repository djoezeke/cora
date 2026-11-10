#ifndef CORA_AST_AST_H
#define CORA_AST_AST_H

#include <Cora/AST/Node.h>

#include <span>
#include <string>

namespace cora::ast
{

    // left(ast, id)
    // right(ast, id)

    struct AST
    {
        // -------- fundamental index types ----------------------------------------
        using NIndex = Node::NIndex; // node reference
        using TIndex = Node::TIndex; // token reference
        using EIndex = Node::EIndex; // reference into extras

        using Tag = Node::Tag;
        using Node = ast::Node;

        // ----------- Storage ------------
        std::vector<Node> nodes;
        std::vector<uint32_t> extras;
        std::vector<Token> tokens;
        // std::vector<Diagnostic> errors;

        // -------- Extra-data helpers ---------------------------------------------

        EIndex addIndexList(std::span<const NIndex> list)
        {
            EIndex at = static_cast<EIndex>(extras.size());
            extras.insert(extras.end(), list.begin(), list.end());
            return at;
        }

        EIndex addTokenList(std::span<const TIndex> list)
        {
            EIndex at = static_cast<EIndex>(extras.size());
            extras.insert(extras.end(), list.begin(), list.end());
            return at;
        }

        std::span<const NIndex> getIndexList(EIndex at, uint32_t n) const
        {
            return {reinterpret_cast<const NIndex *>(extras.data() + at), n};
        }

        std::span<const TIndex> getTokenList(EIndex at, uint32_t n) const
        {
            return {reinterpret_cast<const TIndex *>(extras.data() + at), n};
        };

        // -------- Node construction ----------------------------------------------
        NIndex addNode(Node::Tag t, TIndex main_tok)
        {
            NIndex i = static_cast<NIndex>(nodes.size());
            nodes.push_back(Node{});
            nodes[i].tag = t;
            nodes[i].token = main_tok;
            return i;
        }

        NIndex makeBlock(std::vector<NIndex> stmts)
        {
            NIndex i = addNode(Node::Tag::Block, 0);
            nodes[i].data.block.stmts_extra = addIndexList(stmts);
            nodes[i].data.block.stmt_count = static_cast<uint32_t>(stmts.size());
            return i;
        }

        NIndex makeBinary(NIndex lhs, TIndex op, NIndex rhs)
        {
            NIndex i = addNode(Node::Tag::Binary, op);
            nodes[i].data.binary = {lhs, rhs};
            return i;
        }
        NIndex makeAssign(NIndex lhs, TIndex op, NIndex rhs)
        {
            NIndex i = addNode(Node::Tag::Assign, op);
            nodes[i].data.binary = {lhs, rhs};
            return i;
        }
        NIndex makeCall(NIndex callee, std::vector<NIndex> args, TIndex lp)
        {
            NIndex i = addNode(Node::Tag::Call, lp);
            nodes[i].data.call = {callee, addIndexList(args),
                                  static_cast<uint32_t>(args.size())};
            return i;
        }
        NIndex makeIf(NIndex cond, NIndex then_body, NIndex else_body)
        {
            if (else_body == Node::NNull)
            {
                NIndex i = addNode(Node::Tag::IfSimple, 0);
                nodes[i].data.if_simple = {cond, then_body};
                return i;
            }
            NIndex i = addNode(Node::Tag::IfFull, 0);
            nodes[i].data.if_full = {cond, then_body, else_body};
            return i;
        }
        NIndex makeFn(TIndex name_tok, std::vector<TIndex> params, NIndex body)
        {
            NIndex i = addNode(Node::Tag::FnDecl, name_tok);
            nodes[i].data.fn_decl = {addTokenList(params),
                                     static_cast<uint32_t>(params.size()), body};
            return i;
        }
        NIndex makeLet(TIndex name_tok, NIndex init, bool is_const)
        {
            NIndex i = addNode(is_const ? Node::Tag::ConstDecl : Node::Tag::LetDecl, name_tok);
            nodes[i].data.let_decl.init = init;
            return i;
        }

        // -------- Range computation ----------------------------------------------
        // Find the byte range spanned by a subtree. In Zig this is `nodeToToken`
        // precomputed as a side table for O(1); here we recurse for clarity.
        std::pair<uint32_t, uint32_t> rangeOf(NIndex idx) const
        {
            if (idx == Node::NNull)
                return {0, 0};
            const Node &n = nodes[idx];
            switch (n.tag)
            {
            case Node::Tag::Block:
            {
                if (n.data.block.stmt_count == 0)
                    return {0, 0};
                auto stmts = getIndexList(n.data.block.stmts_extra,
                                          n.data.block.stmt_count);
                auto lo = rangeOf(stmts.front());
                auto hi = rangeOf(stmts.back());
                return {lo.first, hi.second};
            }
            case Node::Tag::Binary:
            case Node::Tag::Assign:
            {
                auto lo = rangeOf(n.data.binary.lhs);
                auto hi = rangeOf(n.data.binary.rhs);
                return {lo.first, hi.second};
            }
            case Node::Tag::IfSimple:
            {
                auto c = rangeOf(n.data.if_simple.cond);
                auto t = rangeOf(n.data.if_simple.then_body);
                return {c.first, t.second};
            }
            case Node::Tag::IfFull:
            {
                auto c = rangeOf(n.data.if_full.cond);
                auto e = rangeOf(n.data.if_full.else_body);
                return {c.first, e.second};
            }
            case Node::Tag::Call:
            {
                auto c = rangeOf(n.data.call.callee);
                const Token &close = tokens.back(); // approximation
                return {c.first, close.loc.end};
            }
            case Node::Tag::Identifier:
            case Node::Tag::Number:
            case Node::Tag::String:
            case Node::Tag::BoolLit:
            case Node::Tag::NullLit:
            {
                const Token &t = tokens[n.token];
                return {t.loc.start, t.loc.end};
            }
            default:
            {
                const Token &t = tokens[n.token];
                return {t.loc.start, t.loc.end};
            }
            }
        }

        // -------- Debug dump ------------------------------------------------------
        void dump(std::ostream &os) const
        {
            for (NIndex i = 1; i < nodes.size(); ++i)
            {
                const Node &n = nodes[i];
                os << "[" << i << "] " << tagName(n.tag)
                   << "  tok=" << n.token;
                switch (n.tag)
                {
                case Node::Tag::Binary:
                    os << "  lhs=" << n.data.binary.lhs
                       << "  rhs=" << n.data.binary.rhs;
                    break;
                case Node::Tag::Assign:
                    os << "  lhs=" << n.data.binary.lhs
                       << "  rhs=" << n.data.binary.rhs;
                    break;
                case Node::Tag::Call:
                    os << "  callee=" << n.data.call.callee
                       << "  argc=" << n.data.call.arg_count;
                    break;
                case Node::Tag::Block:
                    os << "  stmt_count=" << n.data.block.stmt_count;
                    break;
                case Node::Tag::IfSimple:
                    os << "  cond=" << n.data.if_simple.cond
                       << "  then=" << n.data.if_simple.then_body;
                    break;

                case Node::Tag::IfFull:
                    os << "  cond=" << n.data.if_full.cond
                       << "  then=" << n.data.if_full.then_body
                       << "  else=" << n.data.if_full.else_body;
                    break;
                case Node::Tag::While:
                    os << "  cond=" << n.data.while_stmt.cond
                       << "  body=" << n.data.while_stmt.body;
                    break;
                case Node::Tag::LetDecl:
                case Node::Tag::ConstDecl:
                    os << "  init=" << n.data.let_decl.init;
                    break;
                case Node::Tag::Return:
                    os << "  value=" << n.data.ret_value;
                    break;
                case Node::Tag::FnDecl:
                    os << "  params=" << n.data.fn_decl.param_count
                       << "  body=" << n.data.fn_decl.body;
                    break;
                case Node::Tag::Paren:
                case Node::Tag::ExprStmt:
                    os << "  inner=" << n.data.inner;
                    break;
                default:
                    break;
                }
                os << "\n";
            }
        }
    };

} // namespace cora::ast
#endif
