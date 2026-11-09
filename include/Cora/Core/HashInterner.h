
#include <Cora/Core/Interner.h>
#include <cstdint>

#include <string_view>
#include <vector>

class HashInterner : public Interner
{
public:
    uint32_t intern(std::string_view text);
    std::string_view lookup(uint32_t id);

private:
    std::vector<std::string_view> strings;
};
