#include <Cora/AST/AST.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string_view>
#include <utility>
#include <vector>

namespace cora::ast
{
    namespace
    {
        [[nodiscard]] Node::NIndex shiftNode(Node::NIndex value,
                                             std::int64_t delta) noexcept
        {
            if (value == Node::NNull)
            {
                return value;
            }
            return static_cast<Node::NIndex>(
                static_cast<std::int64_t>(value) + delta);
        }

        [[nodiscard]] Node::EIndex shiftExtra(Node::EIndex value,
                                              std::int64_t delta) noexcept
        {
            if (value == Node::ENull)
            {
                return value;
            }
            return static_cast<Node::EIndex>(
                static_cast<std::int64_t>(value) + delta);
        }

        [[nodiscard]] Node::TIndex shiftToken(Node::TIndex value,
                                              std::int64_t delta) noexcept
        {
            if (value == Node::TNull)
            {
                return value;
            }
            return static_cast<Node::TIndex>(
                static_cast<std::int64_t>(value) + delta);
        }

        enum class ListKind
        {
            Nodes,
            FieldPairs,
        };

        struct ListPatch
        {
            Node::EIndex at = Node::ENull;
            std::uint32_t count = 0;
            ListKind kind = ListKind::Nodes;
        };

        void patchNodeData(Node::Tag tag, Node::Data &data,
                           std::int64_t node_delta, std::int64_t extra_delta,
                           std::vector<ListPatch> &lists)
        {
            const auto node = [node_delta](Node::NIndex value)
            {
                return shiftNode(value, node_delta);
            };
            const auto extra = [extra_delta](Node::EIndex value)
            {
                return shiftExtra(value, extra_delta);
            };
            const auto addList = [&lists](Node::EIndex at, std::uint32_t count,
                                          ListKind kind)
            {
                if (count != 0 && at != Node::ENull)
                {
                    lists.push_back(ListPatch{at, count, kind});
                }
            };

            switch (tag)
            {
            case Node::Tag::Root:
            case Node::Tag::Block:
            case Node::Tag::StructDecl:
            case Node::Tag::ArrayInit:
                data.extra_data.at = extra(data.extra_data.at);
                addList(data.extra_data.at, data.extra_data.count, ListKind::Nodes);
                break;

            case Node::Tag::FnDecl:
            case Node::Tag::TypeFn:
                data.fn_decl.params = extra(data.fn_decl.params);
                // The list contains the parameters followed by the return
                // type, so patch one additional entry.
                addList(data.fn_decl.params, data.fn_decl.param_count + 1,
                        ListKind::Nodes);
                data.fn_decl.body = node(data.fn_decl.body);
                break;

            case Node::Tag::Call:
                data.call.callee = node(data.call.callee);
                data.call.args = extra(data.call.args);
                addList(data.call.args, data.call.arg_count, ListKind::Nodes);
                break;

            case Node::Tag::StructInit:
                data.struct_init.type = node(data.struct_init.type);
                data.struct_init.fields = extra(data.struct_init.fields);
                // Each initializer field is a (name-token, value-node) pair.
                addList(data.struct_init.fields,
                        data.struct_init.field_count * 2, ListKind::FieldPairs);
                break;

            case Node::Tag::ForC:
                data.for_c.parts = extra(data.for_c.parts);
                addList(data.for_c.parts, data.for_c.part_count, ListKind::Nodes);
                data.for_c.body = node(data.for_c.body);
                break;

            case Node::Tag::ForRange:
                data.for_range.binding = node(data.for_range.binding);
                data.for_range.iterable = node(data.for_range.iterable);
                data.for_range.body = node(data.for_range.body);
                break;

            case Node::Tag::IfSimple:
            case Node::Tag::While:
                data.cond_then.cond = node(data.cond_then.cond);
                data.cond_then.then_body = node(data.cond_then.then_body);
                break;

            case Node::Tag::IfFull:
                data.cond_then_else.cond = node(data.cond_then_else.cond);
                data.cond_then_else.then_body = node(data.cond_then_else.then_body);
                data.cond_then_else.else_body = node(data.cond_then_else.else_body);
                break;

            case Node::Tag::Binary:
            case Node::Tag::Assign:
            case Node::Tag::IndexExpr:
            case Node::Tag::TypeArray:
                data.node_and_node.lhs = node(data.node_and_node.lhs);
                data.node_and_node.rhs = node(data.node_and_node.rhs);
                break;

            case Node::Tag::LetDecl:
            case Node::Tag::ConstDecl:
            case Node::Tag::FieldDecl:
                data.let_decl.type = node(data.let_decl.type);
                data.let_decl.init = node(data.let_decl.init);
                break;

            case Node::Tag::Param:
                data.param.type = node(data.param.type);
                data.param.default_value = node(data.param.default_value);
                break;

            case Node::Tag::FieldAccess:
                data.field_access.base = node(data.field_access.base);
                break;

            case Node::Tag::Return:
            case Node::Tag::ExprStmt:
            case Node::Tag::Paren:
            case Node::Tag::Unary:
            case Node::Tag::Defer:
            case Node::Tag::TypePointer:
            case Node::Tag::TypeDecl:
                data.lhs = node(data.lhs);
                break;

            case Node::Tag::ImportDecl:
            case Node::Tag::Break:
            case Node::Tag::Continue:
            case Node::Tag::Identifier:
            case Node::Tag::IntLit:
            case Node::Tag::FloatLit:
            case Node::Tag::StringLit:
            case Node::Tag::CharLit:
            case Node::Tag::BoolLit:
            case Node::Tag::NullLit:
            case Node::Tag::TypeName:
            case Node::Tag::Malformed:
            case Node::Tag::Count:
                break;
            }
        }

