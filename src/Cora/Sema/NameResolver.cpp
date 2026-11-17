#include <Cora/Sema/NameResolver.h>

namespace cora::sema
{
    bool NameResolver::resolve(const cir::CIR &module, SymbolTable &symbols,
                               std::vector<std::string> &errors) const
    {
        bool ok = true;
        for (const cir::Body &body : module.bodies)
        {
            if (!body.name.empty() &&
                !symbols.declare({body.name, Symbol::Kind::Function, 0, false, false}))
            {
                errors.push_back("duplicate declaration: " + body.name);
                ok = false;
            }
        }
        return ok;
    }
} // namespace cora::sema
