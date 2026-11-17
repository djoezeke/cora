#include <Cora/Sema/TypeChecker.h>

namespace cora::sema
{
    bool TypeChecker::check(const cir::CIR &module, Types &types,
                            std::vector<std::string> &errors) const
    {
        bool ok = true;
        for (const cir::Body &body : module.bodies)
            for (const cir::Param &param : body.params)
                if (param.type_ref == cir::TNull)
                {
                    errors.push_back("parameter has no type in " + body.name);
                    ok = false;
                }
        (void)types;
        return ok;
    }
} // namespace cora::sema
