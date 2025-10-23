#ifndef CORA_AST_STATEMENT_H
#define CORA_AST_STATEMENT_H

#include "Cora/AST/ASTNode.hpp"

namespace cora
{
    namespace ast
    {

        class Statement : public Node
        {
            ~Statement() override = default;
        };

    } // namespace ast

} // namespace cora

#endif // CORA_AST_STATEMENT_H
