#ifndef CORA_AST_EXPRESSION_H
#define CORA_AST_EXPRESSION_H

#include "Cora/AST/ASTNode.hpp"
#include <vector>

namespace cora
{
    namespace ast
    {

        using StatementList = std::vector<class Statement *>;
        using ExpressionList = std::vector<class Expression *>;

        /**
         * Expression AST node representation
         */
        class Expression : public Node
        {
        public:
            ~Expression() override = default;
        };

        /**
         * Null AST node representation
         */
        class Null : public Expression
        {
        public:
            explicit Null() {};
            ~Null() override = default;
        };

        /**
         * Boolean AST node representation
         */
        class Bool : public Expression
        {
        public:
            explicit Bool(bool const value) : m_Value(value) {}

            ~Bool() override = default;

            /**
             * @brief Return the node value
             *
             * @return bool
             */
            bool GetValue() const { return m_Value; }

        private:
            bool m_Value{false};
        };

        /**
         * Byte AST node representation
         */
        class Byte : public Expression
        {
        public:
            explicit Byte(char const value) : m_Value(value) {}

            ~Byte() override = default;

            /**
             * @brief Return the node value
             *
             * @return char
             */
            char GetValue() const { return m_Value; }

        private:
            char m_Value{0};
        };

        /**
         * Float AST node representation
         */
        class Float : public Expression
        {
        public:
            explicit Float(double value) : m_Value(value) {}

            ~Float() override = default;

            /**
             * @brief Return the node value
             *
             * @return double
             */
            double GetValue() const { return m_Value; }

        private:
            double m_Value{0.0};
        };

        /**
         * Array AST node representation
         */
        class Array : public Expression
        {
        public:
            explicit Array(double value) : m_Value(value) {}

            ~Array() override = default;

        private:
            double m_Value{0.0};
        };

        /**
         * String AST node representation
         */
        class String : public Expression
        {
        public:
            explicit String(std::string value) : m_Value(value) {}

            ~String() override = default;

            /**
             * @brief Return the node value
             *
             * @return std::string
             */
            std::string GetValue() const { return m_Value; }

        private:
            std::string m_Value{0};
        };

        /**
         * Integer AST node representation
         */
        class Integer : public Expression
        {
        public:
            explicit Integer(long long value) : m_Value(value) {}

            ~Integer() override = default;

            /**
             * @brief Return the node value
             *
             * @return int
             */
            int GetValue() const { return m_Value; }

        private:
            int m_Value{0};
        };

        /**
         * Identifier AST node representation
         */
        class Identifier : public Expression
        {
        public:
            Identifier(std::string name) : m_Value(std::move(name)) {}

            ~Identifier() override = default;

            /**
             * @brief Return the identifier name
             *
             * @return std::string
             */
            std::string GetName() const { return m_Value; }

        private:
            std::string m_Value;
        };

        class TryExpr : public Expression
        {
        public:
            explicit TryExpr() {};
            ~TryExpr() override = default;
        };

        class CallExpr : public Expression
        {
        public:
            explicit CallExpr() {};
            ~CallExpr() override = default;
        };

        class BlockExpr : public Expression
        {
        public:
            explicit BlockExpr() {};
            ~BlockExpr() override = default;
        };

        class AssignExpr : public Expression
        {
        public:
            explicit AssignExpr() {};
            ~AssignExpr() override = default;
        };

        class BinaryExpr : public Expression
        {
        public:
            explicit BinaryExpr() {};
            ~BinaryExpr() override = default;
        };

        /**
         * Ternary Operator AST representation node
         * coditionalExpr ? thenExpr : elseExpr
         */
        class TernaryExpr : public Expression
        {
        public:
            explicit TernaryExpr(Expression *condExpr, Expression *thenExpr, Expression *elseExpr)
                : m_CondExpr(condExpr), m_ThenExpr(thenExpr), m_ElseExpr(elseExpr) {};

            ~TernaryExpr() override
            {
                delete m_CondExpr;
                delete m_ThenExpr;
                delete m_ElseExpr;
            }

        private:
            Expression *m_CondExpr{nullptr};
            Expression *m_ThenExpr{nullptr};
            Expression *m_ElseExpr{nullptr};
        };

        class PrefixUnaryExpr : public Expression
        {
        public:
            explicit PrefixUnaryExpr() {};
            ~PrefixUnaryExpr() override = default;
        };

        class PostfixUnaryExpr : public Expression
        {
        public:
            explicit PostfixUnaryExpr() {};
            ~PostfixUnaryExpr() override = default;
        };

    } // namespace ast

} // namespace cora

#endif // CORA_AST_EXPRESSION_H
