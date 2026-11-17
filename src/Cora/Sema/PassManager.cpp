#include <Cora/Sema/PassManager.h>

namespace cora::sema
{
    void PassManager::add(std::string name, Pass pass)
    {
        passes_.emplace_back(std::move(name), std::move(pass));
    }

    PassResult PassManager::run(const cir::CIR &input) const
    {
        PassResult result{input, {}, true};
        for (const auto &pass : passes_)
        {
            if (!result.succeeded)
                break;
            pass.second(result.module, result.errors);
            result.succeeded = result.errors.empty();
        }
        return result;
    }
} // namespace cora::sema
