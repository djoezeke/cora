#include <Cora/CIR/CIR.h>
#include <Cora/Core/SHA256.h>

#include <charconv>
#include <iomanip>
#include <sstream>

namespace cora::cir
{
    namespace
    {
        std::string field(const std::string &text, std::string_view name)
        {
            const std::string prefix = std::string(name) + "=";
            std::size_t lineStart = 0;
            while (lineStart < text.size())
            {
                const std::size_t lineEnd = text.find('\n', lineStart);
                const std::size_t lineLength = lineEnd == std::string::npos
                                                   ? text.size() - lineStart
                                                   : lineEnd - lineStart;
                if (text.compare(lineStart, prefix.size(), prefix) == 0)
                    return text.substr(lineStart + prefix.size(), lineLength - prefix.size());
                if (lineEnd == std::string::npos)
                    break;
                lineStart = lineEnd + 1;
            }
            return {};
        }

        bool number(std::string_view text, std::size_t &value)
        {
            const auto parsed = std::from_chars(text.data(), text.data() + text.size(), value);
            return parsed.ec == std::errc{} && parsed.ptr == text.data() + text.size();
        }

        void writeRange(std::ostream &out, const cora::ast::SourceRange &range)
        {
            out << range.begin.offset << ' ' << range.begin.line << ' ' << range.begin.column << ' '
                << range.end.offset << ' ' << range.end.line << ' ' << range.end.column << ' ';
        }

        bool readRange(std::istream &in, cora::ast::SourceRange &range)
        {
            return static_cast<bool>(in >> range.begin.offset >> range.begin.line >> range.begin.column
                                      >> range.end.offset >> range.end.line >> range.end.column);
        }

        void writeType(std::ostream &out, const cora::ast::TypeRef &type)
        {
            out << std::quoted(type.name) << ' ' << type.bits << ' ' << type.isUnsigned << ' ';
        }

        bool readType(std::istream &in, cora::ast::TypeRef &type)
        {
            return static_cast<bool>(in >> std::quoted(type.name) >> type.bits >> type.isUnsigned);
        }

        void writeExpr(std::ostream &out, const cora::ast::Expr &expr)
        {
            out << static_cast<int>(expr.kind) << ' ';
            writeRange(out, expr.range);
            out << expr.integerValue << ' ' << expr.booleanValue << ' '
                << std::quoted(expr.name) << ' ' << std::quoted(expr.op) << ' '
                << expr.operands.size() << ' ';
            for (const auto &operand : expr.operands)
                writeExpr(out, *operand);
        }

        bool readExpr(std::istream &in, std::unique_ptr<cora::ast::Expr> &result)
        {
            int kind = 0;
            auto expression = std::make_unique<cora::ast::Expr>();
            std::size_t operands = 0;
            if (!(in >> kind) || !readRange(in, expression->range) ||
                !(in >> expression->integerValue >> expression->booleanValue >>
                  std::quoted(expression->name) >> std::quoted(expression->op) >> operands))
                return false;
            if (kind < static_cast<int>(cora::ast::Expr::Kind::Integer) ||
                kind > static_cast<int>(cora::ast::Expr::Kind::Error))
                return false;
            expression->kind = static_cast<cora::ast::Expr::Kind>(kind);
            for (std::size_t i = 0; i < operands; ++i)
            {
                std::unique_ptr<cora::ast::Expr> operand;
                if (!readExpr(in, operand))
                    return false;
                expression->operands.push_back(std::move(operand));
            }
            result = std::move(expression);
            return true;
        }

        void writeStatement(std::ostream &out, const cora::ast::Statement &statement)
        {
            out << static_cast<int>(statement.kind) << ' ';
            writeRange(out, statement.range);
            out << std::quoted(statement.name) << ' ' << statement.isConst << ' ';
            writeType(out, statement.type);
            const auto writeExprSlot = [&](const std::unique_ptr<cora::ast::Expr> &expr) {
                out << static_cast<bool>(expr) << ' ';
                if (expr)
                    writeExpr(out, *expr);
            };
            const auto writeStatementSlot = [&](const std::unique_ptr<cora::ast::Statement> &child) {
                out << static_cast<bool>(child) << ' ';
                if (child)
                    writeStatement(out, *child);
            };
            writeExprSlot(statement.expression);
            writeExprSlot(statement.condition);
            writeExprSlot(statement.tail);
            writeStatementSlot(statement.thenBranch);
            writeStatementSlot(statement.elseBranch);
            writeStatementSlot(statement.body);
            out << statement.statements.size() << ' ';
            for (const auto &child : statement.statements)
                writeStatement(out, *child);
        }

