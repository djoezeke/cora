#pragma once

#include "diag/diagnostic.h"
#include <string_view>

#include <cstdint>
#include <vector>

namespace diag
{

    /// The renderer interface. Multiple consumers can be chained.
    class DiagnosticConsumer
    {
    public:
        virtual ~DiagnosticConsumer() = default;

        virtual void handle(const Diagnostic &d) = 0;
        virtual void handle_raw_text(std::string_view /*text*/) {}
        virtual void finish() {}
    };

    /// Fan out each diagnostic to several independent consumers.
    class CompositeConsumer final : public DiagnosticConsumer
    {
    public:
        void add(DiagnosticConsumer *consumer)
        {
            if (consumer)
                consumers_.push_back(consumer);
        }

        void handle(const Diagnostic &d) override
        {
            for (auto *consumer : consumers_)
                consumer->handle(d);
        }

        void handle_raw_text(std::string_view text) override
        {
            for (auto *consumer : consumers_)
                consumer->handle_raw_text(text);
        }

        void finish() override
        {
            for (auto *consumer : consumers_)
                consumer->finish();
        }

    private:
        std::vector<DiagnosticConsumer *> consumers_;
    };

} // namespace diag
