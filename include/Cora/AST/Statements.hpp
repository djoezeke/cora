#ifndef CORA_AST_STATEMENT_H
#define CORA_AST_STATEMENT_H

#include "Cora/AST/ASTNode.hpp"

namespace cora
{
    namespace ast
    {

        class Statement : public Node
        {
        public:
            Statement()
                : Node(NodeKind::Statement, "Statement") {};

            ~Statement() override = default;

            StatementKind GetStmtKind() const { return m_StmtKind; };

        private:
            StatementKind m_StmtKind;
        };

        /**
         * If AST node representation
         */
        class IfStmt : public Statement
        {
        public:
            explicit IfStmt() {};
            ~IfStmt() override = default;
        };

        /**
         * Do AST node representation
         */
        class DoStmt : public Statement
        {
        public:
            explicit DoStmt() {};
            ~DoStmt() override = default;
        };

        /**
         * For AST node representation
         */
        class ForStmt : public Statement
        {
        public:
            explicit ForStmt() {};
            ~ForStmt() override = default;
        };

        /**
         * New AST node representation
         */
        class NewStmt : public Statement
        {
        public:
            explicit NewStmt() {};
            ~NewStmt() override = default;
        };

        /**
         * Pass AST node representation
         */
        class PassStmt : public Statement
        {
        public:
            explicit PassStmt() {};
            ~PassStmt() override = default;
        };

        /**
         * While AST node representation
         */
        class WhileStmt : public Statement
        {
        public:
            explicit WhileStmt() {};
            ~WhileStmt() override = default;
        };

        /**
         * Break AST node representation
         */
        class BreakStmt : public Statement
        {
        public:
            explicit BreakStmt() {};
            ~BreakStmt() override = default;
        };

        /**
         * Brace {} AST node representation
         */
        class BraceStmt : public Statement
        {
        public:
            explicit BraceStmt() {};
            ~BraceStmt() override = default;
        };

        /**
         * Yield AST node representation
         */
        class YieldStmt : public Statement
        {
        public:
            explicit YieldStmt() {};
            ~YieldStmt() override = default;
        };

        /**
         * Throw AST node representation
         */
        class ThrowStmt : public Statement
        {
        public:
            explicit ThrowStmt() {};
            ~ThrowStmt() override = default;
        };

        /**
         * Delete AST node representation
         */
        class DeleteStmt : public Statement
        {
        public:
            explicit DeleteStmt() {};
            ~DeleteStmt() override = default;
        };

        /**
         * Switch AST node representation
         */
        class SwitchStmt : public Statement
        {
        public:
            explicit SwitchStmt() {};
            ~SwitchStmt() override = default;
        };

        /**
         * Return AST node representation
         */
        class ReturnStmt : public Statement
        {
        public:
            explicit ReturnStmt() {};
            ~ReturnStmt() override = default;
        };

        /**
         * ForEach AST node representation
         */
        class ForEachStmt : public Statement
        {
        public:
            explicit ForEachStmt() {};
            ~ForEachStmt() override = default;
        };

        /**
         * Do AST node representation
         */
        class DoStmt : public Statement
        {
        public:
            explicit DoStmt() {};
            ~DoStmt() override = default;
        };

        /**
         * Continue AST node representation
         */
        class ContinueStmt : public Statement
        {
        public:
            explicit ContinueStmt() {};
            ~ContinueStmt() override = default;
        };

    } // namespace ast

} // namespace cora

#endif // CORA_AST_STATEMENT_H
