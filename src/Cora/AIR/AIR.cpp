#include <Cora/AIR/AIR.h>

#include <algorithm>
#include <unordered_set>

namespace cora::air
{
    AIRModule SemanticAnalyzer::analyze(const cir::CIRArtifact &artifact) const
    {
        AIRModule result = analyze(artifact.cir);
        result.input = artifact.key;
        result.moduleId = artifact.input.moduleId;
        return result;
    }

    AIRModule SemanticAnalyzer::analyze(const cir::CIR &module) const
    {
        AIRModule result;
        std::uint32_t next_id = 1;
        std::unordered_set<std::string> body_names;

        for (const cir::Body &body : module.bodies)
        {
            if (body.index != cir::BNull)
            {
                AIRFunction function;
                function.body = body.index;
                function.first_instruction =
                    static_cast<std::uint32_t>(result.instructions.size());
                for (std::size_t index = 1; index < body.insts.size(); ++index)
                {
                    const cir::Inst instruction = body.insts[index];
                    if (instruction.tag == cir::Tag::Invalid)
                    {
                        result.errors.push_back("invalid CIR instruction");
                        continue;
                    }
                    result.instructions.push_back(
                        {next_id++, static_cast<cir::IIndex>(index), instruction.tag,
                         instruction.a, instruction.b, instruction.c, instruction.op,
                         instruction.int_value, instruction.float_value,
                         instruction.bool_value, instruction.name, instruction.type_ref,
                         instruction.extra, instruction.extra_count, instruction.flags,
                         instruction.span});
                }
                function.instruction_count =
                    static_cast<std::uint32_t>(result.instructions.size()) -
                    function.first_instruction;
                result.functions.push_back(function);
                if (!body.name.empty() && !body_names.insert(body.name).second)
                    result.errors.push_back("duplicate CIR body: " + body.name);
            }
        }

        result.verified = result.errors.empty();
        return result;
    }

    AIRModule AIRPassManager::optimize(AIRModule module, int optimizationLevel) const
    {
        if (optimizationLevel <= 0)
            return module;

        module.instructions.erase(
            std::unique(module.instructions.begin(), module.instructions.end(),
                        [](const AIRInstruction &left, const AIRInstruction &right) {
                            return left.tag == right.tag && left.a == right.a &&
                                   left.b == right.b && left.c == right.c;
                        }),
            module.instructions.end());
        module.verified = module.errors.empty();
        return module;
    }
} // namespace cora::air
