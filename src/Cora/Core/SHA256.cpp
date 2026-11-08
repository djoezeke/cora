#include <Cora/Core/SHA256.h>

#include <algorithm>
#include <cstring>
#include <iterator>

namespace cora::crypto
{
    namespace
    {
        constexpr std::uint32_t RoundConstants[64] = {
            0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u,
            0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
            0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
            0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
            0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
            0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
            0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u,
            0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
            0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
            0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
            0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u,
            0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
            0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u,
            0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
            0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
            0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

        constexpr std::uint32_t InitialState[8] = {
            0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
            0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};

        constexpr std::uint32_t rotateRight(std::uint32_t value, unsigned amount) noexcept
        {
            return (value >> amount) | (value << (32u - amount));
        }

        constexpr std::uint32_t choose(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept
        {
            return (x & y) ^ (~x & z);
        }

        constexpr std::uint32_t majority(std::uint32_t x, std::uint32_t y, std::uint32_t z) noexcept
        {
            return (x & y) ^ (x & z) ^ (y & z);
        }

        constexpr std::uint32_t bigSigma0(std::uint32_t x) noexcept
        {
            return rotateRight(x, 2) ^ rotateRight(x, 13) ^ rotateRight(x, 22);
        }

        constexpr std::uint32_t bigSigma1(std::uint32_t x) noexcept
        {
            return rotateRight(x, 6) ^ rotateRight(x, 11) ^ rotateRight(x, 25);
        }

        constexpr std::uint32_t smallSigma0(std::uint32_t x) noexcept
        {
            return rotateRight(x, 7) ^ rotateRight(x, 18) ^ (x >> 3);
        }

        constexpr std::uint32_t smallSigma1(std::uint32_t x) noexcept
        {
            return rotateRight(x, 17) ^ rotateRight(x, 19) ^ (x >> 10);
        }

        std::uint32_t readBigEndian(const std::uint8_t *data) noexcept
        {
            return (static_cast<std::uint32_t>(data[0]) << 24) |
                   (static_cast<std::uint32_t>(data[1]) << 16) |
                   (static_cast<std::uint32_t>(data[2]) << 8) |
                   static_cast<std::uint32_t>(data[3]);
        }

        void writeBigEndian(std::uint8_t *data, std::uint32_t value) noexcept
        {
            data[0] = static_cast<std::uint8_t>(value >> 24);
            data[1] = static_cast<std::uint8_t>(value >> 16);
            data[2] = static_cast<std::uint8_t>(value >> 8);
            data[3] = static_cast<std::uint8_t>(value);
        }
    } // namespace

    SHA256::SHA256() noexcept
    {
        reset();
    }

    void SHA256::reset() noexcept
    {
        std::copy(std::begin(InitialState), std::end(InitialState), std::begin(state_));
        std::fill(std::begin(block_), std::end(block_), 0);
        blockSize_ = 0;
        byteCount_ = 0;
    }

    void SHA256::update(const std::uint8_t *data, std::size_t size) noexcept
    {
        if (!data || size == 0)
            return;

        byteCount_ += size;
        while (size != 0)
        {
            const std::size_t available = sizeof(block_) - blockSize_;
            const std::size_t amount = std::min(available, size);
            std::memcpy(block_ + blockSize_, data, amount);
            blockSize_ += amount;
            data += amount;
            size -= amount;

            if (blockSize_ == sizeof(block_))
            {
                transform(block_);
                blockSize_ = 0;
            }
        }
    }

    void SHA256::update(std::string_view data) noexcept
    {
        update(reinterpret_cast<const std::uint8_t *>(data.data()), data.size());
    }

    cora::core::Digest SHA256::finalize() const noexcept
    {
        SHA256 copy = *this;
        const std::uint64_t bitCount = byteCount_ * 8u;

        const std::size_t padding = copy.blockSize_ < 56
                                        ? 56 - copy.blockSize_
                                        : 120 - copy.blockSize_;
        std::uint8_t pad[128]{};
        pad[0] = 0x80;
        copy.update(pad, padding);

        std::uint8_t length[8]{};
        for (unsigned i = 0; i < 8; ++i)
            length[7 - i] = static_cast<std::uint8_t>(bitCount >> (i * 8));
        copy.update(length, sizeof(length));

        cora::core::Digest result;
        for (unsigned i = 0; i < 8; ++i)
            writeBigEndian(result.bytes.data() + i * 4, copy.state_[i]);
        return result;
    }

    cora::core::Digest SHA256::hash(const std::uint8_t *data, std::size_t size) noexcept
    {
        SHA256 hasher;
        hasher.update(data, size);
        return hasher.finalize();
    }

    cora::core::Digest SHA256::hash(std::string_view data) noexcept
    {
        SHA256 hasher;
        hasher.update(data);
        return hasher.finalize();
    }

    void SHA256::transform(const std::uint8_t *block) noexcept
    {
        std::uint32_t schedule[64]{};
        for (unsigned i = 0; i < 16; ++i)
            schedule[i] = readBigEndian(block + i * 4);
        for (unsigned i = 16; i < 64; ++i)
            schedule[i] = smallSigma1(schedule[i - 2]) + schedule[i - 7] +
                          smallSigma0(schedule[i - 15]) + schedule[i - 16];

        std::uint32_t a = state_[0];
        std::uint32_t b = state_[1];
        std::uint32_t c = state_[2];
        std::uint32_t d = state_[3];
        std::uint32_t e = state_[4];
        std::uint32_t f = state_[5];
        std::uint32_t g = state_[6];
        std::uint32_t h = state_[7];

        for (unsigned i = 0; i < 64; ++i)
        {
            const std::uint32_t t1 = h + bigSigma1(e) + choose(e, f, g) +
                                     RoundConstants[i] + schedule[i];
            const std::uint32_t t2 = bigSigma0(a) + majority(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        state_[0] += a;
        state_[1] += b;
        state_[2] += c;
        state_[3] += d;
        state_[4] += e;
        state_[5] += f;
        state_[6] += g;
        state_[7] += h;
    }

} // namespace cora::crypto
