
#include <cstdint>

#include <string_view>

class Interner
{
public:
    virtual uint32_t intern(std::string_view text) = 0;
    virtual ~Interner() = default;
};
