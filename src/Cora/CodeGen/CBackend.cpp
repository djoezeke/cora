#include <Cora/CodeGen/CBackend.h>

#include <algorithm>
#include <sstream>
#include <string>

namespace cora::codegen
{
    namespace
    {
        using AirList = std::vector<air::AIRInstruction>;

        const air::AIRInstruction *findInstruction(const AirList &instructions,
                                                   cir::IIndex index)
        {
            for (const air::AIRInstruction &instruction : instructions)
                if (instruction.cir_index == index)
                    return &instruction;
            return nullptr;
        }

        std::string sanitize(std::string result,
                            const std::string &fallback = "cora_value")
        {
            if (result.empty())
                return fallback;
            for (char &character : result)
                if (!((character >= 'a' && character <= 'z') ||
                      (character >= 'A' && character <= 'Z') ||
                      (character >= '0' && character <= '9') ||
                      character == '_'))
                    character = '_';
            if (result[0] >= '0' && result[0] <= '9')
                result.insert(result.begin(), '_');
            return result;
        }

        std::string identifier(const cir::CIR &module, std::uint32_t index,
                               const std::string &fallback = "cora_value")
        {
            return sanitize(std::string(module.string(index)), fallback);
        }

        std::string cType(const cir::CIR &module, cir::TIndex index)
        {
            if (index == cir::TNull || index >= module.types.size())
                return "int64_t";
            const cir::TypeRefInfo &type = module.types[index];
            switch (type.kind)
            {
            case cir::TypeRefKind::Named:
            {
                const std::string name(module.string(type.name));
                if (name == "void" || name == "none") return "void";
                if (name == "bool") return "bool";
                if (name == "char" || name == "u8" || name == "i8") return "int8_t";
                if (name == "u16" || name == "i16") return "int16_t";
                if (name == "u32" || name == "i32") return "int32_t";
                if (name == "u64" || name == "i64" || name == "int" ||
                    name == "uint") return "int64_t";
                if (name == "f32") return "float";
                if (name == "f64") return "double";
                if (name == "string" || name == "str") return "const char *";
                if (name.size() == 1 && name[0] >= 'A' && name[0] <= 'Z')
                    return "int64_t";
                return sanitize(name, "cora_type");
            }
            case cir::TypeRefKind::Pointer:
                return cType(module, type.element) + " *";
            case cir::TypeRefKind::Array:
                return cType(module, type.element) + " *";
            case cir::TypeRefKind::Function:
                return "void *";
            }
            return "int64_t";
        }

        std::string cDeclaration(const cir::CIR &module, cir::TIndex index,
                                 std::string name)
        {
            if (index != cir::TNull && index < module.types.size() &&
                module.types[index].kind == cir::TypeRefKind::Array)
            {
                const cir::TypeRefInfo &array = module.types[index];
                return cType(module, array.element) + " " + std::move(name) + "[" +
                       std::to_string(array.length) + "]";
            }
            return cType(module, index) + " " + std::move(name);
        }

        const char *binaryOperator(cora::Token::Tag tag)
        {
            switch (tag)
            {
            case cora::Token::Tag::T_PLUS: return "+";
            case cora::Token::Tag::T_MINUS: return "-";
            case cora::Token::Tag::T_MUL: return "*";
            case cora::Token::Tag::T_DIV: return "/";
            case cora::Token::Tag::T_MOD: return "%";
            case cora::Token::Tag::T_EQ: return "==";
            case cora::Token::Tag::T_NEQ: return "!=";
            case cora::Token::Tag::T_LT: return "<";
            case cora::Token::Tag::T_GT: return ">";
            case cora::Token::Tag::T_LTEQ: return "<=";
            case cora::Token::Tag::T_GTEQ: return ">=";
            case cora::Token::Tag::T_ANDAND: return "&&";
            case cora::Token::Tag::T_OROR: return "||";
            case cora::Token::Tag::T_BITAND: return "&";
            case cora::Token::Tag::T_BITOR: return "|";
            case cora::Token::Tag::T_XOR: return "^";
            case cora::Token::Tag::T_SHL: return "<<";
            case cora::Token::Tag::T_SHR: return ">>";
            default: return "+";
            }
        }

