#ifndef CORA_AGEN_ASTGEN_H
#define CORA_AGEN_ASTGEN_H

#include <Cora/AST/AST.h>
#include <Cora/CIR/CIR.h>
#include <Cora/Core/SHA256.h>

#include <string_view>

namespace cora
{
    /// Low-allocation AST to CIR lowering. Declaration bodies are independent
    /// cache units, so an unchanged declaration can be copied from a previous
    /// CIR without walking its AST again.
    class AstGen final
    {
    public:
        explicit AstGen(const ast::AST &ast) : ast_(ast) {}

        [[nodiscard]] cir::CIR run(const cir::CIR *previous = nullptr)
        {
            cir::CIR result;
            result.token_count = ast_.contentTokenCount();
            result.source_digest = crypto::SHA256::hash(ast_.source);
            result.revision = previous == nullptr ? 1 : previous->revision + 1;
            const auto root = ast_.root;
            if (root == ast::Node::NNull)
                return result;

            const ast::Node root_node = ast_.node(root);
            for (const auto declaration : ast_.extra(root_node.data.extra_data.at,
                                                     root_node.data.extra_data.count))
            {
                if (declaration >= ast_.nodeCount())
                    continue;
                const auto key = declarationKey(declaration);
                result.declaration_keys.push_back(key);
                const auto old = findPrevious(previous, key);
                if (old != nullptr)
                {
                    result.bodies.push_back(*old);
                    result.bodies.back().index =
                        static_cast<cir::BIndex>(result.bodies.size() - 1);
                    continue;
                }
                emitDeclaration(result, declaration);
            }
            return result;
        }

    private:
        [[nodiscard]] core::Digest declarationKey(ast::Node::NIndex node) const
        {
            const auto span = ast_.span(node);
            if (span.end_offset <= span.start_offset ||
                span.end_offset > ast_.source.size())
                return crypto::SHA256::hash(std::string_view{});
            return crypto::SHA256::hash(std::string_view(
                ast_.source.data() + span.start_offset, span.length()));
        }

        [[nodiscard]] const cir::Body *findPrevious(const cir::CIR *previous,
                                                    const core::Digest &key) const
        {
            if (previous == nullptr)
                return nullptr;
            for (std::size_t index = 0; index < previous->declaration_keys.size();
                 ++index)
                if (previous->declaration_keys[index] == key &&
                    index < previous->bodies.size())
                    return &previous->bodies[index];
            return nullptr;
        }

        static cir::Tag tagFor(ast::Node::Tag tag)
        {
            switch (tag)
            {
            case ast::Node::Tag::FnDecl: return cir::Tag::FnDecl;
            case ast::Node::Tag::StructDecl: return cir::Tag::StructDecl;
            case ast::Node::Tag::LetDecl:
            case ast::Node::Tag::ConstDecl: return cir::Tag::LetDecl;
            case ast::Node::Tag::Block: return cir::Tag::Block;
            case ast::Node::Tag::ExprStmt: return cir::Tag::ExprStmt;
            case ast::Node::Tag::IfSimple:
            case ast::Node::Tag::IfFull: return cir::Tag::If;
            case ast::Node::Tag::While: return cir::Tag::While;
            case ast::Node::Tag::ForC: return cir::Tag::ForC;
            case ast::Node::Tag::ForRange: return cir::Tag::ForRange;
            case ast::Node::Tag::Return: return cir::Tag::Return;
            case ast::Node::Tag::Break: return cir::Tag::Break;
            case ast::Node::Tag::Continue: return cir::Tag::Continue;
            case ast::Node::Tag::Defer: return cir::Tag::Defer;
            case ast::Node::Tag::Identifier: return cir::Tag::Identifier;
            case ast::Node::Tag::IntLit: return cir::Tag::IntLit;
            case ast::Node::Tag::FloatLit: return cir::Tag::FloatLit;
            case ast::Node::Tag::StringLit: return cir::Tag::StringLit;
            case ast::Node::Tag::CharLit: return cir::Tag::CharLit;
            case ast::Node::Tag::BoolLit: return cir::Tag::BoolLit;
            case ast::Node::Tag::NullLit: return cir::Tag::NullLit;
            case ast::Node::Tag::Unary: return cir::Tag::Unary;
            case ast::Node::Tag::Binary: return cir::Tag::Binary;
            case ast::Node::Tag::Assign: return cir::Tag::Assign;
            case ast::Node::Tag::Call: return cir::Tag::Call;
            case ast::Node::Tag::FieldAccess: return cir::Tag::FieldAccess;
            case ast::Node::Tag::IndexExpr: return cir::Tag::Index;
            case ast::Node::Tag::ArrayInit: return cir::Tag::ArrayInit;
            case ast::Node::Tag::StructInit: return cir::Tag::StructInit;
            case ast::Node::Tag::Paren: return cir::Tag::Paren;
            default: return cir::Tag::Malformed;
            }
        }

