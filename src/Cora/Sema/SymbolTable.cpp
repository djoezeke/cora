#include <Cora/Sema/SymbolTable.h>

namespace cora::sema
{
    void SymbolTable::pushScope() { scopes_.emplace_back(); }
    void SymbolTable::popScope()
    {
        if (scopes_.size() > 1)
            scopes_.pop_back();
    }
    bool SymbolTable::declare(Symbol symbol)
    {
        auto &scope = scopes_.back();
        return scope.emplace(symbol.name, std::move(symbol)).second;
    }
    const Symbol *SymbolTable::lookup(std::string_view name) const
    {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
        {
            const auto found = it->find(std::string(name));
            if (found != it->end())
                return &found->second;
        }
        return nullptr;
    }
} // namespace cora::sema
