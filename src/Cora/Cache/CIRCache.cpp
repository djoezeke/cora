#include <Cora/Cache/CIRCache.h>

#include <utility>

namespace cora::cache
{

    CIRCache::CIRCache(std::filesystem::path root, CachePolicy policy)
        : storage_(std::move(root), policy)
    {}

    CIRLookup CIRCache::lookup(const cora::cir::CIRInput &input) const
    {
        CIRLookup result;
        result.key = input.key();
        result.path = storage_.pathFor(CacheNamespace::CIR, result.key, input.sourceFileId, ".cir");

        const CacheLookup file = storage_.lookup(CacheNamespace::CIR, result.key,
                                                 input.sourceFileId, ".cir");
        result.reason = file.reason;
        if (!file.hit)
            return result;

        std::string error;
        const auto bytes = storage_.read(file.path, &error);
        if (!bytes)
        {
            result.reason = error;
            return result;
        }
        result.artifact = cora::cir::CIRArtifact::deserialize(*bytes, &error);
        if (!result.artifact || result.artifact->key != result.key ||
            result.artifact->input.sourceDigest != input.sourceDigest ||
            result.artifact->input.sourceFileId != input.sourceFileId ||
            result.artifact->input.moduleId != input.moduleId ||
            result.artifact->module.ast == nullptr)
        {
            result.artifact.reset();
            result.reason = error.empty() ? "invalid CIR cache artifact" : error;
            return result;
        }
        result.hit = true;
        result.reason = "cache hit";
        return result;
    }

    CacheWrite CIRCache::publish(const cora::cir::CIRArtifact &artifact)
    {
        const std::vector<std::uint8_t> bytes = artifact.serialize();
        return storage_.put(CacheNamespace::CIR, artifact.key, artifact.input.sourceFileId,
                            ".cir", bytes);
    }

} // namespace cora::cache
