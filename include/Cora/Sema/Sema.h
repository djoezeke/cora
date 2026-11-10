#ifndef CORA_SEMA_SEMA_H
#define CORA_SEMA_SEMA_H

#include <Cora/CIR/CIR.h>
#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>

namespace cora::sema
{

    using namespace cora::core;
    using namespace cora::diag;

    class Sema final
    {

    public:
        Sema(cir::CIR &cir, SourceManager &sm, DiagnosticEngine &de);

    private:
        SourceManager &m_SM;
        DiagnosticEngine &m_DE;
    };

} // namespace cora::sema

#endif // CORA_SEMA_SEMA_H
