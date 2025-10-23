#ifndef CORA_RUNTIME_VALUE_H
#define CORA_RUNTIME_VALUE_H

#include <string>
#include <vector>

namespace cora
{
    namespace runtime
    {

        enum class ValueKind
        {
            Null,
            Bool,
            Byte,
            Float,
            Array,
            Object,
            String,
            Integer,

        };

        class Value
        {
        public:
            Value() = default;
            virtual ~Value();

            Value(ValueKind valuekind) { m_Kind = valuekind; };

            Value(ValueKind valuekind, std::string kinsdtring)
            {
                m_Kind = valuekind;
                m_KindString = kinsdtring;
            };

            /**
             * @brief Compute a representation of the value.
             * @return String representation of the value
             */
            virtual std::string Repr() = 0;

            /**
             * @brief Return the value kind
             *
             * @return ValueKind
             */
            virtual ValueKind GetValueKind() = 0;

            /**
             * @brief Return the value kind string
             *
             * @return const std::string&
             */
            virtual std::string GetValueKindString() = 0;

            /**
             * @brief Set the value kind object
             *
             * @param valuekind
             */
            void SetValueKind(ValueKind valuekind) noexcept { m_Kind = valuekind; };

            /**
             * @brief Set the value kind string
             *
             * @param kindstring
             */
            void SetValueKindString(std::string kindstring) noexcept { m_KindString = kindstring; };

        private:
            ValueKind m_Kind;
            std::string m_KindString;
        };

        struct Null : public Value
        {
        };

        struct Byte : public Value
        {
        };

        struct Null : public Value
        {
        };

        struct Float : public Value
        {
        };

        struct Array : public Value
        {
        };

        struct Object : public Value
        {
        };

        struct String : public Value
        {
        };

        struct Integer : public Value
        {
        };

        std::ostream &operator<<(std::ostream &ostream, const Value *value);

    } // namespace runtime

} // namespace cora

#endif // CORA_RUNTIME_VALUE_H
