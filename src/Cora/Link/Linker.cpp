#include <Cora/Link/Linker.h>

#include <sstream>

namespace cora::link
{

    cora::backend::ObjectBuffer Linker::link(
        const std::vector<cora::backend::ObjectBuffer> &objects,
        const std::string &target,
        const std::vector<std::string> &arguments)
    {
        std::ostringstream output;
        output << "CORA-LINKED-SKELETON-V1\n"
               << "target=" << target << '\n';
        for (const auto &argument : arguments)
            output << "link-arg=" << argument << '\n';
        for (const auto &object : objects)
        {
            output << "object-size=" << object.bytes.size() << '\n';
            output.write(reinterpret_cast<const char *>(object.bytes.data()),
                         static_cast<std::streamsize>(object.bytes.size()));
            output << '\n';
        }

        const std::string text = output.str();
        cora::backend::ObjectBuffer result;
        result.format = "cora-linked-skeleton";
        result.bytes.assign(text.begin(), text.end());
        return result;
    }

} // namespace cora::link
