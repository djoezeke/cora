#ifndef CORA_CIR_CIR_H
#define CORA_CIR_CIR_H

#include <Cora/CIR/Inst.h>
#include <Cora/Core/Digest.h>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cora::cir
{
    /// The complete CIR for one translation unit.
    struct CIR
    {
        std::vector<Body> bodies;
        std::vector<std::string> strings;
        std::vector<TypeRefInfo> types;
        std::vector<std::string> errors;

        std::size_t token_count = 0;
        std::size_t line_count = 0;

        [[nodiscard]] std::uint32_t internString(std::string_view text)
        {
            for (std::size_t index = 0; index < strings.size(); ++index)
            {
                if (strings[index] == text)
                    return static_cast<std::uint32_t>(index);
            }
            strings.emplace_back(text);
            return static_cast<std::uint32_t>(strings.size() - 1);
        }

        [[nodiscard]] std::string_view string(std::uint32_t index) const noexcept
        {
            return index < strings.size() ? std::string_view(strings[index])
                                          : std::string_view{};
        }

        [[nodiscard]] TIndex addType(TypeRefInfo type)
        {
            const TIndex at = static_cast<TIndex>(types.size());
            types.push_back(std::move(type));
            return at;
        }

        [[nodiscard]] BIndex addBody(std::string name)
        {
            const BIndex at = static_cast<BIndex>(bodies.size());
            bodies.push_back(Body{});
            Body &body = bodies.back();
            body.name = std::move(name);
            body.index = at;
            return at;
        }

        [[nodiscard]] ListView extraList(const Body &body, EIndex at,
                                         std::uint32_t count) const noexcept
        {
            if (at == ENull || count == 0 || at >= body.extra.size())
                return {};
            const std::size_t available = body.extra.size() - at;
            const std::size_t actual =
                count < available ? static_cast<std::size_t>(count) : available;
            return ListView{body.extra.data() + at, actual};
        }

        [[nodiscard]] ListView extraList(const Body &body, const Inst &inst) const noexcept
        {
            return extraList(body, inst.extra, inst.extra_count);
        }

        [[nodiscard]] const Inst &inst(const Body &body, IIndex index) const noexcept
        {
            static const Inst empty{};
            return index < body.insts.size() ? body.insts[index] : empty;
        }

        void dump(std::ostream &out) const;
    };

    /// Everything that influences a CIR artifact. Keeping this explicit makes
    /// cache keys stable and diagnostics reproducible.
    struct CIRInput
    {
        std::string frontendVersion = "cora-frontend-v2";
        std::string sourceFileId;
        std::string moduleId;
        core::Digest sourceDigest;
        std::string frontendOptions;
        std::vector<core::Digest> generatedInputDigests;

        [[nodiscard]] std::string canonical() const;
        [[nodiscard]] core::Digest key() const;
    };

    /// A cacheable compilation result.
    ///
    /// The CIR itself is kept in memory for the current compilation. Text
    /// outputs are serialized so a cache hit can skip the whole front end.
    struct CIRArtifact
    {
        core::Digest key;
        CIRInput input;
        core::Digest publicInterfaceDigest;
        CIR cir;

        std::string source;
        std::string tokensDump;
        std::string astDump;
        std::string cirDump;
        std::string airDump;
        std::string cSource;
        std::vector<std::string> diagnostics;
        bool succeeded = false;

        [[nodiscard]] std::vector<std::uint8_t> serialize() const;
        [[nodiscard]] static std::optional<CIRArtifact> deserialize(
            const std::vector<std::uint8_t> &bytes, std::string *error = nullptr);
    };

} // namespace cora::cir

#endif // CORA_CIR_CIR_H
