#ifndef CORA_CIR_INST_H
#define CORA_CIR_INST_H

#include <Cora/Core/SourceLocation.h>
#include <Cora/Lexer/Token.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace cora::cir
{
    /// CIR uses 1-based instruction references. Index 0 is the null value.
    using IIndex = std::uint32_t;
    using EIndex = std::uint32_t;
    using BIndex = std::uint32_t;
    using TIndex = std::uint32_t;

    static constexpr IIndex INull = 0;
    static constexpr EIndex ENull = 0;
    static constexpr BIndex BNull = 0xFFFFFFFFu;
    static constexpr TIndex TNull = 0xFFFFFFFFu;

    /// Untyped, structured intermediate representation.
    ///
    /// Expressions and statements are stored in a flat instruction list and
    /// refer to children by index. Type syntax is kept in a small side table
    /// (`TypeRefInfo`) so Sema can build the real type graph without depending
    /// on AST node indices.
    enum class Tag : std::uint8_t
    {
        Invalid = 0,

        // Declarations
        StructDecl,
        FnDecl,
        LetDecl,
        TypeAlias,
        Import,

        // Statements
        Block,
        ExprStmt,
        If,
        While,
        ForC,
        ForRange,
        Return,
        Break,
        Continue,
        Defer,

        // Expressions
        Identifier,
        IntLit,
        FloatLit,
        StringLit,
        CharLit,
        BoolLit,
        NullLit,
        Unary,
        Binary,
        Assign,
        Call,
        FieldAccess,
        Index,
        ArrayInit,
        StructInit,
        Paren,

        // Recovery
        Malformed,
    };

    [[nodiscard]] const char *tagName(Tag tag) noexcept;

    struct Inst
    {
        Tag tag = Tag::Invalid;

        /// Four generic operand slots. Their meaning is determined by `tag`.
        /// a/b/c are generally instruction references; d is a fourth operand.
        IIndex a = INull;
        IIndex b = INull;
        IIndex c = INull;
        IIndex d = INull;

        /// The operator token for Unary/Binary/Assign.
        Token::Tag op = Token::Tag::T_EOF;

        /// Literal payloads.
        std::int64_t int_value = 0;
        double float_value = 0.0;
        bool bool_value = false;

        /// Name/field/string index into CIR::strings.
        std::uint32_t name = TNull;

        /// Type syntax reference into CIR::types.
        TIndex type_ref = TNull;

        /// Extra list: [count][items...] stored in CIR::extra.
        EIndex extra = ENull;
        std::uint32_t extra_count = 0;

        /// Declaration flags / prefix-vs-postfix marker.
        std::uint32_t flags = 0;

        core::SourceSpan span{};
    };

    /// Declaration flags.
    static constexpr std::uint32_t CIR_FLAG_MUTABLE = 1u << 0;
    static constexpr std::uint32_t CIR_FLAG_CONST = 1u << 1;
    static constexpr std::uint32_t CIR_FLAG_PUBLIC = 1u << 2;
    static constexpr std::uint32_t CIR_FLAG_EXTERN = 1u << 3;
    static constexpr std::uint32_t CIR_FLAG_POSTFIX = 1u << 4;
    static constexpr std::uint32_t CIR_FLAG_BARE_RETURN = 1u << 5;

    struct Param
    {
        std::uint32_t name = TNull;
        TIndex type_ref = TNull;
        core::SourceSpan span{};
    };

    enum class TypeRefKind : std::uint8_t
    {
        Named,
        Array,
        Pointer,
        Function,
    };

    /// Syntactic type reference. Sema interns these into semantic TypeIds.
    struct TypeRefInfo
    {
        TypeRefKind kind = TypeRefKind::Named;
        std::uint32_t name = TNull; // Named
        TIndex element = TNull;     // Array/Pointer
        std::uint64_t length = 0;   // Array
        std::vector<TIndex> params; // Function parameters
        TIndex result = TNull;      // Function result
        core::SourceSpan span{};
    };

    struct Body
    {
        std::string name;
        std::vector<Inst> insts{Inst{}}; // index 0 reserved for INull
        std::vector<std::uint32_t> extra{ENull};
        std::vector<Param> params;
        std::vector<IIndex> roots;
        BIndex index = BNull;
        bool is_function = false;
        bool is_extern = false;

        [[nodiscard]] IIndex add(Inst inst)
        {
            const IIndex at = static_cast<IIndex>(insts.size());
            insts.push_back(inst);
            return at;
        }

        [[nodiscard]] EIndex addList(const std::vector<std::uint32_t> &items)
        {
            if (items.empty())
                return ENull;
            const EIndex at = static_cast<EIndex>(extra.size());
            extra.push_back(static_cast<std::uint32_t>(items.size()));
            extra.insert(extra.end(), items.begin(), items.end());
            return at;
        }
    };

    /// Non-owning view of an extra list.
    struct ListView
    {
        const std::uint32_t *items = nullptr;
        std::size_t count = 0;

        [[nodiscard]] bool empty() const noexcept { return count == 0; }
        [[nodiscard]] std::size_t size() const noexcept { return count; }
        [[nodiscard]] const std::uint32_t *begin() const noexcept { return items; }
        [[nodiscard]] const std::uint32_t *end() const noexcept
        {
            return items == nullptr ? nullptr : items + count;
        }
        [[nodiscard]] std::uint32_t operator[](std::size_t index) const noexcept
        {
            return items[index];
        }
    };

} // namespace cora::cir

#endif // CORA_CIR_INST_H
