#ifndef TOY_AST_UNARY_OPERATIONS_H
#define TOY_AST_UNARY_OPERATIONS_H

#include "expression.hpp"

#define UNARY_OPERATION(operation)           \
    class operation : public UnaryOperation  \
    {                                        \
    public:                                  \
        operation(Token &opp, Token &token); \
        virtual ~operation() = default;      \
                                             \
        GET_TYPE(operation);                 \
        STR_TYPE(operation);                 \
        /* ACCEPT();   */                    \
        CODEGEN();                           \
    }

BEGIN_NAMESPACE

class UnaryOperation : public Expression
{
public:
    UnaryOperation(Token &opp, Token &token);
    virtual ~UnaryOperation() = default;

    GET_TYPE(UnaryOperation);
    STR_TYPE(UnaryOperation);
    // ACCEPT();
    CODEGEN();

public:
    Token *oop;
    Token *token;
};

UNARY_OPERATION(UAdd);
UNARY_OPERATION(USub);

UNARY_OPERATION(Invert);

UNARY_OPERATION(Not);

END_NAMESPACE

#endif // TOY_AST_UNARY_OPERATIONS_H