        const char *listKindName(ListKind kind) noexcept
        {
            return kind == ListKind::FieldPairs ? "field-pairs" : "nodes";
        }
    } // namespace

    AST::AST()
    {
        extra_data.clear();
        extra_data.push_back(Node::ENull);
    }

    void AST::clear()
    {
        nodes.clear();
        extra_data.clear();
        extra_data.push_back(Node::ENull);
        tokens.clear();
        source.clear();
        file_id = core::INVALID_FILE_ID;
        root = Node::NNull;
        errors.clear();
        decls.clear();
        node_first_token.clear();
        node_last_token.clear();
    }

    void AST::reserve(std::size_t node_capacity, std::size_t extra_capacity,
                      std::size_t token_capacity)
    {
        nodes.reserve(node_capacity);
        extra_data.reserve(extra_capacity + 1);
        tokens.reserve(token_capacity);
        node_first_token.reserve(node_capacity);
        node_last_token.reserve(node_capacity);
        errors.reserve(16);
        decls.reserve(16);
    }

    AST::NIndex AST::addNode(Tag tag, TIndex main_token)
    {
        Node::Data data{};
        return addNode(tag, main_token, data, main_token, main_token);
    }

    AST::NIndex AST::addNode(Tag tag, TIndex main_token, const Node::Data &data)
    {
        return addNode(tag, main_token, data, main_token, main_token);
    }

    AST::NIndex AST::addNode(Tag tag, TIndex main_token, const Node::Data &data,
                             TIndex first_token, TIndex last_token)
    {
        const NIndex index = nodes.add(tag, main_token, data);
        node_first_token.push_back(first_token);
        node_last_token.push_back(last_token);
        return index;
    }

    void AST::setNodeData(NIndex index, const Node::Data &data)
    {
        if (index < nodes.size())
        {
            nodes.setData(index, data);
        }
    }

    void AST::setNodeRange(NIndex index, TIndex first_token, TIndex last_token)
    {
        if (index < node_first_token.size())
        {
            node_first_token[index] = first_token;
            node_last_token[index] = last_token;
        }
    }

    AST::TIndex AST::firstToken(NIndex index) const noexcept
    {
        if (index >= node_first_token.size())
        {
            return Node::TNull;
        }
        return node_first_token[index];
    }

    AST::TIndex AST::lastToken(NIndex index) const noexcept
    {
        if (index >= node_last_token.size())
        {
            return Node::TNull;
        }
        return node_last_token[index];
    }

    AST::EIndex AST::addExtra(const NIndex *items, std::size_t count)
    {
        if (items == nullptr || count == 0)
        {
            return Node::ENull;
        }
        const EIndex at = static_cast<EIndex>(extra_data.size());
        extra_data.insert(extra_data.end(), items, items + count);
        return at;
    }

