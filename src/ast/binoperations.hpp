#ifndef TOY_AST_BINARY_OPERATIONS_H
#define TOY_AST_BINARY_OPERATIONS_H

#include "expression.hpp"

#define BINARY_OPERATION(operation)                       \
    class operation : public BinaryOperation              \
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

class BinaryOperation : public Expression
{
public:
    BinaryOperation(Token &left, Token &opp, Token &right);
    virtual ~BinaryOperation() = default;

    GET_TYPE(BinaryOperation);
    STR_TYPE(BinaryOperation);
    // ACCEPT();
    CODEGEN();

public:
    Token *right;
    Token *oop;
    Token *left;
};

BINARY_OPERATION(Add);
BINARY_OPERATION(Sub);
BINARY_OPERATION(Mul);
BINARY_OPERATION(Div);

BINARY_OPERATION(BitAnd);
BINARY_OPERATION(BitOr);
BINARY_OPERATION(BitXor);

BINARY_OPERATION(LShift);
BINARY_OPERATION(RShift);

BINARY_OPERATION(Mod);
BINARY_OPERATION(Pow);
BINARY_OPERATION(FloorDiv);

END_NAMESPACE

#endif // TOY_AST_BINARY_OPERATIONS_H
