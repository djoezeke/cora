#ifndef CORA_AST_ASTSTORAGE_H
#define CORA_AST_ASTSTORAGE_H

#include <Cora/AST/ASTNodes.h>

#include <cstdint>
#include <string>
#include <vector>

namespace cora::ast
{
    struct ASTStorage
    {
        std::vector<Statement::Kind> statementKinds;
        std::vector<std::string> statementNames;
        std::vector<std::uint32_t> statementExpressions;
        std::vector<std::uint32_t> statementChildren;
        std::vector<Expr::Kind> expressionKinds;
        std::vector<std::string> expressionNames;
        std::vector<std::string> expressionOperators;
        std::vector<std::int64_t> expressionIntegerValues;
        std::vector<std::uint32_t> expressionOperands;
        std::vector<std::string> functionNames;
        std::vector<TypeRef> functionReturnTypes;
        std::vector<std::uint32_t> functionBodyStatements;
    };
}

#endif
