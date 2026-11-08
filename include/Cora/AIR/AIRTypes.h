#ifndef CORA_AIR_AIRTYPES_H
#define CORA_AIR_AIRTYPES_H

#include <Cora/CIR/CIR.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace cora::air
{
    struct AIRInstruction
    {
        std::uint32_t result = 0;
        std::string opcode;
        std::vector<std::uint32_t> operands;
        std::string operand;
    };
    struct AIRFunction
    {
        std::string name;
        cora::ast::TypeRef returnType;
        std::size_t parameterCount = 0;
        std::size_t statementCount = 0;
    };

    struct AIRModule
    {
        cora::core::Digest input;
        std::string moduleId;
        std::shared_ptr<cora::ast::AST> ast;
        std::vector<AIRFunction> functions;
        std::vector<AIRInstruction> instructions;
        std::vector<std::uint32_t> basicBlockIds;
        std::vector<std::string> errors;
        bool verified = false;
    };
}

#endif
