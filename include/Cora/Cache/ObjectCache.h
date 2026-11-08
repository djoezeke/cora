#ifndef CORA_CACHE_OBJECTCACHE_H
#define CORA_CACHE_OBJECTCACHE_H

#include <Cora/Cache/ContentAddressedStorage.h>
#include <Cora/CodeGen/ObjectInput.h>

#include <filesystem>
#include <vector>

namespace cora::cache
{
    class ObjectCache final
    {
    public:
        explicit ObjectCache(std::filesystem::path root,
                             CachePolicy policy = CachePolicy::Enabled);
        [[nodiscard]] CacheLookup lookup(const cora::codegen::ObjectInput &input) const;
        [[nodiscard]] CacheWrite publish(const cora::codegen::ObjectInput &input,
                                         const std::vector<std::uint8_t> &objectBytes);
        [[nodiscard]] const ContentAddressedStorage &storage() const noexcept { return storage_; }
    private:
        ContentAddressedStorage storage_;
    };
} // namespace cora::cache

#endif // CORA_CACHE_OBJECTCACHE_H
