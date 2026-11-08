#include <Cora/AST/AST.h>

namespace cora::ast
{
    namespace
    {
        std::uint32_t storeExpr(ASTStorage &storage, const Expr &expr)
        {
            const auto index = static_cast<std::uint32_t>(storage.expressionKinds.size());
            storage.expressionKinds.push_back(expr.kind);
            storage.expressionNames.push_back(expr.name);
            storage.expressionOperators.push_back(expr.op);
            storage.expressionIntegerValues.push_back(expr.integerValue);
            storage.expressionOperands.push_back(
                static_cast<std::uint32_t>(expr.operands.size()));
            for (const auto &operand : expr.operands)
                storeExpr(storage, *operand);
            return index;
        }

        std::uint32_t storeStatement(ASTStorage &storage, const Statement &statement)
        {
            const auto index = static_cast<std::uint32_t>(storage.statementKinds.size());
            storage.statementKinds.push_back(statement.kind);
            storage.statementNames.push_back(statement.name);
            storage.statementExpressions.push_back(statement.expression
                                                        ? storeExpr(storage, *statement.expression)
                                                        : UINT32_MAX);
            storage.statementChildren.push_back(
                static_cast<std::uint32_t>(statement.statements.size()));
            for (const auto &child : statement.statements)
                storeStatement(storage, *child);
            if (statement.thenBranch)
                storeStatement(storage, *statement.thenBranch);
            if (statement.elseBranch)
                storeStatement(storage, *statement.elseBranch);
            if (statement.body)
                storeStatement(storage, *statement.body);
            if (statement.condition)
                storeExpr(storage, *statement.condition);
            if (statement.tail)
                storeExpr(storage, *statement.tail);
            return index;
        }

        void indent(std::ostream &out, unsigned depth)
        {
            for (unsigned i = 0; i < depth; ++i)
                out << "  ";
        }

        void dumpType(std::ostream &out, const TypeRef &type)
        {
            out << type.name;
        }

        void dumpExpr(std::ostream &out, const Expr &expr, unsigned depth)
        {
            indent(out, depth);
            switch (expr.kind)
            {
            case Expr::Kind::Integer: out << "Integer(" << expr.integerValue << ")"; break;
            case Expr::Kind::Boolean: out << "Boolean(" << (expr.booleanValue ? "true" : "false") << ")"; break;
            case Expr::Kind::Name: out << "Name(" << expr.name << ")"; break;
            case Expr::Kind::Unary: out << "Unary(" << expr.op << ")"; break;
            case Expr::Kind::Binary: out << "Binary(" << expr.op << ")"; break;
            case Expr::Kind::Call: out << "Call(" << expr.name << ")"; break;
            case Expr::Kind::FieldAccess: out << "Field(" << expr.name << ")"; break;
            case Expr::Kind::StructLiteral: out << "Struct(" << expr.name << ")"; break;
            case Expr::Kind::Error: out << "Error"; break;
            }
            out << '\n';
            for (const auto &operand : expr.operands)
                dumpExpr(out, *operand, depth + 1);
        }

        void dumpStatement(std::ostream &out, const Statement &statement, unsigned depth)
        {
            indent(out, depth);
            switch (statement.kind)
            {
            case Statement::Kind::Block:
                out << "Block\n";
                for (const auto &child : statement.statements)
                    dumpStatement(out, *child, depth + 1);
                if (statement.tail)
                {
                    indent(out, depth + 1);
                    out << "Tail\n";
                    dumpExpr(out, *statement.tail, depth + 2);
                }
                break;
            case Statement::Kind::Variable:
                out << (statement.isConst ? "Const(" : "Let(") << statement.name << ":";
                dumpType(out, statement.type);
                out << ")\n";
                if (statement.expression)
                    dumpExpr(out, *statement.expression, depth + 1);
                break;
            case Statement::Kind::If:
                out << "If\n";
                indent(out, depth + 1);
                out << "Condition\n";
                dumpExpr(out, *statement.condition, depth + 2);
                indent(out, depth + 1);
                out << "Then\n";
                dumpStatement(out, *statement.thenBranch, depth + 2);
                if (statement.elseBranch)
                {
                    indent(out, depth + 1);
                    out << "Else\n";
                    dumpStatement(out, *statement.elseBranch, depth + 2);
                }
                break;
            case Statement::Kind::While:
                out << "While\n";
                dumpExpr(out, *statement.condition, depth + 1);
                dumpStatement(out, *statement.body, depth + 1);
                break;
            case Statement::Kind::Return:
                out << "Return\n";
                if (statement.expression)
                    dumpExpr(out, *statement.expression, depth + 1);
                break;
            case Statement::Kind::Expression:
                out << "Expression\n";
                dumpExpr(out, *statement.expression, depth + 1);
                break;
            }
        }
    } // namespace

    void AST::buildStorage()
    {
        storage = {};
        for (const auto &function : module.functions)
        {
            storage.functionNames.push_back(function->name);
            storage.functionReturnTypes.push_back(function->returnType);
            storage.functionBodyStatements.push_back(
                function->body ? storeStatement(storage, *function->body) : UINT32_MAX);
        }
    }

    void AST::dump(std::ostream &out) const
    {
        out << "Module\n";
        for (const auto &function : module.functions)
        {
            indent(out, 1);
            out << "Function " << function->name << " -> ";
            dumpType(out, function->returnType);
            out << '\n';
            for (const auto &parameter : function->parameters)
            {
                indent(out, 2);
                out << "Param " << parameter.name << ":";
                dumpType(out, parameter.type);
                out << '\n';
            }
            if (function->body)
                dumpStatement(out, *function->body, 2);
        }
    }

} // namespace cora::ast
