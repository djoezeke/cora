#ifndef CORA_AST_AST_H
#define CORA_AST_AST_H

#include <Cora/AST/ASTNodes.h>
#include <Cora/AST/ASTStorage.h>

#include <ostream>
#include <string>

namespace cora::ast
{
    struct AST
    {
        ASTStorage storage;
        std::string source;
        Module module;
        void buildStorage();
        void dump(std::ostream &out) const;
    };
}

#endif
