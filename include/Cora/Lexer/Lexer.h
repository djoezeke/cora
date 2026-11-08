#ifndef CORA_LEXER_LEXER_H
#define CORA_LEXER_LEXER_H

#include <Cora/Lexer/Token.h>

#include <string>

namespace cora
{
    struct LexerConfig
    {
        bool enableComments = true;
        bool enableWhitespace = true;
    };

    class Lexer final
    {
    public:
        explicit Lexer(std::string source, std::uint32_t fileId = 0,
                       LexerConfig config = {});
        [[nodiscard]] Token nextToken();
        [[nodiscard]] TokenBuffer lexAll();

    private:
        [[nodiscard]] bool atEnd() const noexcept;
        [[nodiscard]] char peek(std::size_t distance = 0) const noexcept;
        char advance() noexcept;
        [[nodiscard]] bool match(char expected) noexcept;
        void skipTrivia();
        [[nodiscard]] Token makeToken(Token::Type type, std::size_t begin,
                                      SourcePosition start) const;
        [[nodiscard]] Token makeError(std::size_t begin, SourcePosition start,
                                      std::string message) const;
        [[nodiscard]] Token identifier(std::size_t begin, SourcePosition start);
        [[nodiscard]] Token number(std::size_t begin, SourcePosition start);
        [[nodiscard]] Token stringLiteral(std::size_t begin, SourcePosition start);
        [[nodiscard]] Token characterLiteral(std::size_t begin, SourcePosition start);
        void step(char character) noexcept;
        [[nodiscard]] SourcePosition position() const noexcept;

        std::string source_;
        std::size_t current_ = 0;
        std::uint32_t line_ = 1;
        std::uint32_t column_ = 1;
        std::uint32_t fileId_ = 0;
        LexerConfig config_{};
        std::string pendingTriviaError_;
        SourcePosition pendingTriviaStart_{};
    };
} // namespace cora

#endif // CORA_LEXER_LEXER_H
