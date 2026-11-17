#include <Cora/Sema/Types.h>

#include <sstream>

namespace cora::sema
{
    Types::Types()
    {
        types_.push_back(Type{});
        for (const auto tag : {Type::Tag::Void, Type::Tag::Bool, Type::Tag::Char,
                               Type::Tag::Int, Type::Tag::UInt, Type::Tag::Float,
                               Type::Tag::String, Type::Tag::Null})
            (void)primitive(tag);
    }

    Type::Id Types::add(Type type)
    {
        types_.push_back(std::move(type));
        return static_cast<Type::Id>(types_.size() - 1);
    }

    const Type &Types::get(Type::Id id) const noexcept
    {
        static const Type invalid{};
        return id < types_.size() ? types_[id] : invalid;
    }

    Type::Id Types::primitive(Type::Tag tag)
    {
        for (Type::Id id = 1; id < types_.size(); ++id)
            if (types_[id].tag == tag)
                return id;
        Type type;
        type.tag = tag;
        return add(std::move(type));
    }

    Type::Id Types::pointer(Type::Id pointee)
    {
        for (Type::Id id = 1; id < types_.size(); ++id)
            if (types_[id].tag == Type::Tag::Pointer && types_[id].pointee == pointee)
                return id;
        Type type;
        type.tag = Type::Tag::Pointer;
        type.pointee = pointee;
        return add(std::move(type));
    }

    Type::Id Types::array(Type::Id element, uint64_t length)
    {
        Type type;
        type.tag = Type::Tag::Array;
        type.pointee = element;
        type.length = length;
        return add(std::move(type));
    }

    Type::Id Types::function(const std::vector<Type::Id> &params, Type::Id result)
    {
        Type type;
        type.tag = Type::Tag::Function;
        type.params = params;
        type.ret = result;
        return add(std::move(type));
    }

    Type::Id Types::named(Type::Tag tag, std::string name)
    {
        const auto key = std::to_string(static_cast<int>(tag)) + ":" + name;
        const auto it = named_.find(key);
        if (it != named_.end())
            return it->second;
        Type type;
        type.tag = tag;
        type.name = std::move(name);
        const Type::Id id = add(std::move(type));
        named_.emplace(key, id);
        return id;
    }

    bool Types::compatible(Type::Id left, Type::Id right) const noexcept
    {
        if (left == right || left == error() || right == error())
            return true;
        const Type &a = get(left);
        const Type &b = get(right);
        return (a.tag == Type::Tag::Int && b.tag == Type::Tag::UInt) ||
               (a.tag == Type::Tag::UInt && b.tag == Type::Tag::Int) ||
               (a.tag == Type::Tag::Null &&
                (b.tag == Type::Tag::Pointer || b.tag == Type::Tag::String));
    }

    std::string Types::describe(Type::Id id) const
    {
        const Type &type = get(id);
        if (!type.name.empty())
            return type.name;
        switch (type.tag)
        {
        case Type::Tag::Void: return "void";
        case Type::Tag::Bool: return "bool";
        case Type::Tag::Char: return "char";
        case Type::Tag::Int: return "i64";
        case Type::Tag::UInt: return "u64";
        case Type::Tag::Float: return "f64";
        case Type::Tag::String: return "string";
        case Type::Tag::Null: return "null";
        case Type::Tag::Pointer: return "*" + describe(type.pointee);
        case Type::Tag::Array: return "[" + std::to_string(type.length) + "]" +
                                       describe(type.pointee);
        case Type::Tag::Function: return "fn";
        case Type::Tag::Struct: return "struct";
        case Type::Tag::Generic: return "generic";
        default: return "<error>";
        }
    }
} // namespace cora::sema
