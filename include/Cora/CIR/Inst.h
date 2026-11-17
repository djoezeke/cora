#ifndef CORA_CIR_INST_H
#define CORA_CIR_INST_H

#include <Cora/Core/SourceLocation.h>
#include <Cora/Lexer/Token.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
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

        /// Extra list: `extra_count` items stored in the owning body's arena.
        EIndex extra = ENull;
        std::uint32_t extra_count = 0;

        /// Declaration flags / prefix-vs-postfix marker.
        std::uint32_t flags = 0;

        core::SourceSpan span{};
    };

    /// Columnar instruction storage.  The materialized Inst view keeps the
    /// indexed CIR API convenient while hot fields remain in independent
    /// arrays for linear passes.
    class InstList final
    {
    public:
        [[nodiscard]] std::size_t size() const noexcept { return tags_.size(); }
        [[nodiscard]] bool empty() const noexcept { return tags_.empty(); }
        void reserve(std::size_t count)
        {
            tags_.reserve(count);
            a_.reserve(count);
            b_.reserve(count);
            c_.reserve(count);
            d_.reserve(count);
            ops_.reserve(count);
            ints_.reserve(count);
            floats_.reserve(count);
            bools_.reserve(count);
            names_.reserve(count);
            types_.reserve(count);
            extras_.reserve(count);
            extra_counts_.reserve(count);
            flags_.reserve(count);
            spans_.reserve(count);
        }
        void clear() noexcept
        {
            tags_.clear(); a_.clear(); b_.clear(); c_.clear(); d_.clear();
            ops_.clear(); ints_.clear(); floats_.clear(); bools_.clear();
            names_.clear(); types_.clear(); extras_.clear();
            extra_counts_.clear(); flags_.clear(); spans_.clear();
        }
        void push_back(const Inst &inst)
        {
            tags_.push_back(inst.tag);
            a_.push_back(inst.a); b_.push_back(inst.b); c_.push_back(inst.c);
            d_.push_back(inst.d); ops_.push_back(inst.op);
            ints_.push_back(inst.int_value); floats_.push_back(inst.float_value);
            bools_.push_back(inst.bool_value); names_.push_back(inst.name);
            types_.push_back(inst.type_ref); extras_.push_back(inst.extra);
            extra_counts_.push_back(inst.extra_count); flags_.push_back(inst.flags);
            spans_.push_back(inst.span);
        }
        [[nodiscard]] Inst operator[](std::size_t index) const noexcept
        {
            Inst result{};
            result.tag = tags_[index]; result.a = a_[index]; result.b = b_[index];
            result.c = c_[index]; result.d = d_[index]; result.op = ops_[index];
            result.int_value = ints_[index]; result.float_value = floats_[index];
            result.bool_value = bools_[index]; result.name = names_[index];
            result.type_ref = types_[index]; result.extra = extras_[index];
            result.extra_count = extra_counts_[index]; result.flags = flags_[index];
            result.span = spans_[index];
            return result;
        }
        [[nodiscard]] const std::vector<Tag> &tags() const noexcept { return tags_; }
        [[nodiscard]] const std::vector<IIndex> &operandsA() const noexcept { return a_; }
        [[nodiscard]] const std::vector<IIndex> &operandsB() const noexcept { return b_; }

    private:
        std::vector<Tag> tags_;
        std::vector<IIndex> a_, b_, c_, d_;
        std::vector<Token::Tag> ops_;
        std::vector<std::int64_t> ints_;
        std::vector<double> floats_;
        std::vector<bool> bools_;
        std::vector<std::uint32_t> names_;
        std::vector<TIndex> types_;
        std::vector<EIndex> extras_;
        std::vector<std::uint32_t> extra_counts_;
        std::vector<std::uint32_t> flags_;
        std::vector<core::SourceSpan> spans_;
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

    struct Field
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
        InstList insts; // index 0 reserved for INull
        std::vector<std::uint32_t> extra{ENull};
        std::vector<Param> params;
        std::vector<Field> fields;
        TIndex return_type = TNull;
        TIndex alias_type = TNull;
        std::vector<IIndex> roots;
        BIndex index = BNull;
        bool is_function = false;
        bool is_type_alias = false;
        bool is_extern = false;

        Body() { insts.push_back(Inst{}); }

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
