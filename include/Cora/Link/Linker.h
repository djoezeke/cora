#ifndef CORA_LINK_LINKER_H
#define CORA_LINK_LINKER_H

#include <Cora/Backend/CBackend.h>

#include <string>
#include <vector>

namespace cora::link
{
    class Linker final
    {
    public:
        [[nodiscard]] static cora::backend::ObjectBuffer link(
            const std::vector<cora::backend::ObjectBuffer> &objects,
            const std::string &target,
            const std::vector<std::string> &arguments);
    };
} // namespace cora::link

#endif // CORA_LINK_LINKER_H
