#include <Cora/Sema/ComptimeEvaluator.h>

namespace cora::sema
{
    std::optional<std::int64_t> ComptimeEvaluator::evaluate(
        const cir::CIR &module, cir::BIndex body_index, cir::IIndex instruction) const
    {
        if (body_index >= module.bodies.size())
            return std::nullopt;
        const cir::Body &body = module.bodies[body_index];
        if (instruction == cir::INull || instruction >= body.insts.size())
            return std::nullopt;
        const cir::Inst inst = body.insts[instruction];
        if (inst.tag == cir::Tag::IntLit)
            return inst.int_value;
        if (inst.tag == cir::Tag::BoolLit)
            return inst.bool_value ? 1 : 0;
        const auto lhs = evaluate(module, body_index, inst.a);
        const auto rhs = evaluate(module, body_index, inst.b);
        if (!lhs || !rhs)
            return std::nullopt;
        switch (inst.op)
        {
        case cora::Token::Tag::T_PLUS: return *lhs + *rhs;
        case cora::Token::Tag::T_MINUS: return *lhs - *rhs;
        case cora::Token::Tag::T_MUL: return *lhs * *rhs;
        case cora::Token::Tag::T_DIV:
            return *rhs == 0 ? std::nullopt : std::optional<std::int64_t>(*lhs / *rhs);
        default: return std::nullopt;
        }
    }
} // namespace cora::sema
