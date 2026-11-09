#ifndef CORA_AIR_REF_H
#define CORA_AIR_REF_H

#include <cstdint>

namespace cora::air
{
    struct AirRef
    {
        enum class Tag : uint8_t
        {
        };

        Tag tag;
        uint32_t idx;

        static AirRef inst(uint32_t i);
        static AirRef cval(uint32_t v);
        static AirRef none();

        static bool isComptime() const;
        static bool isRuntime() const;
        static bool isDead() const;
    };

} // namespace cora::air

#endif // CORA_AIR_REF_H