    AST::EIndex AST::addExtra(const std::vector<NIndex> &items)
    {
        return addExtra(items.data(), items.size());
    }

    IndexList AST::extra(EIndex at, std::uint32_t count) const
    {
        if (at == Node::ENull || count == 0 || at >= extra_data.size())
        {
            return {};
        }
        const auto available = extra_data.size() - static_cast<std::size_t>(at);
        const auto actual = std::min<std::size_t>(count, available);
        return IndexList{extra_data.data() + at, actual};
    }

    AST::NIndex AST::paramType(NIndex param) const noexcept
    {
        if (param >= nodes.size())
        {
            return NNull;
        }
        return nodes.data(param).param.type;
    }

    AST::NIndex AST::paramDefault(NIndex param) const noexcept
    {
        if (param >= nodes.size())
        {
            return NNull;
        }
        return nodes.data(param).param.default_value;
    }

    IndexList AST::parameters(NIndex fn) const noexcept
    {
        if (fn >= nodes.size())
        {
            return {};
        }
        const auto &data = nodes.data(fn);
        return extra(data.fn_decl.params, data.fn_decl.param_count);
    }

    AST::NIndex AST::returnType(NIndex fn) const noexcept
    {
        if (fn >= nodes.size())
        {
            return NNull;
        }
        const auto &data = nodes.data(fn);
        if (data.fn_decl.params == Node::ENull)
        {
            return NNull;
        }
        const IndexList list =
            extra(data.fn_decl.params, data.fn_decl.param_count + 1);
        if (list.size() <= data.fn_decl.param_count)
        {
            return NNull;
        }
        return list[data.fn_decl.param_count];
    }

    void AST::copyTokens(const lexer::TokenPool &pool)
    {
        tokens.clear();
        tokens.reserve(pool.size());
        for (std::size_t index = 0; index < pool.size(); ++index)
        {
            tokens.allocate(pool.tag(index), pool.start(index), pool.end(index),
                            pool.flag(index), pool.value(index));
        }
    }

    std::size_t AST::contentTokenCount() const noexcept
    {
        if (tokens.empty())
        {
            return 0;
        }
        const auto last = tokens.size() - 1;
        return tokens.tag(last) == Token::Tag::T_EOF ? last : tokens.size();
    }

    Token AST::token(TIndex index) const
    {
        if (index == Node::TNull || index >= tokens.size())
        {
            Token eof{};
            eof.tag = Token::Tag::T_EOF;
            eof.loc.start = source.size();
            eof.loc.end = source.size();
            return eof;
        }
        return tokens.token(index);
    }

    std::string_view AST::tokenText(TIndex index) const
    {
        if (index == Node::TNull || index >= tokens.size())
        {
            return {};
        }
        const auto start = static_cast<std::size_t>(tokens.start(index));
        const auto finish = static_cast<std::size_t>(tokens.end(index));
        if (start > source.size() || finish > source.size() || finish < start)
        {
            return {};
        }
        return std::string_view(source).substr(start, finish - start);
    }

    core::SourceSpan AST::tokenSpan(TIndex index) const
    {
        const Token current = token(index);
        return core::SourceSpan{file_id,
                                static_cast<std::uint32_t>(current.loc.start),
                                static_cast<std::uint32_t>(current.loc.end)};
    }

    core::SourceSpan AST::span(NIndex index) const
    {
        const TIndex first = firstToken(index);
        if (first == Node::TNull)
        {
            return core::SourceSpan{file_id, 0, 0};
        }
        const TIndex last = lastToken(index);
        const Token first_token = token(first);
        const Token last_token = last == Node::TNull ? first_token : token(last);
        return core::SourceSpan{file_id,
                                static_cast<std::uint32_t>(first_token.loc.start),
                                static_cast<std::uint32_t>(last_token.loc.end)};
    }

