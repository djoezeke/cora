#ifndef CORA_DRIVER_COMPILATIONSESSION_H
#define CORA_DRIVER_COMPILATIONSESSION_H

#include <Cora/Cache/CIRCache.h>
#include <Cora/Cache/ObjectCache.h>
#include <Cora/Driver/BuildConfig.h>

namespace cora::driver
{
    class CompilationSession final
    {
    public:
        explicit CompilationSession(BuildConfig config);
        [[nodiscard]] const BuildConfig &config() const noexcept { return config_; }
        [[nodiscard]] cora::cache::CIRCache &cirCache() noexcept { return cirCache_; }
        [[nodiscard]] cora::cache::ObjectCache &objectCache() noexcept { return objectCache_; }
    private:
        BuildConfig config_;
        cora::cache::CIRCache cirCache_;
        cora::cache::ObjectCache objectCache_;
    };
} // namespace cora::driver

#endif // CORA_DRIVER_COMPILATIONSESSION_H