        cir::IIndex emitNode(cir::CIR &result, cir::Body &body,
                             ast::Node::NIndex node)
        {
            if (node >= ast_.nodeCount())
                return cir::INull;
            const ast::Node value = ast_.node(node);
            cir::Inst instruction{};
            instruction.tag = tagFor(value.tag);
            instruction.span = ast_.span(node);
            instruction.op = ast_.token(value.main_token).tag;
            if (value.tag == ast::Node::Tag::IntLit ||
                value.tag == ast::Node::Tag::CharLit)
                instruction.int_value =
                    static_cast<std::int64_t>(ast_.token(value.main_token).int_val);
            if (value.tag == ast::Node::Tag::BoolLit)
                instruction.bool_value =
                    ast_.token(value.main_token).tag == Token::Tag::T_TRUE;
            if (value.tag == ast::Node::Tag::FloatLit)
                instruction.float_value = ast_.token(value.main_token).float_val;
            if (value.main_token != ast::Node::TNull)
                instruction.name = result.internString(ast_.tokenText(value.main_token));
            const auto emit_child = [this, &result, &body](ast::Node::NIndex child)
            {
                return child == ast::Node::NNull
                           ? cir::INull
                           : emitNode(result, body, child);
            };
            switch (value.tag)
            {
            case ast::Node::Tag::Binary:
            case ast::Node::Tag::Assign:
            case ast::Node::Tag::IndexExpr:
            case ast::Node::Tag::TypeArray:
                instruction.a = emit_child(value.data.node_and_node.lhs);
                instruction.b = emit_child(value.data.node_and_node.rhs);
                break;
            case ast::Node::Tag::Unary:
            case ast::Node::Tag::Paren:
            case ast::Node::Tag::ExprStmt:
            case ast::Node::Tag::Return:
            case ast::Node::Tag::Defer:
                instruction.a = emit_child(value.data.lhs);
                break;
            case ast::Node::Tag::IfSimple:
            case ast::Node::Tag::While:
                instruction.a = emit_child(value.data.cond_then.cond);
                instruction.b = emit_child(value.data.cond_then.then_body);
                break;
            case ast::Node::Tag::IfFull:
                instruction.a = emit_child(value.data.cond_then_else.cond);
                instruction.b = emit_child(value.data.cond_then_else.then_body);
                instruction.c = emit_child(value.data.cond_then_else.else_body);
                break;
            case ast::Node::Tag::ForRange:
                instruction.a = emit_child(value.data.for_range.binding);
                instruction.b = emit_child(value.data.for_range.iterable);
                instruction.c = emit_child(value.data.for_range.body);
                break;
            case ast::Node::Tag::ForC:
            {
                std::vector<std::uint32_t> parts;
                for (const auto part :
                     ast_.extra(value.data.for_c.parts, value.data.for_c.part_count))
                    parts.push_back(emit_child(part));
                instruction.extra = body.addList(parts);
                instruction.extra_count = static_cast<std::uint32_t>(parts.size());
                instruction.a = emit_child(value.data.for_c.body);
                break;
            }
            case ast::Node::Tag::LetDecl:
            case ast::Node::Tag::ConstDecl:
            case ast::Node::Tag::FieldDecl:
                instruction.type_ref = emitType(result, value.data.let_decl.type);
                instruction.a = emit_child(value.data.let_decl.type);
                instruction.b = emit_child(value.data.let_decl.init);
                break;
            case ast::Node::Tag::FieldAccess:
                instruction.a = emit_child(value.data.field_access.base);
                instruction.name =
                    result.internString(ast_.tokenText(value.data.field_access.field));
                break;
            case ast::Node::Tag::Call:
                instruction.a = emit_child(value.data.call.callee);
            {
                std::vector<std::uint32_t> args;
                for (const auto argument :
                     ast_.extra(value.data.call.args, value.data.call.arg_count))
                    args.push_back(emit_child(argument));
                instruction.extra = body.addList(args);
                instruction.extra_count = static_cast<std::uint32_t>(args.size());
                break;
            }
            case ast::Node::Tag::StructInit:
            {
                if (value.data.struct_init.type < ast_.nodeCount())
                {
                    const ast::Node type = ast_.node(value.data.struct_init.type);
                    if (type.main_token != ast::Node::TNull)
                        instruction.name =
                            result.internString(ast_.tokenText(type.main_token));
                }
                std::vector<std::uint32_t> fields;
                const auto items = ast_.extra(value.data.struct_init.fields,
                                              value.data.struct_init.field_count * 2);
                for (std::size_t item_index = 0; item_index < items.size();
                     ++item_index)
                {
                    const auto item = items[item_index];
                    if ((item_index & 1u) == 0)
                    {
                        fields.push_back(result.internString(
                            ast_.tokenText(static_cast<ast::Node::TIndex>(item))));
                        continue;
                    }
                    if (item >= ast_.nodeCount())
                    {
                        fields.push_back(cir::INull);
                        continue;
                    }
                    fields.push_back(emit_child(item));
                }
                instruction.extra = body.addList(fields);
                instruction.extra_count = static_cast<std::uint32_t>(fields.size());
                break;
            }
            case ast::Node::Tag::FnDecl:
                instruction.a = emit_child(value.data.fn_decl.body);
                for (const auto parameter :
                     ast_.parameters(node))
                {
                    if (parameter == ast::Node::NNull)
                        continue;
                    const ast::Node parameter_node = ast_.node(parameter);
                    cir::Param param;
                    param.name = result.internString(ast_.tokenText(parameter_node.main_token));
                    param.type_ref = emitType(result, ast_.paramType(parameter));
                    param.span = ast_.span(parameter);
                    body.params.push_back(param);
                }
                body.return_type = emitType(result, ast_.returnType(node));
                break;
            case ast::Node::Tag::Block:
            case ast::Node::Tag::StructDecl:
            case ast::Node::Tag::ArrayInit:
            {
                std::vector<std::uint32_t> children;
                for (const auto child :
                     ast_.extra(value.data.extra_data.at, value.data.extra_data.count))
                    children.push_back(emit_child(child));
                instruction.extra = body.addList(children);
                instruction.extra_count = static_cast<std::uint32_t>(children.size());
                break;
            }
            default:
                break;
            }
            const auto emitted = body.add(instruction);
            return emitted;
        }

