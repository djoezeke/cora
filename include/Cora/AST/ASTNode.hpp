#ifndef CORA_AST_NODE_H
#define CORA_AST_NODE_H

#include <iostream>

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
            virtual ~Node();

            Node(NodeKind valuekind) { m_Kind = valuekind; };

            Node(NodeKind valuekind, std::string kinsdtring)
            {
                m_Kind = valuekind;
                m_KindString = kinsdtring;
            };

            Node(NodeKind nodekind, SourceRange sourcerange)
            {
                m_Kind = nodekind;
                m_Range = sourcerange;
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
            virtual NodeKind GetNodeKind() = 0;

            /**
             * @brief Return the node kind string
             *
             * @return const std::string&
             */
            virtual std::string GetNodeKindString() = 0;

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

        std::ostream &operator<<(std::ostream &ostream, const Node *value);

    } // namespace ast

} // namespace cora

#endif // CORA_AST_NODE_H