        bool readStatement(std::istream &in, std::unique_ptr<cora::ast::Statement> &result)
        {
            int kind = 0;
            auto statement = std::make_unique<cora::ast::Statement>();
            if (!(in >> kind) || !readRange(in, statement->range) ||
                !(in >> std::quoted(statement->name) >> statement->isConst) ||
                !readType(in, statement->type))
                return false;
            if (kind < static_cast<int>(cora::ast::Statement::Kind::Block) ||
                kind > static_cast<int>(cora::ast::Statement::Kind::Expression))
                return false;
            statement->kind = static_cast<cora::ast::Statement::Kind>(kind);

            const auto readExprSlot = [&](std::unique_ptr<cora::ast::Expr> &expr) {
                bool present = false;
                return static_cast<bool>(in >> present) && (!present || readExpr(in, expr));
            };
            const auto readStatementSlot = [&](std::unique_ptr<cora::ast::Statement> &child) {
                bool present = false;
                return static_cast<bool>(in >> present) && (!present || readStatement(in, child));
            };
            if (!readExprSlot(statement->expression) || !readExprSlot(statement->condition) ||
                !readExprSlot(statement->tail) || !readStatementSlot(statement->thenBranch) ||
                !readStatementSlot(statement->elseBranch) || !readStatementSlot(statement->body))
                return false;
            std::size_t children = 0;
            if (!(in >> children))
                return false;
            for (std::size_t i = 0; i < children; ++i)
            {
                std::unique_ptr<cora::ast::Statement> child;
                if (!readStatement(in, child))
                    return false;
                statement->statements.push_back(std::move(child));
            }
            result = std::move(statement);
            return true;
        }

        std::string serializeAST(const cora::ast::AST &ast)
        {
            std::ostringstream out;
            out << "AST1 " << ast.module.functions.size() << ' ';
            for (const auto &function : ast.module.functions)
            {
                writeRange(out, function->range);
                out << std::quoted(function->name) << ' ' << function->isPublic << ' ';
                writeType(out, function->returnType);
                out << function->parameters.size() << ' ';
                for (const auto &parameter : function->parameters)
                {
                    out << std::quoted(parameter.name) << ' ';
                    writeType(out, parameter.type);
                }
                out << static_cast<bool>(function->body) << ' ';
                if (function->body)
                    writeStatement(out, *function->body);
            }
            return out.str();
        }

        std::optional<std::string> decodeHex(std::string_view text)
        {
            if (text.size() % 2 != 0)
                return std::nullopt;
            auto digit = [](char character) -> int {
                if (character >= '0' && character <= '9') return character - '0';
                if (character >= 'a' && character <= 'f') return character - 'a' + 10;
                if (character >= 'A' && character <= 'F') return character - 'A' + 10;
                return -1;
            };
            std::string result;
            result.reserve(text.size() / 2);
            for (std::size_t i = 0; i < text.size(); i += 2)
            {
                const int high = digit(text[i]);
                const int low = digit(text[i + 1]);
                if (high < 0 || low < 0)
                    return std::nullopt;
                result.push_back(static_cast<char>((high << 4) | low));
            }
            return result;
        }

        std::string encodeHex(std::string_view text)
        {
            static constexpr char digits[] = "0123456789abcdef";
            std::string result;
            result.reserve(text.size() * 2);
            for (unsigned char character : text)
            {
                result.push_back(digits[character >> 4]);
                result.push_back(digits[character & 0x0f]);
            }
            return result;
        }

        bool deserializeAST(std::string_view payload, cora::ast::AST &ast)
        {
            std::istringstream in{std::string(payload)};
            std::string version;
            std::size_t functions = 0;
            if (!(in >> version >> functions) || version != "AST1")
                return false;
            for (std::size_t i = 0; i < functions; ++i)
            {
                auto function = std::make_unique<cora::ast::Function>();
                if (!readRange(in, function->range) || !(in >> std::quoted(function->name) >> function->isPublic) ||
                    !readType(in, function->returnType))
                    return false;
                std::size_t parameters = 0;
                if (!(in >> parameters))
                    return false;
                for (std::size_t p = 0; p < parameters; ++p)
                {
                    cora::ast::Parameter parameter;
                    if (!(in >> std::quoted(parameter.name)) || !readType(in, parameter.type))
                        return false;
                    function->parameters.push_back(std::move(parameter));
                }
                bool hasBody = false;
                if (!(in >> hasBody))
                    return false;
                if (hasBody && !readStatement(in, function->body))
                    return false;
                ast.module.functions.push_back(std::move(function));
            }
            return true;
        }

