#include <Cora/Cache/ContentAddressedStorage.h>

#include <atomic>
#include <cctype>
#include <chrono>
#include <fstream>
#include <utility>

namespace cora::cache
{
    namespace
    {
        std::atomic<std::uint64_t> nextTemporaryId{0};

        bool isSafeLabelCharacter(unsigned char character) noexcept
        {
            return std::isalnum(character) != 0 || character == '-' || character == '_' || character == '.';
        }
    } // namespace

    ContentAddressedStorage::ContentAddressedStorage(std::filesystem::path root,
                                                     CachePolicy policy)
        : root_(std::move(root)), policy_(policy)
    {}

    std::string ContentAddressedStorage::namespaceName(CacheNamespace name)
    {
        return name == CacheNamespace::CIR ? "h" : "o";
    }

    std::string ContentAddressedStorage::safeLabel(std::string_view label)
    {
        std::string result;
        result.reserve(label.size());
        for (const unsigned char character : label)
            result.push_back(isSafeLabelCharacter(character) ? static_cast<char>(character) : '_');
        return result.empty() ? "artifact" : result;
    }

    std::filesystem::path ContentAddressedStorage::pathFor(CacheNamespace name,
                                                            const cora::core::Digest &key,
                                                            std::string_view label,
                                                            std::string_view extension) const
    {
        return root_ / namespaceName(name) / key.hex() /
               (safeLabel(label) + std::string(extension));
    }

    CacheLookup ContentAddressedStorage::lookup(CacheNamespace name,
                                                const cora::core::Digest &key,
                                                std::string_view label,
                                                std::string_view extension) const
    {
        CacheLookup result;
        result.key = key;
        result.path = pathFor(name, key, label, extension);
        if (policy_ == CachePolicy::Disabled)
        {
            result.reason = "cache disabled";
            return result;
        }

        std::error_code error;
        if (!std::filesystem::is_regular_file(result.path, error))
        {
            result.reason = error && error != std::errc::no_such_file_or_directory
                                ? "cache lookup error: " + error.message()
                                : "cache miss";
            return result;
        }
        result.hit = true;
        result.reason = "cache hit";
        return result;
    }

    std::optional<std::vector<std::uint8_t>> ContentAddressedStorage::read(
        const std::filesystem::path &path,
        std::string *error) const
    {
        std::ifstream input(path, std::ios::binary | std::ios::ate);
        if (!input)
        {
            if (error)
                *error = "unable to open cache artifact " + path.string();
            return std::nullopt;
        }

        const std::streampos end = input.tellg();
        if (end < 0)
        {
            if (error)
                *error = "unable to size cache artifact " + path.string();
            return std::nullopt;
        }
        std::vector<std::uint8_t> bytes(static_cast<std::size_t>(end));
        input.seekg(0);
        if (!bytes.empty())
            input.read(reinterpret_cast<char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        if (!input && !input.eof())
        {
            if (error)
                *error = "unable to read cache artifact " + path.string();
            return std::nullopt;
        }
        return bytes;
    }

    std::filesystem::path ContentAddressedStorage::temporaryPath() const
    {
        const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto id = nextTemporaryId.fetch_add(1, std::memory_order_relaxed);
        return root_ / "tmp" / ("artifact-" + std::to_string(now) + "-" + std::to_string(id) + ".partial");
    }

    CacheWrite ContentAddressedStorage::publishFile(const std::filesystem::path &destination,
                                                    const std::filesystem::path &source)
    {
        CacheWrite result;
        result.path = destination;

        if (policy_ != CachePolicy::Enabled)
        {
            result.error = policy_ == CachePolicy::ReadOnly ? "cache is read-only" : "cache is disabled";
            return result;
        }

        std::error_code error;
        std::filesystem::create_directories(destination.parent_path(), error);
        if (error)
        {
            result.error = "unable to create cache directory: " + error.message();
            return result;
        }

        if (std::filesystem::exists(destination, error))
        {
            result.published = !error;
            if (error)
                result.error = "unable to inspect existing cache artifact: " + error.message();
            return result;
        }

        std::filesystem::rename(source, destination, error);
        if (error)
        {
            // Another worker may have won the race between exists() and
            // rename().  The destination remains the authoritative artifact.
            if (std::filesystem::exists(destination))
            {
                std::filesystem::remove(source);
                result.published = true;
                return result;
            }
            result.error = "unable to publish cache artifact: " + error.message();
            std::filesystem::remove(source);
            return result;
        }

        result.published = true;
        return result;
    }

    CacheWrite ContentAddressedStorage::put(CacheNamespace name,
                                            const cora::core::Digest &key,
                                            std::string_view label,
                                            std::string_view extension,
                                            const std::vector<std::uint8_t> &bytes)
    {
        CacheWrite result;
        result.path = pathFor(name, key, label, extension);
        if (policy_ != CachePolicy::Enabled)
        {
            result.error = policy_ == CachePolicy::ReadOnly ? "cache is read-only" : "cache is disabled";
            return result;
        }

        const std::filesystem::path temporary = temporaryPath();
        std::error_code error;
        std::filesystem::create_directories(temporary.parent_path(), error);
        if (error)
        {
            result.error = "unable to create cache temporary directory: " + error.message();
            return result;
        }

        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            if (!output)
            {
                result.error = "unable to create cache temporary file";
                return result;
            }
            if (!bytes.empty())
                output.write(reinterpret_cast<const char *>(bytes.data()),
                             static_cast<std::streamsize>(bytes.size()));
            output.flush();
            if (!output)
            {
                result.error = "unable to write cache temporary file";
                std::filesystem::remove(temporary);
                return result;
            }
        }

        return publishFile(result.path, temporary);
    }

