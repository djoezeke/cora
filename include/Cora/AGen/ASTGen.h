#include <Cora/AST/AST.h>
#include <Cora/CIR/CIR.h>

namespace cora
{

    // ============================================================================
    // AstGen — ast::AST → cir::CIR
    // ============================================================================
    class AstGen final
    {
    public:
        explicit AstGen(ast::AST &ast) : ast_(ast) {}

        cir::CIR run()
        {
            // Root body holds top-level decls.
            cir::CIR::BIndex root_idx = newBody();
            cir::GenCir gz = rootGen(root_idx);

            // Walk the ast::AST's root block, emitting each top-level decl.
            const ast::AST::Node &root = ast_.nodes[ast_.nodes.size() ? 1 : 1];
            for (auto child : ast_.list(root.data.block_stmts))
            {
                if (!child)
                    continue;
                genDecl(gz, child);
            }
            return std::move(cir_);
        }

    private:
        // ------------------------------------------------------------------
        // Body / scope helpers
        // ------------------------------------------------------------------
        cir::CIR::BIndex newBody()
        {
            cir_.bodies.emplace_back();
            return static_cast<cir::CIR::BIndex>(cir_.bodies.size() - 1);
        }

        cir::GenCir rootGen(cir::CIR::BIndex idx)
        {
            cir::GenCir gz;
            gz.body = &cir_.bodies[idx];
            gz.first_inst = 0;
            return gz;
        }
        // Descend into a fresh GenCir for a sub-block (child scope).
        GenCir childGen(cir::GenCir &parent, cir::CIR::IIndex break_blk, uint32_t loop_depth, cir::ResLoc rl)
        {
            GenCir gz;
            gz.body = parent.body;
            gz.first_inst = static_cast<uint32_t>(parent.body->insts.size());
            gz.parent = &parent;
            gz.break_blk = break_blk;
            gz.continue_blk = parent.continue_blk;
            gz.loop_depth = loop_depth;
            gz.rl = rl;
            return gz;
        }

        // ------------------------------------------------------------------
        // Names / literals
        // ------------------------------------------------------------------
        uint32_t internName(ast::AST::TIndex tok)
        {
            return cir_.internString(ast_.tokenText(tok));
        }
        uint32_t internStr(AST::TIndex tok)
        {
            return cir_.internString(ast_.tokenText(tok));
        }

        // ------------------------------------------------------------------
        // Instruction emission shortcuts
        // ------------------------------------------------------------------
        cir::CIR::IIndex emitErrTok(cir::GenCir &gz, uint16_t err_tag)
        {
            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::ErrTok;
            i.data.err_tag = err_tag;
            return gz.emit(i);
        }
        cir::CIR::IIndex emitInt(cir::GenCir &gz, int64_t v)
        {
            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::Int;
            i.data.int_value = v;
            return gz.emit(i);
        }
        cir::CIR::IIndex emitRef(cir::GenCir &gz, uint32_t name)
        {
            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::Ref;
            i.data.name_idx = name;
            return gz.emit(i);
        }
        cir::CIR::IIndex emitLoad(cir::GenCir &gz, cir::CIR::IIndex src)
        {
            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::Load;
            i.data.unary_op.target = src;
            return gz.emit(i);
        }

        // Map ast::AST binary op token → ZIR inst tag.
        static cir::CIR::Tag binTag(const ast::AST::Node &n, const ast::AST &ast)
        {
            // In the real code, switch on token tag; simplified here.
            return cir::CIR::Tag::Add; // filled per-op below
        }

        // ------------------------------------------------------------------
        // Declarations
        // ------------------------------------------------------------------
        cir::CIR::IIndex genDecl(cir::GenCir &gz, ast::AST::NIndex n)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            switch (node.tag)
            {
            case ast::AST::Tag::FnDecl:
                return genFnDecl(gz, n);
            case ast::AST::Tag::LetDecl:
            case ast::AST::Tag::ConstDecl:
                return genLetDecl(gz, n);
            case ast::AST::Tag::Malformed:
                return emitErrTok(gz, node.data.malformed.err);
            default:
                return emitErrTok(gz, /*ExpectedDecl*/ 0);
            }
        }

