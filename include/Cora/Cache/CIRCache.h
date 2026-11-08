#ifndef CORA_CACHE_CIRCACHE_H
#define CORA_CACHE_CIRCACHE_H

#include <Cora/Cache/ContentAddressedStorage.h>
#include <Cora/CIR/CIR.h>

#include <filesystem>
#include <optional>
#include <string>

namespace cora::cache
{
    struct CIRLookup
    {
        bool hit = false;
        cora::core::Digest key;
        std::filesystem::path path;
        std::string reason;
        std::optional<cora::cir::CIRArtifact> artifact;
    };
    class CIRCache final
    {
    public:
        explicit CIRCache(std::filesystem::path root,
                          CachePolicy policy = CachePolicy::Enabled);
        [[nodiscard]] CIRLookup lookup(const cora::cir::CIRInput &input) const;
        [[nodiscard]] CacheWrite publish(const cora::cir::CIRArtifact &artifact);
        [[nodiscard]] const ContentAddressedStorage &storage() const noexcept { return storage_; }
    private:
        ContentAddressedStorage storage_;
    };
} // namespace cora::cache

#endif // CORA_CACHE_CIRCACHE_H
