#ifndef CORA_AST_AST_H
#define CORA_AST_AST_H

#include <Cora/AST/ASTNodes.h>
#include <Cora/AST/ASTStorage.h>

#include <ostream>
#include <string>

namespace cora::ast
{

    // left(ast, id)
    // right(ast, id)

    struct AST
    {

        using NIndex = uint32_t; // node reference
        using TIndex = uint32_t; // token reference
        using EIndex = uint32_t; // reference into extra_data

        // ----------- Storage ------------
        // std::vector<ast::Node> nodes;
        // std::vector<uint32_t> extras;
        // std::vector<Token> tokens;
        // std::vector<Diagnostic> errors;

        ASTStorage storage;
        std::string source;
        Module module;
        void buildStorage();
        void dump(std::ostream &out) const;
    };
}

#endif
