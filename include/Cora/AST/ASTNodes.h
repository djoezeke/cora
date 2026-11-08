#ifndef CORA_AST_ASTNODES_H
#define CORA_AST_ASTNODES_H

#include <Cora/Lexer/Token.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cora::ast
{
    using SourceRange = cora::SourceRange;

    struct TypeRef
    {
        std::string name = "void";
        std::uint32_t bits = 0;
        bool isUnsigned = false;
        [[nodiscard]] bool isVoid() const noexcept { return name == "void"; }
    };

    struct Expr
    {
        enum class Kind { Integer, Boolean, Name, Unary, Binary, Call, FieldAccess,
                          StructLiteral, Error };
        Kind kind = Kind::Error;
        SourceRange range{};
        std::int64_t integerValue = 0;
        bool booleanValue = false;
        std::string name;
        std::string op;
        std::vector<std::unique_ptr<Expr>> operands;
    };

    struct Statement
    {
        enum class Kind { Block, Variable, If, While, Return, Expression };
        Kind kind = Kind::Expression;
        SourceRange range{};
        std::string name;
        bool isConst = false;
        TypeRef type{};
        std::unique_ptr<Expr> expression;
        std::unique_ptr<Expr> condition;
        std::unique_ptr<Expr> tail;
        std::unique_ptr<Statement> thenBranch;
        std::unique_ptr<Statement> elseBranch;
        std::unique_ptr<Statement> body;
        std::vector<std::unique_ptr<Statement>> statements;
    };

    struct Parameter { std::string name; TypeRef type; };
    struct Field { std::string name; TypeRef type; };
    struct TypeDecl { std::string name; std::vector<Field> fields; };
    struct Function
    {
        SourceRange range{};
        std::string name;
        bool isPublic = false;
        std::vector<Parameter> parameters;
        TypeRef returnType{};
        std::unique_ptr<Statement> body;
    };
    struct Module
    {
        std::vector<TypeDecl> types;
        std::vector<std::unique_ptr<Function>> functions;
    };
}

#endif
