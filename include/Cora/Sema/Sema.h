#ifndef CORA_SEMA_SEMA_H
#define CORA_SEMA_SEMA_H

#include <Cora/CIR/CIR.h>
#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Sema/BorrowChecker.h>
#include <Cora/Sema/SymbolTable.h>
#include <Cora/Sema/Types.h>

#include <string>
#include <vector>

namespace cora::sema
{

    using namespace cora::core;
    using namespace cora::diag;

    class Sema final
    {

    public:
        Sema(cir::CIR &cir, SourceManager &sm, DiagnosticEngine &de);
        [[nodiscard]] bool analyze();
        [[nodiscard]] Type::Id typeOf(cir::IIndex instruction) const noexcept;
        [[nodiscard]] const Types &types() const noexcept { return types_; }
        [[nodiscard]] const SymbolTable &symbols() const noexcept { return symbols_; }
        [[nodiscard]] const std::vector<std::string> &errors() const noexcept
        {
            return errors_;
        }

    private:
        Type::Id resolveType(cir::TIndex type);
        Type::Id checkInstruction(const cir::Body &body, cir::IIndex instruction,
                                  Type::Id expected = 0);
        void error(const std::string &message);
        cir::CIR &m_CIR;
        SourceManager &m_SM;
        DiagnosticEngine &m_DE;
        Types types_;
        SymbolTable symbols_;
        std::vector<Type::Id> instruction_types_;
        std::vector<std::string> errors_;
    };

} // namespace cora::sema

#endif // CORA_SEMA_SEMA_H
