#ifndef CORA_SEMA_SYMBOLTABLE_H
#define CORA_SEMA_SYMBOLTABLE_H

#include <Cora\Sema\Types.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace cora::sema
{
    struct Symbol
    {
        enum class Kind { Variable, Function, Type };
        std::string name;
        Kind kind = Kind::Variable;
        Type::Id type = 0;
        bool mutable_binding = false;
        bool generic = false;
    };

    class SymbolTable final
    {
    public:
        void pushScope();
        void popScope();
        [[nodiscard]] bool declare(Symbol symbol);
        [[nodiscard]] const Symbol *lookup(std::string_view name) const;
        [[nodiscard]] const std::vector<std::unordered_map<std::string, Symbol>> &scopes() const noexcept
        {
            return scopes_;
        }
    private:
        std::vector<std::unordered_map<std::string, Symbol>> scopes_{1};
    };
} // namespace cora::sema

#endif
