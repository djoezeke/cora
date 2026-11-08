#include <Cora/AIR/AIR.h>

#include <algorithm>
#include <unordered_set>

namespace cora::air
{
    namespace
    {
        std::size_t countStatements(const cora::ast::Statement &statement)
        {
            std::size_t count = statement.statements.size();
            if (statement.thenBranch) count += countStatements(*statement.thenBranch);
            if (statement.elseBranch) count += countStatements(*statement.elseBranch);
            if (statement.body) count += countStatements(*statement.body);
            for (const auto &child : statement.statements)
                if (child->kind == cora::ast::Statement::Kind::Block)
                    count += countStatements(*child);
            return count + 1;
        }
    } // namespace

    AIRModule SemanticAnalyzer::analyze(const cora::cir::CIRArtifact &cir)
    {
        AIRModule result;
        result.input = cir.key;
        result.moduleId = cir.input.moduleId;
        result.ast = cir.module.ast;
        if (!result.ast)
        {
            result.errors.push_back("CIR artifact has no parsed AST");
            return result;
        }

        std::unordered_set<std::string> functions;
        for (const auto &function : result.ast->module.functions)
        {
            if (!functions.insert(function->name).second)
                result.errors.push_back("duplicate function: " + function->name);
            AIRFunction airFunction;
            airFunction.name = function->name;
            airFunction.returnType = function->returnType;
            airFunction.parameterCount = function->parameters.size();
            airFunction.statementCount = function->body ? countStatements(*function->body) : 0;
            result.functions.push_back(std::move(airFunction));
            result.instructions.push_back({"function", function->name});
        }
        result.verified = result.errors.empty();
        return result;
    }

    AIRModule AIRPassManager::optimize(AIRModule module, int optimizationLevel)
    {
        if (optimizationLevel > 0)
        {
            module.instructions.erase(
                std::unique(module.instructions.begin(), module.instructions.end(),
                            [](const AIRInstruction &left, const AIRInstruction &right) {
                                return left.opcode == right.opcode && left.operand == right.operand;
                            }),
                module.instructions.end());
        }
        return module;
    }

} // namespace cora::air
