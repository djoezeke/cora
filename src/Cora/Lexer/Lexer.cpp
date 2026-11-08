#include <Cora/Lexer/Lexer.h>

#include <charconv>
#include <cctype>
#include <string_view>
#include <utility>

namespace cora
{
    namespace
    {
        Token::Type keyword(std::string_view text) noexcept
        {
            if (text == "import") return Token::Type::IMPORT;
            if (text == "as") return Token::Type::AS;
            if (text == "type") return Token::Type::TYPE;
            if (text == "let") return Token::Type::LET;
            if (text == "mut") return Token::Type::MUT;
            if (text == "const") return Token::Type::CONST;
            if (text == "func" || text == "function") return Token::Type::FUNC;
            if (text == "struct") return Token::Type::STRUCT;
            if (text == "enum") return Token::Type::ENUM;
            if (text == "interface") return Token::Type::INTERFACE;
            if (text == "impl") return Token::Type::IMPL;
            if (text == "comptime") return Token::Type::COMPTIME;
            if (text == "async") return Token::Type::ASYNC;
            if (text == "await") return Token::Type::AWAIT;
            if (text == "if") return Token::Type::IF;
            if (text == "else") return Token::Type::ELSE;
            if (text == "while") return Token::Type::WHILE;
            if (text == "for") return Token::Type::FOR;
            if (text == "in") return Token::Type::IN;
            if (text == "match") return Token::Type::MATCH;
            if (text == "case") return Token::Type::CASE;
            if (text == "default") return Token::Type::DEFAULT;
            if (text == "return") return Token::Type::RETURN;
            if (text == "pub" || text == "public") return Token::Type::PUB;
            if (text == "self") return Token::Type::SELF;
            if (text == "true") return Token::Type::TRUE;
            if (text == "false") return Token::Type::FALSE;
            if (text == "null") return Token::Type::_NULL;
            return Token::Type::IDENTIFIER;
        }

        bool identifierStart(unsigned char character) noexcept
        {
            return std::isalpha(character) != 0 || character == '_';
        }

        bool identifierPart(unsigned char character) noexcept
        {
            return std::isalnum(character) != 0 || character == '_';
        }
    } // namespace

    Lexer::Lexer(std::string source, std::uint32_t fileId, LexerConfig config)
        : source_(std::move(source)), fileId_(fileId), config_(config)
    {}

    bool Lexer::atEnd() const noexcept
    {
        return current_ >= source_.size();
    }

    char Lexer::peek(std::size_t distance) const noexcept
    {
        const std::size_t offset = current_ + distance;
        return offset < source_.size() ? source_[offset] : '\0';
    }

    char Lexer::advance() noexcept
    {
        if (atEnd())
            return '\0';
        const char character = source_[current_++];
        step(character);
        return character;
    }

    bool Lexer::match(char expected) noexcept
    {
        if (peek() != expected)
            return false;
        advance();
        return true;
    }

    void Lexer::step(char character) noexcept
    {
        if (character == '\n')
        {
            ++line_;
            column_ = 1;
        }
        else
        {
            ++column_;
        }
    }

    SourcePosition Lexer::position() const noexcept
    {
        return {static_cast<std::uint32_t>(current_), line_, column_};
    }

    void Lexer::skipTrivia()
    {
        if (!config_.enableWhitespace && !config_.enableComments)
            return;

        for (;;)
        {
            bool consumed = false;
            if (config_.enableWhitespace)
            {
                while (!atEnd() && std::isspace(static_cast<unsigned char>(peek())) != 0)
                {
                    advance();
                    consumed = true;
                }
            }
            if (!config_.enableComments || peek() != '/')
                return;

            if (peek(1) == '/')
            {
                advance();
                advance();
                while (!atEnd() && peek() != '\n')
                    advance();
                consumed = true;
                continue;
            }
            if (peek(1) == '*')
            {
                const SourcePosition commentStart = position();
                advance();
                advance();
                while (!atEnd() && !(peek() == '*' && peek(1) == '/'))
                    advance();
                if (atEnd())
                {
                    pendingTriviaError_ = "unterminated block comment";
                    pendingTriviaStart_ = commentStart;
                    return;
                }
                advance();
                advance();
                consumed = true;
                continue;
            }
            if (!consumed)
                return;
        }
    }

