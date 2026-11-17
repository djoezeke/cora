#ifndef CORA_SEMA_VALUES_H
#define CORA_SEMA_VALUES_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace cora::sema
{

    struct Value
    {
        enum class Tag : uint8_t
        {
            Invalid,
            Integer,
            Floating,
            Boolean,
            String,
            Null
        };

        Tag tag = Tag::Invalid;
        std::int64_t integer = 0;
        double floating = 0.0;
        bool boolean = false;
        std::string string;

        [[nodiscard]] static Value integerValue(std::int64_t value)
        {
            Value result;
            result.tag = Tag::Integer;
            result.integer = value;
            return result;
        }
    };

} // namespace cora::sema

#endif // CORA_SEMA_VALUES_H
