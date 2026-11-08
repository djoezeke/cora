#ifndef CORA_DRIVER_DRIVER_H
#define CORA_DRIVER_DRIVER_H

#include <iosfwd>

namespace cora::driver
{
    class Driver final
    {
    public:
        int run(int argc, const char *const *argv,
                std::ostream &out, std::ostream &err) const;
    };
} // namespace cora::driver

#endif // CORA_DRIVER_DRIVER_H