    CacheWrite ContentAddressedStorage::putFile(CacheNamespace name,
                                                const cora::core::Digest &key,
                                                std::string_view label,
                                                std::string_view extension,
                                                const std::filesystem::path &source)
    {
        CacheWrite result;
        result.path = pathFor(name, key, label, extension);
        if (policy_ != CachePolicy::Enabled)
        {
            result.error = policy_ == CachePolicy::ReadOnly ? "cache is read-only" : "cache is disabled";
            return result;
        }

        const std::filesystem::path temporary = temporaryPath();
        std::error_code error;
        std::filesystem::create_directories(temporary.parent_path(), error);
        if (error)
        {
            result.error = "unable to create cache temporary directory: " + error.message();
            return result;
        }

        std::ifstream input(source, std::ios::binary);
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!input || !output)
        {
            result.error = "unable to copy cache artifact";
            std::filesystem::remove(temporary);
            return result;
        }
        output << input.rdbuf();
        output.flush();
        if (!output)
        {
            result.error = "unable to write cache artifact";
            std::filesystem::remove(temporary);
            return result;
        }
        output.close();
        input.close();
        return publishFile(result.path, temporary);
    }

    std::vector<std::filesystem::path> ContentAddressedStorage::list(CacheNamespace name) const
    {
        std::vector<std::filesystem::path> result;
        const std::filesystem::path directory = root_ / namespaceName(name);
        std::error_code error;
        if (!std::filesystem::is_directory(directory, error))
            return result;

        for (const auto &keyDirectory : std::filesystem::directory_iterator(directory, error))
        {
            if (error)
                break;
            if (!keyDirectory.is_directory())
                continue;
            for (const auto &artifact : std::filesystem::directory_iterator(keyDirectory.path(), error))
            {
                if (error)
                    break;
                if (artifact.is_regular_file())
                    result.push_back(artifact.path());
            }
        }
        return result;
    }

    bool ContentAddressedStorage::clean(CacheNamespace name, std::string *error)
    {
        if (policy_ == CachePolicy::ReadOnly || policy_ == CachePolicy::Disabled)
        {
            if (error)
                *error = "cache policy does not permit cleanup";
            return false;
        }

        std::error_code filesystemError;
        std::filesystem::remove_all(root_ / namespaceName(name), filesystemError);
        if (filesystemError)
        {
            if (error)
                *error = "unable to clean cache: " + filesystemError.message();
            return false;
        }
        return true;
    }

} // namespace cora::cache
