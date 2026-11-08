#ifndef CORA_CACHE_CACHEKEY_H
#define CORA_CACHE_CACHEKEY_H

#include <Cora/Core/Digest.h>

namespace cora::cache
{

    using CacheKey = cora::core::Digest;
    using CacheKeyHasher = cora::core::DigestHasher;

} // namespace cora::cache

#endif // CORA_CACHE_CACHEKEY_H
