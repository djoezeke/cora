#include "Cora/AST/ASTNode.hpp"

#define MAKE_GETNODEKIND(Node)      \
    NodeKind GetNodeKind() override \
    {                               \
        return NodeKind::Node;      \
    }

#define MAKE_GETKINDSTRING(Node)             \
    std::string GetNodeKindString() override \
    {                                        \
        return #Node;                        \
    }

namespace cora
{
    namespace ast
    {

    } // namespace ast

} // namespace cora
