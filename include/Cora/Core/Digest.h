#ifndef CORA_CORE_DIGEST_H
#define CORA_CORE_DIGEST_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace cora::core
{
    struct Digest
    {
        static constexpr std::size_t Size = 32;
        std::array<std::uint8_t, Size> bytes{};

        [[nodiscard]] bool isZero() const noexcept;
        [[nodiscard]] std::string hex() const;
        static std::optional<Digest> fromHex(std::string_view text);

        friend bool operator==(const Digest &left, const Digest &right) noexcept
        { return left.bytes == right.bytes; }
        friend bool operator!=(const Digest &left, const Digest &right) noexcept
        { return !(left == right); }
        friend bool operator<(const Digest &left, const Digest &right) noexcept
        { return left.bytes < right.bytes; }
    };

    struct DigestHasher
    {
        std::size_t operator()(const Digest &digest) const noexcept;
    };
} // namespace cora::core

namespace cora
{
    using Digest = core::Digest;
    using DigestHasher = core::DigestHasher;
}

#endif // CORA_CORE_DIGEST_H
