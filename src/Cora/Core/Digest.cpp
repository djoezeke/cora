#include <Cora/Core/Digest.h>

#include <cctype>

namespace cora::core
{

    bool Digest::isZero() const noexcept
    {
        for (const std::uint8_t byte : bytes)
            if (byte != 0)
                return false;
        return true;
    }

    std::string Digest::hex() const
    {
        static constexpr char digits[] = "0123456789abcdef";
        std::string result;
        result.reserve(Size * 2);
        for (const std::uint8_t byte : bytes)
        {
            result.push_back(digits[(byte >> 4) & 0x0f]);
            result.push_back(digits[byte & 0x0f]);
        }
        return result;
    }

    std::optional<Digest> Digest::fromHex(std::string_view text)
    {
        if (text.size() != Size * 2)
            return std::nullopt;

        auto value = [](char c) -> int {
            if (c >= '0' && c <= '9')
                return c - '0';
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            return -1;
        };

        Digest result;
        for (std::size_t i = 0; i < Size; ++i)
        {
            const int high = value(text[i * 2]);
            const int low = value(text[i * 2 + 1]);
            if (high < 0 || low < 0)
                return std::nullopt;
            result.bytes[i] = static_cast<std::uint8_t>((high << 4) | low);
        }
        return result;
    }

    std::size_t DigestHasher::operator()(const Digest &digest) const noexcept
    {
        std::size_t value = 1469598103934665603ull;
        for (const std::uint8_t byte : digest.bytes)
        {
            value ^= byte;
            value *= 1099511628211ull;
        }
        return value;
    }

} // namespace cora::core
