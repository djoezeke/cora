#ifndef TOY_AST_STATEMENT_H
#define TOY_AST_STATEMENT_H

#include "ast/node.hpp"

BEGIN_NAMESPACE

class Statement : public Node
{
public:
    GET_TYPE(Statement);
    STR_TYPE(Statement);
    // ACCEPT_VISITOR();
};

END_NAMESPACE

#endif // TOY_AST_STATEMENT_H
