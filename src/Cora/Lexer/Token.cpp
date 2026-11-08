#include <Cora/Lexer/Token.h>

namespace cora
{

    const char *tokenKindName(Token::Type type) noexcept
    {
        switch (type)
        {
        case Token::Type::IDENTIFIER: return "identifier";
        case Token::Type::NUMBER: return "number";
        case Token::Type::FLOAT: return "float";
        case Token::Type::STRING: return "string";
        case Token::Type::CHAR: return "character";
        case Token::Type::IMPORT: return "'import'";
        case Token::Type::AS: return "'as'";
        case Token::Type::TYPE: return "'type'";
        case Token::Type::LET: return "'let'";
        case Token::Type::MUT: return "'mut'";
        case Token::Type::CONST: return "'const'";
        case Token::Type::FUNC: return "'func'";
        case Token::Type::STRUCT: return "'struct'";
        case Token::Type::ENUM: return "'enum'";
        case Token::Type::INTERFACE: return "'interface'";
        case Token::Type::IMPL: return "'impl'";
        case Token::Type::COMPTIME: return "'comptime'";
        case Token::Type::ASYNC: return "'async'";
        case Token::Type::AWAIT: return "'await'";
        case Token::Type::IF: return "'if'";
        case Token::Type::ELSE: return "'else'";
        case Token::Type::WHILE: return "'while'";
        case Token::Type::FOR: return "'for'";
        case Token::Type::IN: return "'in'";
        case Token::Type::MATCH: return "'match'";
        case Token::Type::CASE: return "'case'";
        case Token::Type::DEFAULT: return "'default'";
        case Token::Type::RETURN: return "'return'";
        case Token::Type::PUB: return "'pub'";
        case Token::Type::SELF: return "'self'";
        case Token::Type::TRUE: return "'true'";
        case Token::Type::FALSE: return "'false'";
        case Token::Type::_NULL: return "'null'";
        case Token::Type::PLUS: return "'+'";
        case Token::Type::MINUS: return "'-'";
        case Token::Type::STAR: return "'*'";
        case Token::Type::SLASH: return "'/'";
        case Token::Type::PERCENT: return "'%'";
        case Token::Type::EQUALS: return "'='";
        case Token::Type::EQUALS_EQUALS: return "'=='";
        case Token::Type::NOT_EQUALS: return "'!='";
        case Token::Type::LESS_THAN: return "'<'";
        case Token::Type::LESS_THAN_EQUALS: return "'<='";
        case Token::Type::GREATER_THAN: return "'>'";
        case Token::Type::GREATER_THAN_EQUALS: return "'>='";
        case Token::Type::AMPERSAND: return "'&'";
        case Token::Type::BANG: return "'!'";
        case Token::Type::DOT: return "'.'";
        case Token::Type::COLON: return "':'";
        case Token::Type::COLON_COLON: return "'::'";
        case Token::Type::ARROW: return "'->'";
        case Token::Type::QUESTION_MARK: return "'?'";
        case Token::Type::LPAREN: return "'('";
        case Token::Type::RPAREN: return "')'";
        case Token::Type::LBRACE: return "'{'";
        case Token::Type::RBRACE: return "'}'";
        case Token::Type::LBRACKET: return "'['";
        case Token::Type::RBRACKET: return "']'";
        case Token::Type::COMMA: return "','";
        case Token::Type::SEMICOLON: return "';'";
        case Token::Type::_EOF: return "end of file";
        case Token::Type::ERROR: return "invalid token";
        }
        return "token";
    }

} // namespace cora
