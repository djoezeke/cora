#ifndef CORA_CACHE_CONTENTADDRESSEDSTORAGE_H
#define CORA_CACHE_CONTENTADDRESSEDSTORAGE_H

#include <Cora/Core/Digest.h>

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cora::cache
{
    enum class CacheNamespace { CIR, Object };
    enum class CachePolicy { Enabled, ReadOnly, Disabled };
    struct CacheLookup
    {
        bool hit = false;
        cora::core::Digest key;
        std::filesystem::path path;
        std::string reason;
    };
    struct CacheWrite
    {
        bool published = false;
        std::filesystem::path path;
        std::string error;
    };
    class ContentAddressedStorage final
    {
    public:
        explicit ContentAddressedStorage(std::filesystem::path root,
                                         CachePolicy policy = CachePolicy::Enabled);
        [[nodiscard]] const std::filesystem::path &root() const noexcept { return root_; }
        [[nodiscard]] CachePolicy policy() const noexcept { return policy_; }
        [[nodiscard]] static std::string namespaceName(CacheNamespace name);
        [[nodiscard]] static std::string safeLabel(std::string_view label);
        [[nodiscard]] std::filesystem::path pathFor(CacheNamespace name,
                                                     const cora::core::Digest &key,
                                                     std::string_view label,
                                                     std::string_view extension) const;
        [[nodiscard]] CacheLookup lookup(CacheNamespace name,
                                         const cora::core::Digest &key,
                                         std::string_view label,
                                         std::string_view extension) const;
        [[nodiscard]] std::optional<std::vector<std::uint8_t>> read(
            const std::filesystem::path &path, std::string *error = nullptr) const;
        [[nodiscard]] CacheWrite put(CacheNamespace name, const cora::core::Digest &key,
                                     std::string_view label, std::string_view extension,
                                     const std::vector<std::uint8_t> &bytes);
        [[nodiscard]] CacheWrite putFile(CacheNamespace name, const cora::core::Digest &key,
                                         std::string_view label, std::string_view extension,
                                         const std::filesystem::path &source);
        [[nodiscard]] std::vector<std::filesystem::path> list(CacheNamespace name) const;
        bool clean(CacheNamespace name, std::string *error = nullptr);
    private:
        [[nodiscard]] std::filesystem::path temporaryPath() const;
        [[nodiscard]] CacheWrite publishFile(const std::filesystem::path &destination,
                                              const std::filesystem::path &source);
        std::filesystem::path root_;
        CachePolicy policy_;
    };
} // namespace cora::cache

#endif // CORA_CACHE_CONTENTADDRESSEDSTORAGE_H
