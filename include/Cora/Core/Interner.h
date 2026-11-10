#ifndef CORA_CORE_INTERNER_H
#define CORA_CORE_INTERNER_H
#include <cstdint>

#include <string_view>

namespace cora::core
{

    class Interner
    {
    public:
        virtual uint32_t intern(std::string_view text) = 0;
        virtual ~Interner() = default;
    };

    class HashInterner : public Interner
    {
    public:
        uint32_t intern(std::string_view text);
        std::string_view lookup(uint32_t id);

    private:
        std::vector<std::string_view> strings;
    };

} // namespace cora::core

#endif // CORA_CORE_INTERNER_H