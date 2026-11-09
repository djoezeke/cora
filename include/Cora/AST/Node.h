#ifndef CORA_AST_NODES_H
#define CORA_AST_NODES_H

#include <Cora/Lexer/Token.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cora::ast
{

    class Node
    {

        using NIndex = uint32_t; // node reference
        using TIndex = uint32_t; // token reference
        using EIndex = uint32_t; // reference into extra_data

        enum class Tag
        {
            root,
            func_decl,
        };

        union Data
        {
            uint32_t lhs;
            uint32_t rhs;
        };

        Tag tag;
        Data data;
        uint32_t main_token;
    }
}

#endif // CORA_AST_NODES_H
