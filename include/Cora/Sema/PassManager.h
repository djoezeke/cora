#ifndef CORA_SEMA_PASSMANAGER_H
#define CORA_SEMA_PASSMANAGER_H

#include <Cora\CIR\CIR.h>
#include <Cora\Sema\Sema.h>

#include <functional>
#include <string>
#include <vector>

namespace cora::sema
{
    struct PassResult
    {
        cir::CIR module;
        std::vector<std::string> errors;
        bool succeeded = false;
    };

    class PassManager final
    {
    public:
        using Pass = std::function<void(cir::CIR &, std::vector<std::string> &)>;
        void add(std::string name, Pass pass);
        [[nodiscard]] PassResult run(const cir::CIR &input) const;
    private:
        std::vector<std::pair<std::string, Pass>> passes_;
    };
} // namespace cora::sema

#endif
