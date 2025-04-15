#ifndef TOY_AST_EXPRESSION_H
#define TOY_AST_EXPRESSION_H

#include "node.hpp"

BEGIN_NAMESPACE

class Expression : public Node
{
public:
    GET_TYPE(Expression);
    STR_TYPE(Expression);
    // ACCEPT_VISITOR();
};

END_NAMESPACE

#endif // TOY_AST_EXPRESSION_H
