#include <Cora/Sema/Sema.h>

namespace cora::sema
{
    Sema::Sema(cir::CIR &cir, SourceManager &sm, DiagnosticEngine &de)
        : m_CIR(cir), m_SM(sm), m_DE(de)
    {
        (void)m_SM;
        (void)m_DE;
    }

    void Sema::error(const std::string &message) { errors_.push_back(message); }

    Type::Id Sema::resolveType(cir::TIndex index)
    {
        if (index == cir::TNull || index >= m_CIR.types.size())
            return types_.error();
        const cir::TypeRefInfo &ref = m_CIR.types[index];
        switch (ref.kind)
        {
        case cir::TypeRefKind::Named:
        {
            const std::string name(m_CIR.string(ref.name));
            if (name.size() == 1 && name[0] >= 'A' && name[0] <= 'Z')
                return types_.named(Type::Tag::Generic, name);
            if (name == "void") return types_.primitive(Type::Tag::Void);
            if (name == "bool") return types_.primitive(Type::Tag::Bool);
            if (name == "char") return types_.primitive(Type::Tag::Char);
            if (name == "i32" || name == "i64") return types_.primitive(Type::Tag::Int);
            if (name == "u32" || name == "u64") return types_.primitive(Type::Tag::UInt);
            if (name == "f32" || name == "f64") return types_.primitive(Type::Tag::Float);
            if (name == "string") return types_.primitive(Type::Tag::String);
            if (const Symbol *symbol = symbols_.lookup(name))
                return symbol->type;
            return types_.named(Type::Tag::Struct, name);
        }
        case cir::TypeRefKind::Pointer: return types_.pointer(resolveType(ref.element));
        case cir::TypeRefKind::Array: return types_.array(resolveType(ref.element), ref.length);
        case cir::TypeRefKind::Function:
        {
            std::vector<Type::Id> params;
            for (const auto param : ref.params)
                params.push_back(resolveType(param));
            return types_.function(params, resolveType(ref.result));
        }
        }
        return types_.error();
    }

    Type::Id Sema::checkInstruction(const cir::Body &body, cir::IIndex index,
                                    Type::Id expected)
    {
        if (index == cir::INull || index >= body.insts.size())
            return types_.primitive(Type::Tag::Void);
        if (index >= instruction_types_.size())
            instruction_types_.resize(index + 1, types_.error());
        const cir::Inst inst = body.insts[index];
        const auto child = [this, &body](cir::IIndex value) {
            return checkInstruction(body, value);
        };
        Type::Id result = types_.error();
        switch (inst.tag)
        {
        case cir::Tag::IntLit: result = types_.primitive(Type::Tag::Int); break;
        case cir::Tag::FloatLit: result = types_.primitive(Type::Tag::Float); break;
        case cir::Tag::BoolLit: result = types_.primitive(Type::Tag::Bool); break;
        case cir::Tag::StringLit: result = types_.primitive(Type::Tag::String); break;
        case cir::Tag::NullLit: result = types_.primitive(Type::Tag::Null); break;
        case cir::Tag::Identifier:
        {
            const Symbol *symbol = m_CIR.string(inst.name).empty()
                                       ? nullptr
                                       : symbols_.lookup(m_CIR.string(inst.name));
            if (symbol == nullptr)
                error("unresolved name: " + std::string(m_CIR.string(inst.name)));
            else
                result = symbol->type;
            break;
        }
        case cir::Tag::LetDecl:
        {
            result = inst.type_ref == cir::TNull ? child(inst.a) : resolveType(inst.type_ref);
            Symbol symbol{std::string(m_CIR.string(inst.name)), Symbol::Kind::Variable,
                          result, (inst.flags & cir::CIR_FLAG_MUTABLE) != 0, false};
            if (!symbols_.declare(std::move(symbol)))
                error("duplicate declaration: " + std::string(m_CIR.string(inst.name)));
            if (inst.a != cir::INull && !types_.compatible(result, child(inst.a)))
                error("initializer type does not match declaration");
            break;
        }
        case cir::Tag::Binary:
        case cir::Tag::Assign:
        {
            const Type::Id left = child(inst.a);
            const Type::Id right = child(inst.b);
            if (!types_.compatible(left, right))
                error("incompatible operand types");
            result = left;
            break;
        }
        case cir::Tag::Return:
            result = inst.a == cir::INull ? types_.primitive(Type::Tag::Void) : child(inst.a);
            break;
        case cir::Tag::ExprStmt:
        case cir::Tag::Paren:
            result = child(inst.a);
            break;
        default:
            for (const cir::IIndex operand : {inst.a, inst.b, inst.c})
                if (operand != cir::INull)
                    result = child(operand);
            break;
        }
        if (expected != 0 && !types_.compatible(expected, result))
            error("expression type does not match expected type");
        instruction_types_[index] = result;
        return result;
    }

    bool Sema::analyze()
    {
        errors_.clear();
        for (const cir::Body &body : m_CIR.bodies)
        {
            if (body.is_function)
                (void)symbols_.declare({body.name, Symbol::Kind::Function,
                                         types_.primitive(Type::Tag::Int), false, false});
        }
        for (const cir::Body &body : m_CIR.bodies)
        {
            symbols_.pushScope();
            for (const cir::Param &param : body.params)
                (void)symbols_.declare({std::string(m_CIR.string(param.name)),
                                        Symbol::Kind::Variable, resolveType(param.type_ref),
                                        false, false});
            for (const cir::IIndex root : body.roots)
                checkInstruction(body, root);
            symbols_.popScope();
        }
        const auto borrow_errors = BorrowChecker{}.check(m_CIR);
        for (const BorrowError &borrow : borrow_errors)
            error(borrow.message);
        m_CIR.errors.insert(m_CIR.errors.end(), errors_.begin(), errors_.end());
        return errors_.empty();
    }

    Type::Id Sema::typeOf(cir::IIndex instruction) const noexcept
    {
        return instruction < instruction_types_.size() ? instruction_types_[instruction]
                                                        : types_.error();
    }
} // namespace cora::sema
