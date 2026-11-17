#include <Cora/Sema/BorrowChecker.h>

#include <unordered_set>

namespace cora::sema
{
    std::vector<BorrowError> BorrowChecker::check(const cir::CIR &module) const
    {
        std::vector<BorrowError> errors;
        for (const cir::Body &body : module.bodies)
        {
            std::unordered_set<std::string> declared;
            for (std::size_t i = 1; i < body.insts.size(); ++i)
            {
                const cir::Inst inst = body.insts[i];
                if (inst.tag == cir::Tag::LetDecl)
                {
                    if (!declared.insert(std::string(module.string(inst.name))).second)
                        errors.push_back({static_cast<cir::IIndex>(i),
                                           "binding is declared more than once"});
                }
                if ((inst.tag == cir::Tag::Assign || inst.tag == cir::Tag::Unary) &&
                    inst.a == cir::INull)
                    errors.push_back({static_cast<cir::IIndex>(i),
                                      "borrowed value has no valid owner"});
            }
        }
        return errors;
    }
} // namespace cora::sema
