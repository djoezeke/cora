#ifndef TOY_AST_MODIFIERS_H
#define TOY_AST_MODIFIERS_H

#include "expression.hpp"

#define MODIFICATION(modifier)               \
    class modifier : public Modification     \
    {                                        \
    public:                                  \
        modifier(Token &kind, Token &value); \
        virtual ~modifier() = default;       \
                                             \
        GET_TYPE(modifier);                  \
        STR_TYPE(modifier);                  \
        /* ACCEPT();   */                    \
        CODEGEN();                           \
    }

BEGIN_NAMESPACE

// values : char, str, bytes, bool, int, float, complex
class Modification : public Expression
{
public:
    Modification(Token &kind, Token &value);
    virtual ~Modification() = default;

    GET_TYPE(Modification);
    STR_TYPE(Modification);

    // ACCEPT();
    CODEGEN();

public:
    Token *kind;
    Token *value;
};

MODIFICATION(Const);

MODIFICATION(Long);
MODIFICATION(Short);

MODIFICATION(Signed);
MODIFICATION(Unsigned);

MODIFICATION(Static);
MODIFICATION(Extern);

END_NAMESPACE

#endif // TOY_AST_MODIFIERS_H
