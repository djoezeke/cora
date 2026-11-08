#include <Cora/Cache/ObjectCache.h>

#include <utility>

namespace cora::cache
{

    ObjectCache::ObjectCache(std::filesystem::path root, CachePolicy policy)
        : storage_(std::move(root), policy)
    {}

    CacheLookup ObjectCache::lookup(const cora::codegen::ObjectInput &input) const
    {
        return storage_.lookup(CacheNamespace::Object, input.key(), input.codegenUnitId, ".o");
    }

    CacheWrite ObjectCache::publish(const cora::codegen::ObjectInput &input,
                                    const std::vector<std::uint8_t> &objectBytes)
    {
        return storage_.put(CacheNamespace::Object, input.key(), input.codegenUnitId, ".o", objectBytes);
    }

} // namespace cora::cache
