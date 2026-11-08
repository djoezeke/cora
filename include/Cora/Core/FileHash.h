#ifndef CORA_CORE_FILEHASH_H
#define CORA_CORE_FILEHASH_H

#include <Cora/Core/Digest.h>

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace cora::core
{
    [[nodiscard]] Digest hashBytes(std::string_view bytes) noexcept;
    [[nodiscard]] std::optional<Digest> hashFile(const std::filesystem::path &path,
                                                  std::string *error = nullptr) noexcept;
}

namespace cora
{
    using core::hashBytes;
    using core::hashFile;
}

#endif // CORA_CORE_FILEHASH_H
