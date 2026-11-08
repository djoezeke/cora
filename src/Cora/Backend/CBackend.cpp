#include <Cora/Backend/CBackend.h>

#include <sstream>

namespace cora::backend
{
    namespace
    {
        using cora::ast::Expr;
        using cora::ast::Statement;
        using cora::ast::TypeRef;

        std::string cType(const TypeRef &type)
        {
            if (type.name == "void")
                return "void";
            if (type.name == "bool")
                return "bool";
            if (type.name == "u8") return "uint8_t";
            if (type.name == "u16") return "uint16_t";
            if (type.name == "u32") return "uint32_t";
            if (type.name == "u64") return "uint64_t";
            if (type.name == "i8") return "int8_t";
            if (type.name == "i16") return "int16_t";
            if (type.name == "i32") return "int32_t";
            if (type.name == "i64") return "int64_t";
            if (type.name == "f32") return "float";
            if (type.name == "f64") return "double";
            if (type.name == "string") return "const char *";
            return "uint32_t";
        }

        std::string expression(const Expr &expr)
        {
            switch (expr.kind)
            {
            case Expr::Kind::Integer:
                return std::to_string(expr.integerValue);
            case Expr::Kind::Boolean:
                return expr.booleanValue ? "true" : "false";
            case Expr::Kind::Name:
                return expr.name;
            case Expr::Kind::Error:
                return "0";
            case Expr::Kind::Unary:
                if (expr.operands.empty())
                    return "0";
                return "(" + expr.op + expression(*expr.operands.front()) + ")";
            case Expr::Kind::Binary:
                if (expr.operands.size() < 2)
                    return "0";
                return "(" + expression(*expr.operands[0]) + " " + expr.op + " " +
                       expression(*expr.operands[1]) + ")";
            case Expr::Kind::Call:
            {
                std::ostringstream output;
                output << expr.name << '(';
                for (std::size_t i = 0; i < expr.operands.size(); ++i)
                {
                    if (i != 0)
                        output << ", ";
                    output << expression(*expr.operands[i]);
                }
                output << ')';
                return output.str();
            }
            }
            return "0";
        }

        void indent(std::ostream &out, unsigned depth)
        {
            for (unsigned i = 0; i < depth; ++i)
                out << "    ";
        }

        void emitStatement(std::ostream &out, const Statement &statement,
                           unsigned depth, const TypeRef &functionReturn);

        void emitBlock(std::ostream &out, const Statement &block, unsigned depth,
                       const TypeRef &functionReturn, bool braces)
        {
            if (braces)
            {
                indent(out, depth);
                out << "{\n";
            }

            for (const auto &statement : block.statements)
                emitStatement(out, *statement, depth + (braces ? 1u : 0u), functionReturn);

            if (block.tail)
            {
                indent(out, depth + (braces ? 1u : 0u));
                if (!functionReturn.isVoid())
                    out << "return ";
                out << expression(*block.tail) << ";\n";
            }

            if (braces)
            {
                indent(out, depth);
                out << "}\n";
            }
        }

        void emitStatement(std::ostream &out, const Statement &statement,
                           unsigned depth, const TypeRef &functionReturn)
        {
            switch (statement.kind)
            {
            case Statement::Kind::Block:
                emitBlock(out, statement, depth, functionReturn, true);
                break;
            case Statement::Kind::Variable:
                indent(out, depth);
                out << (statement.isConst ? "const " : "") << cType(statement.type) << ' '
                    << statement.name;
                if (statement.expression)
                    out << " = " << expression(*statement.expression);
                out << ";\n";
                break;
            case Statement::Kind::If:
                indent(out, depth);
                out << "if (" << (statement.condition ? expression(*statement.condition) : "0")
                    << ")\n";
                if (statement.thenBranch)
                    emitStatement(out, *statement.thenBranch, depth, functionReturn);
                if (statement.elseBranch)
                {
                    indent(out, depth);
                    out << "else\n";
                    emitStatement(out, *statement.elseBranch, depth, functionReturn);
                }
                break;
            case Statement::Kind::While:
                indent(out, depth);
                out << "while (" << (statement.condition ? expression(*statement.condition) : "0")
                    << ")\n";
                if (statement.body)
                    emitStatement(out, *statement.body, depth, functionReturn);
                break;
            case Statement::Kind::Return:
                indent(out, depth);
                out << "return";
                if (statement.expression)
                    out << ' ' << expression(*statement.expression);
                out << ";\n";
                break;
            case Statement::Kind::Expression:
                indent(out, depth);
                out << (statement.expression ? expression(*statement.expression) : "0") << ";\n";
                break;
            }
        }

        std::string functionReturnType(const cora::ast::Function &function)
        {
            // C requires the hosted entry point to return int.  The source
            // language may use u32 for main, so the backend adapts only this
            // ABI boundary.
            return function.name == "main" ? "int" : cType(function.returnType);
        }

        void emitFunctionSignature(std::ostream &out,
                                   const cora::ast::Function &function)
        {
            out << functionReturnType(function) << ' ' << function.name << '(';
            for (std::size_t i = 0; i < function.parameters.size(); ++i)
            {
                if (i != 0)
                    out << ", ";
                out << cType(function.parameters[i].type) << ' ' << function.parameters[i].name;
            }
            out << ')';
        }

        void emitFunction(std::ostream &out, const cora::ast::Function &function)
        {
            emitFunctionSignature(out, function);
            out << "\n";
            if (function.body)
                emitBlock(out, *function.body, 0,
                          function.name == "main" ? TypeRef{"i32", 32, false}
                                                   : function.returnType,
                          true);
            else
                out << "{ }\n";
            out << '\n';
        }
    } // namespace

    std::string CBackend::emitC(const cora::mir::MIRModule &mir)
    {
        std::ostringstream output;
        output << "/* Generated by Cora C backend; target: " << mir.target.triple << " */\n"
               << "#include <stdint.h>\n"
               << "#include <stdbool.h>\n\n";

        if (mir.ast)
        {
            for (const auto &function : mir.ast->module.functions)
            {
                emitFunctionSignature(output, *function);
                output << ";\n";
            }
            output << '\n';
            for (const auto &function : mir.ast->module.functions)
                emitFunction(output, *function);
        }
        else
        {
            output << "void cora_generated_unit(void)\n{\n"
                   << "    /* CIR did not carry an AST */\n"
                   << "}\n";
        }
        return output.str();
    }

    ObjectBuffer CBackend::emitObject(const cora::mir::MIRModule &mir,
                                      const cora::codegen::TargetSpec &target)
    {
        std::ostringstream output;
        output << "CORA-OBJECT-SKELETON-V2\n"
               << "target=" << target.triple << '\n'
               << "format=" << target.objectFormat << '\n'
               << "mir-input=" << mir.input.hex() << '\n'
               << emitC(mir);

        const std::string text = output.str();
        ObjectBuffer result;
        result.format = target.objectFormat;
        result.bytes.assign(text.begin(), text.end());
        return result;
    }

} // namespace cora::backend