        std::string expression(const cir::CIR &module, const cir::Body &body,
                               const AirList &instructions, cir::IIndex index)
        {
            if (index == cir::INull)
                return "0";
            const air::AIRInstruction *value = findInstruction(instructions, index);
            if (value == nullptr)
                return "0";
            switch (value->tag)
            {
            case cir::Tag::IntLit:
            case cir::Tag::CharLit:
                return std::to_string(value->int_value);
            case cir::Tag::FloatLit:
            {
                std::ostringstream result;
                result << value->float_value;
                return result.str();
            }
            case cir::Tag::BoolLit:
                return value->bool_value ? "1" : "0";
            case cir::Tag::NullLit:
                return "0";
            case cir::Tag::StringLit:
                return "\"" + std::string(module.string(value->name)) + "\"";
            case cir::Tag::Identifier:
                return identifier(module, value->name);
            case cir::Tag::Unary:
                return "(" + std::string(
                                   value->op == cora::Token::Tag::T_MINUS ? "-" : "!") +
                       expression(module, body, instructions, value->a) + ")";
            case cir::Tag::Binary:
            case cir::Tag::Assign:
                return "(" + expression(module, body, instructions, value->a) + " " +
                       binaryOperator(value->op) + " " +
                       expression(module, body, instructions, value->b) + ")";
            case cir::Tag::Paren:
                return "(" + expression(module, body, instructions, value->a) + ")";
            case cir::Tag::Call:
            {
                std::ostringstream result;
                result << expression(module, body, instructions, value->a) << "(";
                const cir::ListView args = module.extraList(body, value->extra,
                                                            value->extra_count);
                for (std::size_t i = 0; i < args.size(); ++i)
                {
                    if (i != 0)
                        result << ", ";
                    result << expression(module, body, instructions, args[i]);
                }
                result << ")";
                return result.str();
            }
            case cir::Tag::FieldAccess:
            {
                const air::AIRInstruction *base =
                    findInstruction(instructions, value->a);
                const std::string separator =
                    base != nullptr && base->type_ref != cir::TNull &&
                            base->type_ref < module.types.size() &&
                            module.types[base->type_ref].kind ==
                                cir::TypeRefKind::Pointer
                        ? "->"
                        : ".";
                return expression(module, body, instructions, value->a) + separator +
                       identifier(module, value->name, "field");
            }
            case cir::Tag::Index:
                return expression(module, body, instructions, value->a) + "[" +
                       expression(module, body, instructions, value->b) + "]";
            case cir::Tag::ArrayInit:
            {
                std::ostringstream result;
                result << "{";
                const cir::ListView values =
                    module.extraList(body, value->extra, value->extra_count);
                for (std::size_t i = 0; i < values.size(); ++i)
                {
                    if (i != 0)
                        result << ", ";
                    result << expression(module, body, instructions, values[i]);
                }
                return result.str() + "}";
            }
            case cir::Tag::StructInit:
            {
                std::ostringstream result;
                result << "(" << identifier(module, value->name, "cora_struct")
                       << "){";
                const cir::ListView fields =
                    module.extraList(body, value->extra, value->extra_count);
                for (std::size_t i = 0; i + 1 < fields.size(); i += 2)
                {
                    if (i != 0)
                        result << ", ";
                    result << "." << identifier(module, fields[i], "field") << " = "
                           << expression(module, body, instructions, fields[i + 1]);
                }
                return result.str() + "}";
            }
            default:
                return "0";
            }
        }

        void emitStatement(std::ostream &out, const cir::CIR &module,
                           const cir::Body &body, const AirList &instructions,
                           cir::IIndex index, unsigned indent);

        void emitBlock(std::ostream &out, const cir::CIR &module,
                       const cir::Body &body, const AirList &instructions,
                       cir::IIndex index, unsigned indent)
        {
            const air::AIRInstruction *block = findInstruction(instructions, index);
            if (block == nullptr)
                return;
            const cir::ListView children =
                module.extraList(body, block->extra, block->extra_count);
            for (const std::uint32_t child : children)
                emitStatement(out, module, body, instructions, child, indent);
        }

        void emitIndent(std::ostream &out, unsigned count)
        {
            for (unsigned i = 0; i < count; ++i)
                out << "    ";
        }

        void emitStatement(std::ostream &out, const cir::CIR &module,
                           const cir::Body &body, const AirList &instructions,
                           cir::IIndex index, unsigned indent)
        {
            const air::AIRInstruction *value = findInstruction(instructions, index);
            if (value == nullptr)
                return;
            switch (value->tag)
            {
            case cir::Tag::FnDecl:
            case cir::Tag::Block:
                if (value->tag == cir::Tag::FnDecl)
                    emitBlock(out, module, body, instructions, value->a, indent);
                else
                    emitBlock(out, module, body, instructions, index, indent);
                break;
            case cir::Tag::ExprStmt:
                emitIndent(out, indent);
                out << expression(module, body, instructions, value->a) << ";\n";
                break;
            case cir::Tag::LetDecl:
                emitIndent(out, indent);
                out << cDeclaration(module, value->type_ref,
                                    identifier(module, value->name)) << " = "
                    << expression(module, body, instructions, value->b) << ";\n";
                emitIndent(out, indent);
                out << "(void)" << identifier(module, value->name) << ";\n";
                break;
            case cir::Tag::Return:
                emitIndent(out, indent);
                out << "return " << expression(module, body, instructions, value->a)
                    << ";\n";
                break;
            case cir::Tag::If:
                emitIndent(out, indent);
                out << "if (" << expression(module, body, instructions, value->a)
                    << ") {\n";
                emitStatement(out, module, body, instructions, value->b, indent + 1);
                emitIndent(out, indent);
                out << "}";
                if (value->c != cir::INull)
                {
                    out << " else {\n";
                    emitStatement(out, module, body, instructions, value->c, indent + 1);
                    emitIndent(out, indent);
                    out << "}";
                }
                out << "\n";
                break;
            case cir::Tag::While:
                emitIndent(out, indent);
                out << "while (" << expression(module, body, instructions, value->a)
                    << ") {\n";
                emitStatement(out, module, body, instructions, value->b, indent + 1);
                emitIndent(out, indent);
                out << "}\n";
                break;
            case cir::Tag::Break:
                emitIndent(out, indent);
                out << "break;\n";
                break;
            case cir::Tag::Continue:
                emitIndent(out, indent);
                out << "continue;\n";
                break;
            default:
                emitIndent(out, indent);
                out << expression(module, body, instructions, index) << ";\n";
                break;
            }
        }
    }

