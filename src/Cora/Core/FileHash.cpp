#include <Cora/Core/FileHash.h>
#include <Cora/Core/SHA256.h>

#include <array>
#include <fstream>

namespace cora::core
{

    Digest hashBytes(std::string_view bytes) noexcept
    {
        return crypto::SHA256::hash(bytes);
    }

    std::optional<Digest> hashFile(const std::filesystem::path &path,
                                   std::string *error) noexcept
    {
        std::ifstream input(path, std::ios::binary);
        if (!input)
        {
            if (error)
                *error = "unable to open " + path.string();
            return std::nullopt;
        }

        crypto::SHA256 hasher;
        std::array<char, 64 * 1024> buffer{};
        while (input)
        {
            input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
            const std::streamsize count = input.gcount();
            if (count > 0)
                hasher.update(reinterpret_cast<const std::uint8_t *>(buffer.data()),
                              static_cast<std::size_t>(count));
        }

        if (!input.eof())
        {
            if (error)
                *error = "unable to read " + path.string();
            return std::nullopt;
        }
        return hasher.finalize();
    }

} // namespace cora::core