    DeclInfo AST::appendDeclaration(const AST &src, const DeclInfo &decl,
                                    std::int64_t token_delta,
                                    std::int64_t byte_delta)
    {
        DeclInfo result;
        if (decl.node_begin >= decl.node_end ||
            decl.node_begin >= src.nodes.size())
        {
            return result;
        }

        const std::size_t node_begin = decl.node_begin;
        const std::size_t node_end =
            std::min<std::size_t>(decl.node_end, src.nodes.size());
        const std::size_t extra_begin =
            std::min<std::size_t>(decl.extra_begin, src.extra_data.size());
        const std::size_t extra_end =
            std::min<std::size_t>(decl.extra_end, src.extra_data.size());

        const NIndex node_base = static_cast<NIndex>(nodes.size());
        const EIndex extra_base = static_cast<EIndex>(extra_data.size());
        const std::int64_t node_delta =
            static_cast<std::int64_t>(node_base) -
            static_cast<std::int64_t>(decl.node_begin);
        const std::int64_t extra_delta =
            static_cast<std::int64_t>(extra_base) -
            static_cast<std::int64_t>(decl.extra_begin);

        std::vector<ListPatch> lists;
        lists.reserve(node_end - node_begin);

        for (std::size_t index = node_begin; index < node_end; ++index)
        {
            Node::Tag tag = src.nodes.tag(static_cast<NIndex>(index));
            Node::TIndex main_token =
                shiftToken(src.nodes.mainToken(static_cast<NIndex>(index)),
                           token_delta);
            Node::Data data = src.nodes.data(static_cast<NIndex>(index));
            patchNodeData(tag, data, node_delta, extra_delta, lists);

            // FieldAccess carries a token in its payload as well.
            if (tag == Node::Tag::FieldAccess)
            {
                data.field_access.field =
                    shiftToken(data.field_access.field, token_delta);
            }

            (void)nodes.add(tag, main_token, data);
            node_first_token.push_back(shiftToken(src.node_first_token[index],
                                                  token_delta));
            node_last_token.push_back(shiftToken(src.node_last_token[index],
                                                 token_delta));
        }

        if (extra_begin < extra_end)
        {
            extra_data.insert(extra_data.end(),
                              src.extra_data.begin() +
                                  static_cast<std::ptrdiff_t>(extra_begin),
                              src.extra_data.begin() +
                                  static_cast<std::ptrdiff_t>(extra_end));
        }

        for (const ListPatch &patch : lists)
        {
            for (std::uint32_t index = 0; index < patch.count; ++index)
            {
                const std::size_t position =
                    static_cast<std::size_t>(patch.at) + index;
                if (position >= extra_data.size())
                {
                    break;
                }
                if (patch.kind == ListKind::FieldPairs)
                {
                    if ((index % 2) == 0)
                    {
                        extra_data[position] = shiftToken(
                            static_cast<Node::TIndex>(extra_data[position]),
                            token_delta);
                    }
                    else
                    {
                        extra_data[position] =
                            shiftNode(extra_data[position], node_delta);
                    }
                }
                else
                {
                    extra_data[position] =
                        shiftNode(extra_data[position], node_delta);
                }
            }
        }

        result.node = shiftNode(decl.node, node_delta);
        result.node_begin = node_base;
        result.node_end = static_cast<NIndex>(nodes.size());
        result.extra_begin = extra_base;
        result.extra_end = static_cast<EIndex>(extra_data.size());
        result.first_token = shiftToken(decl.first_token, token_delta);
        result.last_token = shiftToken(decl.last_token, token_delta);
        result.start_offset = static_cast<std::uint32_t>(
            static_cast<std::int64_t>(decl.start_offset) + byte_delta);
        result.end_offset = static_cast<std::uint32_t>(
            static_cast<std::int64_t>(decl.end_offset) + byte_delta);
        return result;
    }