    std::string CBackend::generate(const cir::CIR &module,
                                   const air::AIRModule &air) const
    {
        std::ostringstream out;
        out << "/* Generated by Cora from verified AIR. */\n"
            << "#include <stdint.h>\n"
            << "#include <stddef.h>\n"
            << "#include <stdbool.h>\n\n";

        for (const cir::Body &body : module.bodies)
        {
            if (body.is_function || body.name.empty() || body.is_type_alias)
                continue;
            out << "typedef struct " << sanitize(body.name) << " {\n";
            for (const cir::Field &field : body.fields)
                out << "    " << cDeclaration(module, field.type_ref,
                                                identifier(module, field.name, "field"))
                    << ";\n";
            out << "} " << sanitize(body.name) << ";\n\n";
        }
        for (const cir::Body &body : module.bodies)
        {
            if (!body.is_type_alias || body.name.empty())
                continue;
            out << "typedef " << cType(module, body.alias_type) << " "
                << sanitize(body.name) << ";\n";
        }
        if (std::any_of(module.bodies.begin(), module.bodies.end(),
                        [](const cir::Body &body) { return body.is_type_alias; }))
            out << "\n";

        bool emitted_main = false;
        for (const air::AIRFunction &function : air.functions)
        {
            if (function.body >= module.bodies.size())
                continue;
            const cir::Body &body = module.bodies[function.body];
            if (!body.is_function)
                continue;
            const std::string name = sanitize(body.name, "cora_function");
            emitted_main = emitted_main || name == "main";
            out << (name == "main" ? "int " : cType(module, body.return_type) + " ")
                << name << "(";
            if (body.params.empty())
                out << "void";
            for (std::size_t i = 0; i < body.params.size(); ++i)
            {
                if (i != 0)
                    out << ", ";
                out << cDeclaration(module, body.params[i].type_ref,
                                    identifier(module, body.params[i].name));
            }
            out << ");\n";
        }
        out << "\n";

        for (const air::AIRFunction &function : air.functions)
        {
            if (function.body >= module.bodies.size())
                continue;
            const cir::Body &body = module.bodies[function.body];
            if (!body.is_function)
            {
                const AirList global_instructions(
                    air.instructions.begin() + function.first_instruction,
                    air.instructions.begin() + function.first_instruction +
                        function.instruction_count);
                for (const cir::IIndex root : body.roots)
                {
                    const air::AIRInstruction *value =
                        findInstruction(global_instructions, root);
                    if (value != nullptr && value->tag == cir::Tag::LetDecl)
                        out << "int64_t " << identifier(module, value->name) << " = "
                            << expression(module, body, global_instructions, value->b)
                            << ";\n";
                }
                continue;
            }
            const std::string name = sanitize(body.name, "cora_function");
            out << (name == "main" ? "int " : cType(module, body.return_type) + " ")
                << name << "(";
            if (body.params.empty())
                out << "void";
            for (std::size_t i = 0; i < body.params.size(); ++i)
            {
                if (i != 0)
                    out << ", ";
                out << cDeclaration(module, body.params[i].type_ref,
                                    identifier(module, body.params[i].name));
            }
            out << ")\n{\n";
            for (const cir::Param &param : body.params)
                out << "    (void)" << identifier(module, param.name) << ";\n";
            const AirList function_instructions(
                air.instructions.begin() + function.first_instruction,
                air.instructions.begin() + function.first_instruction +
                    function.instruction_count);
            for (const cir::IIndex root : body.roots)
                emitStatement(out, module, body, function_instructions, root, 1);
            out << "    return 0;\n}\n\n";
        }
        if (!emitted_main)
            out << "int main(void) { return 0; }\n";
        return out.str();
    }
} // namespace cora::codegen
