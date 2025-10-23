#ifndef CORA_BASIC_SOURCE_LOCATION_H
#define CORA_BASIC_SOURCE_LOCATION_H

namespace cora
{
    namespace basic
    {
        class SourceManager;

        class SourceLocation
        {
            friend class SourceRange;
            friend class SourceManager;
            friend class CharSourceRange;

        public:
            SourceLocation()
            {
                m_line = 0;
                m_Column = 0;
            };

            SourceLocation(unsigned int line)
            {
                m_line = line;
                m_Column = 0;
            };

            SourceLocation(unsigned int line, unsigned int column)
            {
                m_line = line;
                m_Column = column;
            };

            unsigned int Line() const { return m_line; };
            unsigned int Column() const { return m_Column; };

            bool Valid() const { return m_line != 0 && m_Column != 0; };

            bool operator==(const SourceLocation &other) const;
            bool operator!=(const SourceLocation &other) const;

        private:
            unsigned int m_line;
            unsigned int m_Column;
        };

        class SourceRange
        {
        public:
            SourceRange(SourceLocation loc)
            {
                m_Start = loc;
                m_End = loc;
            };

            SourceRange(SourceLocation start, SourceLocation end)
            {
                m_Start = start;
                m_End = end;
            };

            SourceLocation GetStart() const { return m_Start; };

            SourceLocation GetEnd() const { return m_End; };

            void SetStart(SourceLocation start) noexcept { m_Start = start; };
            void SetEnd(SourceLocation end) noexcept { m_End = end; };

            bool Valid() const { return m_Start.Valid() && m_End.Valid(); };
            bool Invalid() const { !Valid(); };

            bool operator==(const SourceRange &other) const;
            bool operator!=(const SourceRange &other) const;

        private:
            SourceLocation m_Start;
            SourceLocation m_End;
        };

        class CharSourceRange
        {
        private:
            SourceLocation m_Start;
            unsigned int m_ByteLenght;

        public:
            CharSourceRange() = default;

            CharSourceRange(SourceLocation start, unsigned int bytelenght)
            {
                m_Start = start;
                m_ByteLenght = bytelenght;
            };

            CharSourceRange(SourceLocation start, SourceLocation end)
            {
                m_Start = start;
                m_ByteLenght = end.Column() - m_Start.Column();
            };

            bool Valid() const;
            bool Invalid() const;

            SourceLocation Start() const { return m_Start; };

            SourceLocation End() const { return SourceLocation(m_Start.Line(), m_Start.Column() + m_ByteLenght); };

            bool operator==(const CharSourceRange &other) const;
            bool operator!=(const CharSourceRange &other) const;
        };

        template <typename T>
        class Location
        {
        private:
            T m_Item;
            SourceLocation m_Loc;

        public:
            Location();
            Location(T item, SourceLocation loc);

            T Item() const;
            SourceLocation Located() const;
        };

        template <typename T>
        bool operator==(const Location<T> &lhs, const Location<T> &rhs);

    } // namespace basic

} // namespace cora

#endif // CORA_BASIC_SOURCE_LOCATION_H