    void AST::dump(std::ostream &out) const
    {
        out << "AST file=" << file_id << " source=" << source.size()
            << " nodes=" << nodes.size() << " extras=" << extra_data.size()
            << " tokens=" << tokens.size() << " decls=" << decls.size()
            << " errors=" << errors.size() << '\n';

        for (std::size_t index = 0; index < nodes.size(); ++index)
        {
            const Node current = nodes[static_cast<NIndex>(index)];
            out << '[' << index << "] " << tagName(current.tag)
                << " main=" << current.main_token << " '"
                << tokenText(current.main_token) << "' [" << firstToken(
                       static_cast<NIndex>(index))
                << ',' << lastToken(static_cast<NIndex>(index)) << ']';

            switch (current.tag)
            {
            case Node::Tag::Root:
            case Node::Tag::Block:
            case Node::Tag::StructDecl:
            case Node::Tag::ArrayInit:
                out << " list=" << current.data.extra_data.at
                    << " count=" << current.data.extra_data.count;
                break;
            case Node::Tag::FnDecl:
            case Node::Tag::TypeFn:
                out << " params=" << current.data.fn_decl.params
                    << " count=" << current.data.fn_decl.param_count
                    << " body=" << current.data.fn_decl.body;
                break;
            case Node::Tag::Call:
                out << " callee=" << current.data.call.callee
                    << " args=" << current.data.call.args
                    << " count=" << current.data.call.arg_count;
                break;
            case Node::Tag::StructInit:
                out << " type=" << current.data.struct_init.type
                    << " fields=" << current.data.struct_init.fields
                    << " count=" << current.data.struct_init.field_count;
                break;
            case Node::Tag::ForC:
                out << " parts=" << current.data.for_c.parts
                    << " count=" << current.data.for_c.part_count
                    << " body=" << current.data.for_c.body;
                break;
            case Node::Tag::ForRange:
                out << " binding=" << current.data.for_range.binding
                    << " iterable=" << current.data.for_range.iterable
                    << " body=" << current.data.for_range.body;
                break;
            case Node::Tag::IfSimple:
            case Node::Tag::While:
                out << " cond=" << current.data.cond_then.cond
                    << " body=" << current.data.cond_then.then_body;
                break;
            case Node::Tag::IfFull:
                out << " cond=" << current.data.cond_then_else.cond
                    << " then=" << current.data.cond_then_else.then_body
                    << " else=" << current.data.cond_then_else.else_body;
                break;
            case Node::Tag::Binary:
            case Node::Tag::Assign:
            case Node::Tag::IndexExpr:
            case Node::Tag::TypeArray:
                out << " lhs=" << current.data.node_and_node.lhs
                    << " rhs=" << current.data.node_and_node.rhs;
                break;
            case Node::Tag::LetDecl:
            case Node::Tag::ConstDecl:
            case Node::Tag::FieldDecl:
                out << " type=" << current.data.let_decl.type
                    << " init=" << current.data.let_decl.init;
                break;
            case Node::Tag::Param:
                out << " type=" << current.data.param.type
                    << " default=" << current.data.param.default_value;
                break;
            case Node::Tag::FieldAccess:
                out << " base=" << current.data.field_access.base
                    << " field=" << current.data.field_access.field;
                break;
            case Node::Tag::Return:
            case Node::Tag::ExprStmt:
            case Node::Tag::Paren:
            case Node::Tag::Unary:
            case Node::Tag::Defer:
            case Node::Tag::TypePointer:
            case Node::Tag::TypeDecl:
                out << " lhs=" << current.data.lhs;
                break;
            case Node::Tag::ImportDecl:
            case Node::Tag::Break:
            case Node::Tag::Continue:
            case Node::Tag::Identifier:
            case Node::Tag::IntLit:
            case Node::Tag::FloatLit:
            case Node::Tag::StringLit:
            case Node::Tag::CharLit:
            case Node::Tag::BoolLit:
            case Node::Tag::NullLit:
            case Node::Tag::TypeName:
            case Node::Tag::Malformed:
            case Node::Tag::Count:
                break;
            }
            out << '\n';
        }

        for (std::size_t index = 0; index < decls.size(); ++index)
        {
            const DeclInfo &decl = decls[index];
            out << "decl[" << index << "] node=" << decl.node
                << " nodes=[" << decl.node_begin << ',' << decl.node_end
                << ") extras=[" << decl.extra_begin << ',' << decl.extra_end
                << ") tokens=[" << decl.first_token << ',' << decl.last_token
                << "] bytes=[" << decl.start_offset << ',' << decl.end_offset
                << ")\n";
        }

        for (const ParseError &error : errors)
        {
            out << "error " << diag_info(error.id).code << " ["
                << error.span.start_offset << ',' << error.span.end_offset
                << "): " << error.message << '\n';
        }

        // Keep this helper referenced so a future dump option can annotate
        // lists without changing the output format.
        (void)listKindName;
    }

} // namespace cora::ast