        cora::core::Digest interfaceDigest(const cora::ast::AST &ast)
        {
            std::ostringstream input;
            for (const auto &function : ast.module.functions)
            {
                input << function->name << '(';
                for (const auto &parameter : function->parameters)
                    input << parameter.type.name << ',';
                input << ")->" << function->returnType.name << '\n';
            }
            return cora::crypto::SHA256::hash(input.str());
        }
    } // namespace

    std::string CIRInput::canonical() const
    {
        std::ostringstream output;
        output << frontendVersion << '\n'
               << sourceFileId << '\n'
               << moduleId << '\n'
               << sourceDigest.hex() << '\n'
               << frontendOptions << '\n';
        for (const auto &digest : generatedInputDigests)
            output << digest.hex() << '\n';
        return output.str();
    }

    cora::core::Digest CIRInput::key() const
    {
        return cora::crypto::SHA256::hash("cora-cir-key-v2\n" + canonical());
    }

    std::vector<std::uint8_t> CIRArtifact::serialize() const
    {
        std::ostringstream output;
        output << "CORA-CIR-V2\n"
               << "key=" << key.hex() << '\n'
               << "source-file=" << input.sourceFileId << '\n'
               << "module=" << input.moduleId << '\n'
               << "source-digest=" << input.sourceDigest.hex() << '\n'
               << "interface-digest=" << publicInterfaceDigest.hex() << '\n'
               << "tokens=" << module.tokenCount << '\n'
               << "lines=" << module.lineCount << '\n'
               << "functions=" << module.functionCount << '\n';
        if (module.ast)
            output << "ast=" << encodeHex(serializeAST(*module.ast)) << '\n';
        const std::string text = output.str();
        return {text.begin(), text.end()};
    }

    std::optional<CIRArtifact> CIRArtifact::deserialize(const std::vector<std::uint8_t> &bytes,
                                                        std::string *error)
    {
        const std::string text(bytes.begin(), bytes.end());
        if (text.rfind("CORA-CIR-V2\n", 0) != 0)
        {
            if (error)
                *error = "invalid CIR magic or version";
            return std::nullopt;
        }

        const auto key = cora::core::Digest::fromHex(field(text, "key"));
        const auto sourceDigest = cora::core::Digest::fromHex(field(text, "source-digest"));
        const auto interface = cora::core::Digest::fromHex(field(text, "interface-digest"));
        if (!key || !sourceDigest || !interface)
        {
            if (error)
                *error = "invalid CIR digest field";
            return std::nullopt;
        }

        CIRArtifact result;
        result.key = *key;
        result.input.sourceFileId = field(text, "source-file");
        result.input.moduleId = field(text, "module");
        result.input.sourceDigest = *sourceDigest;
        result.publicInterfaceDigest = *interface;
        if (result.input.sourceFileId.empty() || result.input.moduleId.empty() ||
            !number(field(text, "tokens"), result.module.tokenCount) ||
            !number(field(text, "lines"), result.module.lineCount) ||
            !number(field(text, "functions"), result.module.functionCount))
        {
            if (error)
                *error = "invalid CIR metadata";
            return std::nullopt;
        }

        const std::string astHex = field(text, "ast");
        if (!astHex.empty())
        {
            const auto payload = decodeHex(astHex);
            auto ast = std::make_shared<cora::ast::AST>();
            if (!payload || !deserializeAST(*payload, *ast))
            {
                if (error)
                    *error = "invalid CIR AST payload";
                return std::nullopt;
            }
            result.module.ast = std::move(ast);
        }
        return result;
    }

    CIRArtifact build(const CIRInput &input, const cora::ParsedUnit &parsed)
    {
        CIRArtifact result;
        result.key = input.key();
        result.input = input;
        result.module.tokenCount = parsed.tokens.significantCount;
        result.module.lineCount = 1;
        for (char character : parsed.tokens.source)
            if (character == '\n')
                ++result.module.lineCount;
        result.module.functionCount = parsed.ast ? parsed.ast->module.functions.size() : 0;
        result.module.ast = parsed.ast;
        result.publicInterfaceDigest = parsed.ast ? interfaceDigest(*parsed.ast)
                                                  : cora::crypto::SHA256::hash(input.moduleId);
        return result;
    }

} // namespace cora::cir
