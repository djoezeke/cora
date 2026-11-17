#include <Cora/Sema/Monomorphizer.h>

namespace cora::sema
{
    cir::CIR Monomorphizer::run(const cir::CIR &module,
                                std::vector<std::string> *instances) const
    {
        cir::CIR result = module;
        if (instances != nullptr)
            instances->clear();
        for (const cir::Body &body : module.bodies)
        {
            if (!body.is_function)
                continue;
            bool generic = false;
            for (const cir::Param &param : body.params)
                if (param.type_ref != cir::TNull)
                    generic = generic ||
                              (param.type_ref < module.types.size() &&
                               module.types[param.type_ref].kind == cir::TypeRefKind::Named &&
                               module.string(module.types[param.type_ref].name) == "T");
            if (generic && instances != nullptr)
                instances->push_back(body.name + "<monomorphized>");
        }
        return result;
    }
} // namespace cora::sema
