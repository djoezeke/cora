#ifndef CORA_AST_AST_H
#define CORA_AST_AST_H

#include <Cora/AST/Node.h>
#include <Cora/Core/SourceLocation.h>
#include <Cora/Diag/DiagnosticCodes.h>
#include <Cora/Lexer/Token.h>
#include <Cora/Lexer/TokenPool.h>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cora::ast
{

    // -----------------------------------------------------------------------
    // NodeList — structure-of-arrays node storage
    //
    // This is Cora's equivalent of Zig's `Ast.NodeList = MultiArrayList(Node)`.
    // Tags, main tokens and payloads live in three independent contiguous
    // arrays. The parser can scan tags linearly without pulling 20-byte nodes
    // through the cache, and the AST stays compact.
    // -----------------------------------------------------------------------
    class NodeList final
    {
    public:
        using NIndex = Node::NIndex;
        using TIndex = Node::TIndex;
        using EIndex = Node::EIndex;
        using Tag = Node::Tag;
        using Data = Node::Data;

        [[nodiscard]] std::size_t size() const noexcept { return m_tags.size(); }
        [[nodiscard]] bool empty() const noexcept { return m_tags.empty(); }
        [[nodiscard]] std::size_t capacity() const noexcept
        {
            return m_tags.capacity();
        }
        void reserve(std::size_t capacity)
        {
            m_tags.reserve(capacity);
            m_main_tokens.reserve(capacity);
            m_data.reserve(capacity);
        }
        void clear() noexcept
        {
            m_tags.clear();
            m_main_tokens.clear();
            m_data.clear();
        }

        NIndex add(Tag tag, TIndex main_token, const Data &data)
        {
            const NIndex index = static_cast<NIndex>(m_tags.size());
            m_tags.push_back(tag);
            m_main_tokens.push_back(main_token);
            m_data.push_back(data);
            return index;
        }

        [[nodiscard]] Tag tag(NIndex index) const noexcept
        {
            return m_tags[index];
        }
        [[nodiscard]] TIndex mainToken(NIndex index) const noexcept
        {
            return m_main_tokens[index];
        }
        [[nodiscard]] const Data &data(NIndex index) const noexcept
        {
            return m_data[index];
        }
        void setData(NIndex index, const Data &data) noexcept
        {
            m_data[index] = data;
        }

        [[nodiscard]] Node operator[](NIndex index) const noexcept
        {
            Node node{};
            node.tag = m_tags[index];
            node.main_token = m_main_tokens[index];
            node.data = m_data[index];
            return node;
        }

        [[nodiscard]] const std::vector<Tag> &tagColumn() const noexcept
        {
            return m_tags;
        }
        [[nodiscard]] const std::vector<TIndex> &mainTokenColumn() const noexcept
        {
            return m_main_tokens;
        }
        [[nodiscard]] const std::vector<Data> &dataColumn() const noexcept
        {
            return m_data;
        }

    private:
        std::vector<Tag> m_tags;
        std::vector<TIndex> m_main_tokens;
        std::vector<Data> m_data;
    };

    // -----------------------------------------------------------------------
    // Parse diagnostics
    // -----------------------------------------------------------------------
    struct ParseError
    {
        diag::Severity severity = diag::Severity::Error;
        diag::DiagID id = diag::DiagID::err_expected_token;
        core::SourceSpan span{};
        std::string message;
    };

    // -----------------------------------------------------------------------
    // Top-level declaration range
    //
    // Declarations are parsed contiguously. Storing the node/extra/token/byte
    // ranges lets incremental parsing reuse an entire top-level declaration
    // without walking or cloning it blindly.
    // -----------------------------------------------------------------------
    struct DeclInfo
    {
        Node::NIndex node = Node::NNull;
        Node::NIndex node_begin = Node::NNull;
        Node::NIndex node_end = Node::NNull; // exclusive
        Node::EIndex extra_begin = Node::ENull;
        Node::EIndex extra_end = Node::ENull; // exclusive
        Node::TIndex first_token = Node::TNull;
        Node::TIndex last_token = Node::TNull;
        std::uint32_t start_offset = 0;
        std::uint32_t end_offset = 0;
    };

    /// Non-owning view of an extra_data list.
    struct IndexList
    {
        const Node::NIndex *items = nullptr;
        std::size_t count = 0;

        [[nodiscard]] bool empty() const noexcept { return count == 0; }
        [[nodiscard]] std::size_t size() const noexcept { return count; }
        [[nodiscard]] const Node::NIndex *begin() const noexcept { return items; }
        [[nodiscard]] const Node::NIndex *end() const noexcept
        {
            return items == nullptr ? nullptr : items + count;
        }
        [[nodiscard]] Node::NIndex operator[](std::size_t index) const noexcept
        {
            return items[index];
        }
    };

    // -----------------------------------------------------------------------
    // AST
    // -----------------------------------------------------------------------
    struct AST
    {
        using NIndex = Node::NIndex;
        using TIndex = Node::TIndex;
        using EIndex = Node::EIndex;
        using Tag = Node::Tag;
        using Node = ast::Node;

        static constexpr NIndex NNull = Node::NNull;
        static constexpr EIndex ENull = Node::ENull;
        static constexpr TIndex TNull = Node::TNull;

        AST();
        AST(const AST &) = default;
        AST(AST &&) noexcept = default;
        AST &operator=(const AST &) = default;
        AST &operator=(AST &&) noexcept = default;
        ~AST() = default;

        // -------- Storage ----------------------------------------------------
        NodeList nodes;
        std::vector<NIndex> extra_data{ENull};
        lexer::TokenPool tokens;
        std::string source;
        core::FileID file_id = core::INVALID_FILE_ID;

        NIndex root = NNull;
        std::vector<ParseError> errors;
        std::vector<DeclInfo> decls;

        // Side tables for O(1) source spans. They are kept parallel to the
        // node arrays and survive incremental subtree copies.
        std::vector<TIndex> node_first_token;
        std::vector<TIndex> node_last_token;

        void clear();
        void reserve(std::size_t node_capacity,
                     std::size_t extra_capacity = 0,
                     std::size_t token_capacity = 0);

        // -------- Node construction -----------------------------------------
        NIndex addNode(Tag tag, TIndex main_token = TNull);
        NIndex addNode(Tag tag, TIndex main_token, const Node::Data &data);
        NIndex addNode(Tag tag, TIndex main_token, const Node::Data &data,
                       TIndex first_token, TIndex last_token);

        void setNodeData(NIndex index, const Node::Data &data);
        void setNodeRange(NIndex index, TIndex first_token, TIndex last_token);

        [[nodiscard]] std::size_t nodeCount() const noexcept
        {
            return nodes.size();
        }
        [[nodiscard]] Tag tag(NIndex index) const noexcept
        {
            return nodes.tag(index);
        }
        [[nodiscard]] TIndex mainToken(NIndex index) const noexcept
        {
            return nodes.mainToken(index);
        }
        [[nodiscard]] const Node::Data &data(NIndex index) const noexcept
        {
            return nodes.data(index);
        }
        [[nodiscard]] Node node(NIndex index) const noexcept
        {
            return nodes[index];
        }
        [[nodiscard]] TIndex firstToken(NIndex index) const noexcept;
        [[nodiscard]] TIndex lastToken(NIndex index) const noexcept;

        // -------- Extra data -------------------------------------------------
        EIndex addExtra(const NIndex *items, std::size_t count);
        EIndex addExtra(const std::vector<NIndex> &items);
        [[nodiscard]] IndexList extra(EIndex at, std::uint32_t count) const;

        /// Child node of a Node::Tag::Param.
        [[nodiscard]] NIndex paramType(NIndex param) const noexcept;
        [[nodiscard]] NIndex paramDefault(NIndex param) const noexcept;

        /// Parameters and return type of a FnDecl / TypeFn.
        ///
        /// The return type is stored as the final entry of the parameter list;
        /// this keeps the node payload at 12 bytes (Zig's limit) while still
        /// supporting both a body and a return type.
        [[nodiscard]] IndexList parameters(NIndex fn) const noexcept;
        [[nodiscard]] NIndex returnType(NIndex fn) const noexcept;

        // -------- Tokens / source -------------------------------------------
        void copyTokens(const lexer::TokenPool &pool);
        [[nodiscard]] std::size_t contentTokenCount() const noexcept;
        [[nodiscard]] Token token(TIndex index) const;
        [[nodiscard]] std::string_view tokenText(TIndex index) const;
        [[nodiscard]] core::SourceSpan tokenSpan(TIndex index) const;
        [[nodiscard]] core::SourceSpan span(NIndex index) const;

        // -------- Incremental support ---------------------------------------
        /// Deep-copy `decl` from `src` into the end of this AST.
        ///
        /// Node references, extra-data references and token references are
        /// rebased by the current storage offsets plus the supplied deltas.
        /// The returned DeclInfo describes the copied range in this AST.
        DeclInfo appendDeclaration(const AST &src, const DeclInfo &decl,
                                   std::int64_t token_delta,
                                   std::int64_t byte_delta);

        void appendError(const ParseError &error) { errors.push_back(error); }

        // -------- Queries / debugging ---------------------------------------
        [[nodiscard]] bool hasErrors() const noexcept { return !errors.empty(); }
        [[nodiscard]] std::size_t errorCount() const noexcept
        {
            return errors.size();
        }
        void dump(std::ostream &out) const;
        [[nodiscard]] bool validate(std::string *error = nullptr) const;
    };

} // namespace cora::ast

#endif // CORA_AST_AST_H