    Token Lexer::makeToken(Token::Type type, std::size_t begin,
                           SourcePosition start) const
    {
        Token token;
        token.type = type;
        token.range = {start, position()};
        token.text = source_.substr(begin, current_ - begin);
        return token;
    }

    Token Lexer::makeError(std::size_t begin, SourcePosition start,
                           std::string message) const
    {
        Token token = makeToken(Token::Type::ERROR, begin, start);
        token.text = std::move(message);
        return token;
    }

    Token Lexer::identifier(std::size_t begin, SourcePosition start)
    {
        while (!atEnd() && identifierPart(static_cast<unsigned char>(peek())))
            advance();
        return makeToken(keyword(source_.substr(begin, current_ - begin)), begin, start);
    }

    Token Lexer::number(std::size_t begin, SourcePosition start)
    {
        bool floating = false;
        if (source_[begin] == '0' && (peek() == 'x' || peek() == 'X' ||
                                      peek() == 'b' || peek() == 'B' ||
                                      peek() == 'o' || peek() == 'O'))
        {
            advance();
            while (std::isalnum(static_cast<unsigned char>(peek())) != 0 || peek() == '_')
                advance();
        }
        else
        {
            while (std::isdigit(static_cast<unsigned char>(peek())) != 0 || peek() == '_')
                advance();
            if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peek(1))) != 0)
            {
                floating = true;
                advance();
                while (std::isdigit(static_cast<unsigned char>(peek())) != 0 || peek() == '_')
                    advance();
            }
            if (peek() == 'e' || peek() == 'E')
            {
                floating = true;
                advance();
                if (peek() == '+' || peek() == '-')
                    advance();
                while (std::isdigit(static_cast<unsigned char>(peek())) != 0 || peek() == '_')
                    advance();
            }
        }

        Token token = makeToken(floating ? Token::Type::FLOAT : Token::Type::NUMBER,
                                begin, start);
        if (!floating)
        {
            std::string digits;
            for (char character : token.text)
                if (character != '_')
                    digits.push_back(character);
            int base = 10;
            std::size_t digitStart = 0;
            if (digits.size() > 2 && digits[0] == '0')
            {
                if (digits[1] == 'x' || digits[1] == 'X') { base = 16; digitStart = 2; }
                else if (digits[1] == 'b' || digits[1] == 'B') { base = 2; digitStart = 2; }
                else if (digits[1] == 'o' || digits[1] == 'O') { base = 8; digitStart = 2; }
            }
            const auto parsed = std::from_chars(digits.data() + digitStart,
                                                digits.data() + digits.size(),
                                                token.integerValue, base);
            if (parsed.ec != std::errc{} || parsed.ptr != digits.data() + digits.size())
                token.type = Token::Type::ERROR;
        }
        return token;
    }

    Token Lexer::stringLiteral(std::size_t begin, SourcePosition start)
    {
        bool closed = false;
        while (!atEnd())
        {
            const char character = advance();
            if (character == '\\' && !atEnd())
            {
                advance();
                continue;
            }
            if (character == '"')
            {
                closed = true;
                break;
            }
        }
        return closed ? makeToken(Token::Type::STRING, begin, start)
                      : makeError(begin, start, "unterminated string literal");
    }

    Token Lexer::characterLiteral(std::size_t begin, SourcePosition start)
    {
        bool closed = false;
        if (!atEnd() && peek() == '\\')
        {
            advance();
            if (!atEnd())
                advance();
        }
        else if (!atEnd())
        {
            advance();
        }
        if (!atEnd() && peek() == '\'')
        {
            advance();
            closed = true;
        }
        return closed ? makeToken(Token::Type::CHAR, begin, start)
                      : makeError(begin, start, "invalid character literal");
    }

    Token Lexer::nextToken()
    {
        skipTrivia();
        if (!pendingTriviaError_.empty())
        {
            const std::string message = std::move(pendingTriviaError_);
            pendingTriviaError_.clear();
            return makeError(pendingTriviaStart_.offset, pendingTriviaStart_, message);
        }

        const std::size_t begin = current_;
        const SourcePosition start = position();
        if (atEnd())
            return makeToken(Token::Type::_EOF, begin, start);

        const unsigned char first = static_cast<unsigned char>(peek());
        if (identifierStart(first))
        {
            advance();
            return identifier(begin, start);
        }
        if (std::isdigit(first) != 0)
        {
            advance();
            return number(begin, start);
        }

        const char character = advance();
        switch (character)
        {
        case '+': return makeToken(Token::Type::PLUS, begin, start);
        case '-': return match('>') ? makeToken(Token::Type::ARROW, begin, start)
                                    : makeToken(Token::Type::MINUS, begin, start);
        case '*': return makeToken(Token::Type::STAR, begin, start);
        case '/': return makeToken(Token::Type::SLASH, begin, start);
        case '%': return makeToken(Token::Type::PERCENT, begin, start);
        case '=': return match('=') ? makeToken(Token::Type::EQUALS_EQUALS, begin, start)
                                    : makeToken(Token::Type::EQUALS, begin, start);
        case '!': return match('=') ? makeToken(Token::Type::NOT_EQUALS, begin, start)
                                    : makeToken(Token::Type::BANG, begin, start);
        case '<': return match('=') ? makeToken(Token::Type::LESS_THAN_EQUALS, begin, start)
                                    : makeToken(Token::Type::LESS_THAN, begin, start);
        case '>': return match('=') ? makeToken(Token::Type::GREATER_THAN_EQUALS, begin, start)
                                    : makeToken(Token::Type::GREATER_THAN, begin, start);
        case '&': return makeToken(Token::Type::AMPERSAND, begin, start);
        case '.': return makeToken(Token::Type::DOT, begin, start);
        case ':': return match(':') ? makeToken(Token::Type::COLON_COLON, begin, start)
                                    : makeToken(Token::Type::COLON, begin, start);
        case '?': return makeToken(Token::Type::QUESTION_MARK, begin, start);
        case '(': return makeToken(Token::Type::LPAREN, begin, start);
        case ')': return makeToken(Token::Type::RPAREN, begin, start);
        case '{': return makeToken(Token::Type::LBRACE, begin, start);
        case '}': return makeToken(Token::Type::RBRACE, begin, start);
        case '[': return makeToken(Token::Type::LBRACKET, begin, start);
        case ']': return makeToken(Token::Type::RBRACKET, begin, start);
        case ',': return makeToken(Token::Type::COMMA, begin, start);
        case ';': return makeToken(Token::Type::SEMICOLON, begin, start);
        case '"': return stringLiteral(begin, start);
        case '\'': return characterLiteral(begin, start);
        default: return makeError(begin, start, "unexpected character");
        }
    }

    TokenBuffer Lexer::lexAll()
    {
        TokenBuffer result;
        result.source = source_;
        for (;;)
        {
            Token token = nextToken();
            if (token.type == Token::Type::ERROR)
                result.errors.push_back(token.text + " at " +
                                       std::to_string(token.range.begin.line) + ":" +
                                       std::to_string(token.range.begin.column));
            else if (token.type != Token::Type::_EOF)
                ++result.significantCount;
            const bool done = token.type == Token::Type::_EOF;
            result.tokens.push_back(std::move(token));
            if (done)
                return result;
        }
    }

} // namespace cora
