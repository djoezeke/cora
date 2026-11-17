#ifndef CORA_SEMA_TYPES_H
#define CORA_SEMA_TYPES_H

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace cora::sema
{

    struct Type
    {
        using Id = uint32_t;
        enum class Tag : uint8_t
        {
            Error,
            Void,
            Bool,
            Char,
            Int,
            UInt,
            Float,
            String,
            Null,
            Pointer,
            Array,
            Function,
            Struct,
            Generic,
        };

        Tag tag = Tag::Error;
        Id ret = 0;
        Id pointee = 0;
        uint64_t length = 0;
        std::vector<Id> params;
        std::string name;

        [[nodiscard]] bool operator==(const Type &other) const noexcept
        {
            return tag == other.tag && ret == other.ret && pointee == other.pointee &&
                   length == other.length && params == other.params && name == other.name;
        }
    };

    class Types final
    {
    public:
        Types();

        [[nodiscard]] Type::Id error() const noexcept { return 0; }
        [[nodiscard]] Type::Id add(Type type);
        [[nodiscard]] const Type &get(Type::Id id) const noexcept;
        [[nodiscard]] Type::Id primitive(Type::Tag tag);
        [[nodiscard]] Type::Id pointer(Type::Id pointee);
        [[nodiscard]] Type::Id array(Type::Id element, uint64_t length);
        [[nodiscard]] Type::Id function(const std::vector<Type::Id> &params,
                                        Type::Id result);
        [[nodiscard]] Type::Id named(Type::Tag tag, std::string name);
        [[nodiscard]] bool compatible(Type::Id left, Type::Id right) const noexcept;
        [[nodiscard]] std::string describe(Type::Id id) const;
        [[nodiscard]] const std::vector<Type> &all() const noexcept { return types_; }

    private:
        std::vector<Type> types_;
        std::unordered_map<std::string, Type::Id> named_;
    };

} // namespace cora::sema

#endif // CORA_SEMA_TYPES_H
