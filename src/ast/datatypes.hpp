#ifndef TOY_AST_DATATYPES_H
#define TOY_AST_DATATYPES_H

#include "expression.hpp"

#define DATA_TYPE(type)            \
    class type : public Node       \
    {                              \
    public:                        \
        type(Token *_token);       \
        virtual ~type() = default; \
                                   \
        GET_TYPE(type);            \
        STR_TYPE(type);            \
        /* ACCEPT();   */          \
        CODEGEN();                 \
                                   \
        Token *token;              \
    }

BEGIN_NAMESPACE

DATA_TYPE(Identifier);
DATA_TYPE(Document);
DATA_TYPE(Integer);
DATA_TYPE(Float);
DATA_TYPE(Double);
DATA_TYPE(String);
DATA_TYPE(Byte);
DATA_TYPE(Bool);
DATA_TYPE(None);

END_NAMESPACE

#endif // TOY_AST_DATATYPES_H
