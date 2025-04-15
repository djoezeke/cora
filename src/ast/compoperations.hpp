#ifndef TOY_AST_COMPARISSION_OPERATIONS_H
#define TOY_AST_COMPARISSION_OPERATIONS_H

#include "expression.hpp"

#define COMPARISSION_OPERATION(operation)                 \
    class operation : public CompOperation                \
    {                                                     \
    public:                                               \
        operation(Token &left, Token &opp, Token &right); \
        virtual ~operation() = default;                   \
                                                          \
        GET_TYPE(operation);                              \
        STR_TYPE(operation);                              \
        /* ACCEPT();   */                                 \
        CODEGEN();                                        \
    }

BEGIN_NAMESPACE

class CompOperation : public Expression
{
public:
    CompOperation(Token &left, Token &opp, Token &right);
    virtual ~CompOperation() = default;

    GET_TYPE(CompOperation);
    STR_TYPE(CompOperation);
    // ACCEPT();
    CODEGEN();

public:
    Token *oop;
    Token *token;
};

COMPARISSION_OPERATION(EQOperation);
COMPARISSION_OPERATION(NEQOperation);

COMPARISSION_OPERATION(GTOperation);
COMPARISSION_OPERATION(GTEOperation);

COMPARISSION_OPERATION(LTOperation);
COMPARISSION_OPERATION(LTEOperation);

COMPARISSION_OPERATION(InOperation);
COMPARISSION_OPERATION(NotInOperation);

COMPARISSION_OPERATION(IsOperation);
COMPARISSION_OPERATION(IsNotOperation);

END_NAMESPACE

#endif // TOY_AST_COMPARISSION_OPERATIONS_H
