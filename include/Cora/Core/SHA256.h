#ifndef CORA_CORE_SHA256_H
#define CORA_CORE_SHA256_H

#include <Cora/Core/Digest.h>

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace cora::crypto
{
    class SHA256 final
    {
    public:
        SHA256() noexcept;
        void reset() noexcept;
        void update(const std::uint8_t *data, std::size_t size) noexcept;
        void update(std::string_view data) noexcept;
        [[nodiscard]] cora::core::Digest finalize() const noexcept;
        [[nodiscard]] static cora::core::Digest hash(const std::uint8_t *data,
                                                      std::size_t size) noexcept;
        [[nodiscard]] static cora::core::Digest hash(std::string_view data) noexcept;

    private:
        void transform(const std::uint8_t *block) noexcept;
        std::uint32_t state_[8]{};
        std::uint8_t block_[64]{};
        std::size_t blockSize_ = 0;
        std::uint64_t byteCount_ = 0;
    };
} // namespace cora::crypto

namespace cora { using SHA256 = crypto::SHA256; }

#endif // CORA_CORE_SHA256_H
