#ifndef CORA_CIR_CIR_H
#define CORA_CIR_CIR_H

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace cora::sema
{

    struct Type
    {
        using Id = uint32_t;
        enum class Tag
        {
        };

        Tag tag;
        Id ret = 0;
        Id pointee = 0;
        std::vector<Id> params;
    };

    struct Types
    {
        /* data */
    };

} // namespace cora::sema

#endif // CORA_CIR_CIR_H