        cir::TIndex emitType(cir::CIR &result, ast::Node::NIndex node)
        {
            if (node >= ast_.nodeCount())
                return cir::TNull;
            const ast::Node value = ast_.node(node);
            cir::TypeRefInfo type;
            type.span = ast_.span(node);
            switch (value.tag)
            {
            case ast::Node::Tag::TypeName:
                type.kind = cir::TypeRefKind::Named;
                type.name = result.internString(ast_.tokenText(value.main_token));
                break;
            case ast::Node::Tag::TypePointer:
                type.kind = cir::TypeRefKind::Pointer;
                type.element = emitType(result, value.data.lhs);
                break;
            case ast::Node::Tag::TypeArray:
                type.kind = cir::TypeRefKind::Array;
                type.element = emitType(result, value.data.node_and_node.lhs);
                if (value.data.node_and_node.rhs != ast::Node::NNull)
                {
                    const ast::Node length = ast_.node(value.data.node_and_node.rhs);
                    if (length.tag == ast::Node::Tag::IntLit)
                        type.length = ast_.token(length.main_token).int_val;
                }
                break;
            case ast::Node::Tag::TypeFn:
                type.kind = cir::TypeRefKind::Function;
                for (const auto param : ast_.parameters(node))
                    type.params.push_back(emitType(result, param));
                type.result = emitType(result, ast_.returnType(node));
                break;
            default:
                type.kind = cir::TypeRefKind::Named;
                type.name = result.internString("void");
                break;
            }
            return result.addType(std::move(type));
        }

        void emitDeclaration(cir::CIR &result, ast::Node::NIndex node)
        {
            const ast::Node value = ast_.node(node);
            const std::string name =
                value.main_token == ast::Node::TNull
                    ? std::string{}
                    : std::string(ast_.tokenText(value.main_token));
            const auto body_index = result.addBody(name);
            cir::Body &body = result.bodies[body_index];
            body.is_function = value.tag == ast::Node::Tag::FnDecl;
            body.is_extern = false;
            body.is_type_alias = value.tag == ast::Node::Tag::TypeDecl;
            if (body.is_type_alias && value.data.lhs < ast_.nodeCount())
                body.alias_type = emitType(result, value.data.lhs);
            if (value.tag == ast::Node::Tag::StructDecl)
            {
                for (const auto field_node :
                     ast_.extra(value.data.extra_data.at, value.data.extra_data.count))
                {
                    const ast::Node field = ast_.node(field_node);
                    if (field.tag != ast::Node::Tag::FieldDecl)
                        continue;
                    cir::Field field_info;
                    field_info.name = result.internString(ast_.tokenText(field.main_token));
                    field_info.type_ref = emitType(result, field.data.let_decl.type);
                    field_info.span = ast_.span(field_node);
                    body.fields.push_back(field_info);
                }
            }
            body.roots.push_back(emitNode(result, body, node));
        }

        const ast::AST &ast_;
    };
} // namespace cora

#endif // CORA_AGEN_ASTGEN_H