        cir::CIR::IIndex genFnDecl(cir::GenCir &gz, ast::AST::NIndex n)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            uint32_t name = internName(node.token);

            // New body for the function.
            cir::CIR::BodyIndex fn_body = newBody();

            // Fresh GenCir for the function body; RL is Return.
            cir::GenCir fgz;
            fgz.body = &cir_.bodies[fn_body];
            fgz.first_inst = 0;
            fgz.parent = &gz;
            fgz.rl = cir::ResLoc::Return();

            // Walk the ast::AST block into the new body.
            if (node.data.fn_decl.body)
                genBlockBody(fgz, node.data.fn_decl.body);

            // Params: just names for now.
            std::vector<uint32_t> params;
            for (auto p : ast_.list(node.data.fn_decl.params))
                params.push_back(ast_.nodes[p].token);
            auto pextra = cir_.addList(*gz.body, params);

            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::FnDecl;
            i.data.fn_decl = {name, /*body=*/fn_body, pextra,
                              static_cast<uint32_t>(params.size())};
            return gz.emit(i);
        }

        cir::CIR::IIndex genLetDecl(cir::GenCir &gz, ast::AST::NIndex n)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            uint32_t name = internName(node.token);

            cir::CIR::IIndex init = node.data.let_init
                                        ? genExpr(gz, node.data.let_init, cir::ResLoc::Ref(/*slot*/ 0))
                                        : cir::CIR::INull;

            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::LetDecl;
            i.data.let_decl = {name, init};
            return gz.emit(i);
        }

        // ------------------------------------------------------------------
        // Statements
        // ------------------------------------------------------------------
        void genBlockBody(cir::GenCir &gz, ast::AST::NIndex block_ast)
        {
            // Emit a Block inst, then walk stmts inline; closeBlock patches len.
            cir::CIR::Inst b{};
            b.tag = cir::CIR::Tag::Block;
            cir::CIR::IIndex blk = gz.emit(b);

            cir::GenCir inner = childGen(gz, /*break_blk=*/blk, gz.loop_depth, gz.rl);
            const ast::AST::Node &node = ast_.nodes[block_ast];
            for (auto s : ast_.list(node.data.block_stmts))
            {
                if (!s)
                    continue;
                genStmt(inner, s);
            }
            gz.closeBlock(blk);

            // If it's a fn body: implicit `return` the block's value.
            if (gz.rl.tag == cir::ResLoc::Tag::Return && /*no explicit return*/ true)
            {
                cir::CIR::Inst r{};
                r.tag = cir::CIR::Tag::ReturnVoid;
                gz.emit(r);
            }
        }

        cir::CIR::IIndex genStmt(cir::GenCir &gz, ast::AST::NIndex n)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            switch (node.tag)
            {
            case ast::AST::Tag::Return:
                return genReturn(gz, n);
            case ast::AST::Tag::ExprStmt:
                return genExpr(gz, node.data.inner, cir::ResLoc::Discard());
            case ast::AST::Tag::Block:
                genBlockBody(gz, n);
                return cir::CIR::Null;
            case ast::AST::Tag::IfSimple:
                return genIf(gz, n);
            case ast::AST::Tag::IfFull:
                return genIf(gz, n);
            case ast::AST::Tag::While:
                return genWhile(gz, n);
            case ast::AST::Tag::Break:
            {
                cir::CIR::IIndex target = findBreakTarget(gz);
                cir::CIR::Inst i{};
                i.tag = cir::CIR::Tag::BreakVoid;
                i.data.brk.block = target;
                return gz.emit(i);
            }
            case ast::AST::Tag::Continue:
            {
                cir::CIR::IIndex target = findContinueTarget(gz);
                cir::CIR::Inst i{};
                i.tag = cir::CIR::Tag::BreakVoid;
                i.data.brk.block = target;
                return gz.emit(i);
            }
            case ast::AST::Tag::LetDecl:
            case ast::AST::Tag::ConstDecl:
                return genLetDecl(gz, n);
            case ast::AST::Tag::Malformed:
                return emitErrTok(gz, node.data.malformed.err);
            default:
                return genExpr(gz, n, cir::ResLoc::Discard());
            }
        }

        cir::CIR::IIndex genReturn(cir::GenCir &gz, ast::AST::NIndex n)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            if (!node.data.ret_value)
            {
                cir::CIR::Inst i{};
                i.tag = cir::CIR::Tag::ReturnVoid;
                return gz.emit(i);
            }
            cir::CIR::IIndex v = genExpr(gz, node.data.ret_value, cir::ResLoc::Return());
            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::Return;
            i.data.unary_op.target = v;
            return gz.emit(i);
        }

        cir::CIR::IIndex genIf(cir::GenCir &gz, ast::AST::NIndex n)
        {
        const ast::AST::Node& node = ast_.nodes[n;
        cir::CIR::IIndex cond = genExpr(gz, node.data.if_simple.cond, cir::ResLoc::None());

        // then-block
        cir::CIR::Inst tb{}; tb.tag = cir::CIR::Tag::Block;
        cir::CIR::IIndex then_blk = gz.emit(tb);
        GenCir tz = childGen(gz, then_blk, gz.loop_depth, gz.rl);
        genStmt(tz, node.data.if_simple.then_body);
        gz.closeBlock(then_blk);

        // else-block (or empty)
        cir::CIR::Inst eb{}; eb.tag = cir::CIR::Tag::Block;
        cir::CIR::IIndex else_blk = gz.emit(eb);
        cir::CIR::IIndex else_body = (node.tag == ast::AST::Tag::IfFull)
            ? ast_.list(node.data.if_full.branches)[1] : 0;
        if (else_body) {
                GenCir ez = childGen(gz, else_blk, gz.loop_depth, gz.rl);
                genStmt(ez, else_body);
        }
        gz.closeBlock(else_blk);

        cir::CIR::Inst cb{}; cb.tag = cir::CIR::Tag::CondBr;
        cb.data.condbr = { cond, then_blk, else_blk };
        return gz.emit(cb);
        }

        cir::CIR::IIndex genWhile(cir::GenCir &gz, ast::AST::NIndex n)
        {
            const ast::AST::Node &node = ast_.nodes[n];

            cir::CIR::Inst hb{};
            hb.tag = cir::CIR::Tag::Block;
            cir::CIR::IIndex head = gz.emit(hb);

            cir::CIR::IIndex cond = genExpr(gz, node.data.while_stmt.cond, cir::ResLoc::None());

            cir::CIR::Inst bb{};
            bb.tag = cir::CIR::Tag::Block;
            cir::CIR::IIndex body_blk = gz.emit(bb);
            GenCir bz = childGen(gz, /*break=*/head, /*loop=*/gz.loop_depth + 1, cir::ResLoc::None());
            bz.continue_blk = head;
            genStmt(bz, node.data.while_stmt.body);
            gz.closeBlock(body_blk);

            cir::CIR::Inst cb{};
            cb.tag = cir::CIR::Tag::CondBr;
            cb.data.condbr = {cond, body_blk, /*exit=*/0};
            gz.emit(cb);

            gz.closeBlock(head);
            return head;
        }

        // ------------------------------------------------------------------
        // Expressions — Pratt already flattened the shape; we just lower.
        // ------------------------------------------------------------------
        cir::CIR::IIndex genExpr(cir::GenCir &gz, ast::AST::NIndex n, cir::ResLoc rl)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            switch (node.tag)
            {
            case ast::AST::Tag::IntegerLit:
            {
                int64_t v = 0;
                auto txt = ast_.tokenText(node.token);
                for (char c : txt)
                    if (c >= '0' && c <= '9')
                        v = v * 10 + (c - '0');
                cir::CIR::IIndex i = emitInt(gz, v);
                return storeToRl(gz, rl, i);
            }
            case ast::AST::Tag::Identifier:
            {
                uint32_t name = internName(node.token);
                cir::CIR::IIndex ref = emitRef(gz, name);
                // RL::Ref — caller wants the ref; RL::None — caller wants a value.
                return rl.tag == cir::ResLoc::Tag::Ref ? ref : emitLoad(gz, ref);
            }
            case ast::AST::Tag::Binary:
                return genBinary(gz, n, rl);
            case ast::AST::Tag::Assign:
                return genAssign(gz, n);
            case ast::AST::Tag::Call:
                return genCall(gz, n, rl);
            case ast::AST::Tag::Paren:
                return genExpr(gz, node.data.inner, rl);
            case ast::AST::Tag::Malformed:
                return emitErrTok(gz, node.data.malformed.err);
            default:
                return emitErrTok(gz, /*ExpectedExpr*/ 0);
            }
        }

        cir::CIR::IIndex genBinary(cir::GenCir &gz, ast::AST::NIndex n, cir::ResLoc rl)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            cir::CIR::IIndex lhs = genExpr(gz, node.data.binary.lhs, cir::ResLoc::None());
            cir::CIR::IIndex rhs = genExpr(gz, node.data.binary.rhs, cir::ResLoc::None());

            cir::CIR::Tag tag = mapBinTag(node.token);
            cir::CIR::Inst i{};
            i.tag = tag;
            i.data.bin = {lhs, rhs};
            cir::CIR::IIndex out = gz.emit(i);
            return storeToRl(gz, rl, out);
        }

        cir::CIR::IIndex genAssign(cir::GenCir &gz, ast::AST::NIndex n)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            cir::CIR::IIndex lhs_ref = genExpr(gz, node.data.binary.lhs, cir::ResLoc::Ref(0));
            cir::CIR::IIndex rhs = genExpr(gz, node.data.binary.rhs, cir::ResLoc::None());
            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::Store;
            i.data.bin = {lhs_ref, rhs};
            return gz.emit(i);
        }

        cir::CIR::IIndex genCall(cir::GenCir &gz, ast::AST::NIndex n, cir::ResLoc rl)
        {
            const ast::AST::Node &node = ast_.nodes[n];
            cir::CIR::IIndex callee = genExpr(gz, node.data.call.callee, cir::ResLoc::None());
            std::vector<uint32_t> args;
            for (auto a : ast_.list(node.data.call.args))
                args.push_back(genExpr(gz, a, cir::ResLoc::None()));
            auto aext = cir_.addList(*gz.body, args);

            cir::CIR::Inst i{};
            i.tag = cir::CIR::Tag::Call;
            i.data.call = {callee, aext};
            cir::CIR::IIndex out = gz.emit(i);
            return storeToRl(gz, rl, out);
        }

        // ------------------------------------------------------------------
        // Result Location
        // ------------------------------------------------------------------
        // If `rl` names a slot, emit a Store and return the slot.
        // Otherwise just return the value inst.
        cir::CIR::IIndex storeToRl(cir::GenCir &gz, cir::ResLoc rl, cir::CIR::IIndex value)
        {
            if (rl.tag == cir::ResLoc::Tag::Ref && rl.ref)
            {
                cir::CIR::Inst s{};
                s.tag = cir::CIR::Tag::Store;
                s.data.bin = {rl.ref, value};
                gz.emit(s);
                return rl.ref;
            }
            return value;
        }

        // ------------------------------------------------------------------
        // Break/continue target lookup (scope chain)
        // ------------------------------------------------------------------
        cir::CIR::IIndex findBreakTarget(cir::GenCir &gz)
        {
            for (cir::GenCir *g = &gz; g; g = g->parent)
                if (g->break_blk)
                    return g->break_blk;
            return cir::CIR::INull;
        }
        cir::CIR::IIndex findContinueTarget(cir::GenCir &gz)
        {
            for (cir::GenCir *g = &gz; g; g = g->parent)
                if (g->loop_depth > 0 && g->continue_blk)
                    return g->continue_blk;
            return cir::CIR::INull;
        }

        cir::CIR::Tag mapBinTag(ast::AST::TIndex) { return cir::CIR::Tag::Add; } // real: switch on tag

        ast::AST &ast_;
        cir::CIR cir_;
    };
}
