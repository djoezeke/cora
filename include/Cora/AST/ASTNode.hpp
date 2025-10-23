#ifndef CORA_AST_NODE_H
#define CORA_AST_NODE_H

#include <iostream>
#include <utility>

#include "Cora/Basic/SourceLocation.h"


namespace cora
{
    using namespace basic;

    namespace ast
    {

        enum class NodeKind
        {
            Module,
            Program,
            Statement,
            Expression,
        };

        enum class StatementKind
        {
            IfStmt,
            DoStmt,
            ForStmt,
            PassStmt,
            WhileStmt,
            BreakStmt,
            BraceStmt,
            YieldStmt,
            ThrowStmt,
            SwitchStmt,
            ReturnStmt,
            ForEachStmt,
            ContinueStmt,
        };

        enum class ExpressionKind
        {
            Null,
            Bool,
            Byte,
            Float,
            Array,
            String,
            Integer,

            Identifier,

            TryExpr,
            CallExpr,
            BlockExpr,
            AssignExpr,
            BinaryExpr,
            TernaryExpr,
            PrefixUnaryExpr,
            PostfixUnaryExpr,

        };

        class Node
        {
        public:
            Node() = default;
            virtual ~Node() = default;

            Node(NodeKind valuekind) { m_Kind = valuekind; };

            Node(NodeKind valuekind, std::string kinsdtring)
            {
                m_Kind = valuekind;
                m_KindString = kinsdtring;
            };

            Node(NodeKind nodekind, SourceRange sourcerange)
            {
                m_Kind = nodekind;
                m_Range = std::move(sourcerange);
            };

            Node(NodeKind nodekind, SourceLocation loc)
            {
                m_Kind = nodekind;
                m_Range = SourceRange(loc);
            };

            Node(NodeKind nodekind, SourceLocation start, SourceLocation end)
            {
                m_Kind = nodekind;
                m_Range = SourceRange(start, end);
            };

            /**
             * @brief Compute a representation of the node.
             * @return String representation of the node
             */
            virtual std::string Repr() = 0;

            /**
             * @brief Return the node kind
             *
             * @return NodeKind
             */
             NodeKind GetNodeKind() {return m_Kind;};

            /**
             * @brief Return the node kind string
             *
             * @return const std::string&
             */
             std::string GetNodeKindString() {return m_KindString;};

            /**
             * @brief Get the start position of the node
             *
             * @return Position
             */
            SourceLocation GetStartPosition() const noexcept { return m_Range.GetStart(); };

            /**
             * @brief Get the end position of the node
             *
             * @return Position
             */
            SourceLocation GetEndPosition() const noexcept { return m_Range.GetEnd(); };

            /**
             * @brief Get the source range of the node
             *
             * @return SourceRange
             */
            SourceRange GetSourceRange() const { return m_Range; };

            /**
             * @brief Set the Node Type object
             *
             * @param nodekind
             */
            void SetNodeKind(NodeKind nodekind) noexcept { m_Kind = nodekind; };

            /**
             * @brief Set the Node Type string
             *
             * @param kindstring
             */
            void SetNodeKindString(std::string kindstring) noexcept { m_KindString = kindstring; };

            /**
             * @brief Set the Node start position
             *
             * @param start
             */
            void SetStartPosition(SourceLocation start) noexcept { m_Range.SetStart(start); };

            /**
             * @brief Set the Node end position
             *
             * @param end
             */
            void SetEndPosition(SourceLocation end) noexcept { m_Range.SetEnd(end); };

            /**
             * @brief Set the Node source range.
             *
             * @param range
             */
            void SetSourceRange(SourceRange range) noexcept { m_Range = range; };

        private:
            NodeKind m_Kind;
            SourceRange m_Range;
            std::string m_KindString;
        };

        std::ostream &operator<<(std::ostream &ostream, const Node *node);

        class Statement;

        class Program : public Node
        {
        private:
        public:
            Program();
            ~Program() override = default;
        };

        class Module : public Node
        {
        private:
        public:
            Module();
            ~Module() override = default;
        };

    } // namespace ast

} // namespace cora

#endif // CORA_AST_NODE_H
