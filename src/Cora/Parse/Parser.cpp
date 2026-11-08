#include <Cora/Parse/Parser.h>

#include <charconv>
#include <initializer_list>
#include <string_view>
#include <utility>

namespace cora
{
    namespace
    {
        using Expr = cora::ast::Expr;
        using Function = cora::ast::Function;
        using Parameter = cora::ast::Parameter;
        using Statement = cora::ast::Statement;
        using TypeRef = cora::ast::TypeRef;

        class ParserImpl final
        {
        public:
            explicit ParserImpl(TokenBuffer tokens) : tokens_(std::move(tokens)) {}

            ParsedUnit run()
            {
                ParsedUnit result;
                result.tokens = std::move(tokens_);
                result.ast = std::make_shared<cora::ast::AST>();
                result.ast->source = result.tokens.source;
                diagnostics_ = &result.diagnostics;
                tokensView_ = &result.tokens;

                while (!check(Token::Type::_EOF))
                {
                    if (!check(Token::Type::FUNC) && !check(Token::Type::PUB))
                    {
                        error(current(), "expected 'func' at module scope");
                        synchronize({Token::Type::FUNC, Token::Type::PUB, Token::Type::_EOF});
                        continue;
                    }
                    auto function = parseFunction();
                    if (function)
                        result.ast->module.functions.push_back(std::move(function));
                }
                return result;
            }

        private:
            const Token &current() const
            {
                const std::size_t index = index_ < tokensView_->tokens.size()
                                               ? index_
                                               : tokensView_->tokens.size() - 1;
                return tokensView_->tokens[index];
            }

            void consume()
            {
                if (index_ + 1 < tokensView_->tokens.size())
                    ++index_;
            }

            bool check(Token::Type type) const { return current().type == type; }

            bool match(Token::Type type)
            {
                if (!check(type))
                    return false;
                consume();
                return true;
            }

            Token expect(Token::Type type, std::string_view description)
            {
                if (check(type))
                {
                    Token result = current();
                    consume();
                    return result;
                }
                error(current(), "expected " + std::string(description) + ", got " +
                                    tokenKindName(current().type));
                Token synthetic;
                synthetic.type = type;
                synthetic.range = current().range;
                return synthetic;
            }

            void error(const Token &token, std::string message)
            {
                diagnostics_->push_back({token.range, std::move(message)});
            }

            void synchronize(std::initializer_list<Token::Type> stops)
            {
                while (!check(Token::Type::_EOF))
                {
                    for (Token::Type stop : stops)
                        if (check(stop))
                            return;
                    consume();
                }
            }

            std::unique_ptr<Function> parseFunction()
            {
                auto function = std::make_unique<Function>();
                function->isPublic = match(Token::Type::PUB);
                const Token functionToken = expect(Token::Type::FUNC, "'func'");
                function->range.begin = functionToken.range.begin;
                const Token name = expect(Token::Type::IDENTIFIER, "function name");
                function->name = name.text;
                expect(Token::Type::LPAREN, "'('");
                if (!check(Token::Type::RPAREN))
                {
                    do
                    {
                        const Token parameterName = expect(Token::Type::IDENTIFIER,
                                                           "parameter name");
                        expect(Token::Type::COLON, "':'");
                        Parameter parameter;
                        parameter.name = parameterName.text;
                        parameter.type = parseType();
                        function->parameters.push_back(std::move(parameter));
                    } while (match(Token::Type::COMMA));
                }
                expect(Token::Type::RPAREN, "')'");
                function->returnType = match(Token::Type::ARROW) ? parseType() : TypeRef{};
                function->body = parseBlock();
                function->range.end = function->body ? function->body->range.end : current().range.end;
                return function;
            }

            TypeRef parseType()
            {
                TypeRef result;
                if (check(Token::Type::IDENTIFIER))
                    result.name = current().text, consume();
                else if (check(Token::Type::NUMBER))
                    result.name = "u" + current().text, consume();
                else
                {
                    error(current(), "expected a type name");
                    result.name = "u32";
                }

                if (result.name.size() > 1 && (result.name[0] == 'u' || result.name[0] == 'i'))
                {
                    const std::string_view width(result.name.data() + 1, result.name.size() - 1);
                    std::uint32_t bits = 0;
                    const auto parsed = std::from_chars(width.data(), width.data() + width.size(), bits);
                    if (parsed.ec == std::errc{} && parsed.ptr == width.data() + width.size())
                    {
                        result.bits = bits;
                        result.isUnsigned = result.name[0] == 'u';
                    }
                }
                return result;
            }

            std::unique_ptr<Statement> parseBlock()
            {
                auto block = std::make_unique<Statement>();
                block->kind = Statement::Kind::Block;
                const Token open = expect(Token::Type::LBRACE, "'{'");
                block->range.begin = open.range.begin;
                while (!check(Token::Type::RBRACE) && !check(Token::Type::_EOF))
                {
                    if (check(Token::Type::CONST) || check(Token::Type::LET) ||
                        check(Token::Type::RETURN) || check(Token::Type::IF) ||
                        check(Token::Type::WHILE) || check(Token::Type::LBRACE))
                    {
                        block->statements.push_back(parseStatement());
                        continue;
                    }

                    auto expression = parseExpression(0);
                    if (match(Token::Type::SEMICOLON))
                    {
                        auto statement = std::make_unique<Statement>();
                        statement->kind = Statement::Kind::Expression;
                        statement->range = expression->range;
                        statement->expression = std::move(expression);
                        block->statements.push_back(std::move(statement));
                    }
                    else if (check(Token::Type::RBRACE))
                    {
                        block->tail = std::move(expression);
                    }
                    else
                    {
                        error(current(), "expected ';' or '}' after expression");
                        synchronize({Token::Type::SEMICOLON, Token::Type::RBRACE,
                                     Token::Type::_EOF});
                        match(Token::Type::SEMICOLON);
                    }
                }
                const Token close = expect(Token::Type::RBRACE, "'}'");
                block->range.end = close.range.end;
                return block;
            }

            std::unique_ptr<Statement> parseStatement()
            {
                if (check(Token::Type::CONST) || check(Token::Type::LET)) return parseVariable();
                if (check(Token::Type::RETURN)) return parseReturn();
                if (check(Token::Type::IF)) return parseIf();
                if (check(Token::Type::WHILE)) return parseWhile();
                if (check(Token::Type::LBRACE)) return parseBlock();

                auto statement = std::make_unique<Statement>();
                statement->kind = Statement::Kind::Expression;
                statement->expression = parseExpression(0);
                statement->range = statement->expression->range;
                expect(Token::Type::SEMICOLON, "';'");
                return statement;
            }

            std::unique_ptr<Statement> parseVariable()
            {
                auto statement = std::make_unique<Statement>();
                statement->kind = Statement::Kind::Variable;
                statement->isConst = match(Token::Type::CONST);
                if (!statement->isConst)
                    expect(Token::Type::LET, "'let'");
                const Token name = expect(Token::Type::IDENTIFIER, "variable name");
                statement->name = name.text;
                statement->range.begin = name.range.begin;
                statement->type = match(Token::Type::COLON) ? parseType()
                                                              : TypeRef{"u32", 32, true};
                expect(Token::Type::EQUALS, "'='");
                statement->expression = parseExpression(0);
                const Token semi = expect(Token::Type::SEMICOLON, "';'");
                statement->range.end = semi.range.end;
                return statement;
            }

            std::unique_ptr<Statement> parseReturn()
            {
                auto statement = std::make_unique<Statement>();
                statement->kind = Statement::Kind::Return;
                const Token keyword = current();
                consume();
                if (!check(Token::Type::SEMICOLON) && !check(Token::Type::RBRACE) &&
                    !check(Token::Type::_EOF))
                    statement->expression = parseExpression(0);
                if (check(Token::Type::SEMICOLON))
                    statement->range.end = (consume(), current().range.begin);
                else
                    statement->range.end = statement->expression ? statement->expression->range.end
                                                                 : keyword.range.end;
                statement->range.begin = keyword.range.begin;
                return statement;
            }

            std::unique_ptr<Statement> parseIf()
            {
                auto statement = std::make_unique<Statement>();
                statement->kind = Statement::Kind::If;
                const Token keyword = current();
                consume();
                statement->range.begin = keyword.range.begin;
                statement->condition = parseExpression(0);
                statement->thenBranch = parseBlock();
                if (match(Token::Type::ELSE))
                    statement->elseBranch = check(Token::Type::IF) ? parseIf() : parseBlock();
                statement->range.end = statement->elseBranch ? statement->elseBranch->range.end
                                                              : statement->thenBranch->range.end;
                return statement;
            }

            std::unique_ptr<Statement> parseWhile()
            {
                auto statement = std::make_unique<Statement>();
                statement->kind = Statement::Kind::While;
                const Token keyword = current();
                consume();
                statement->range.begin = keyword.range.begin;
                statement->condition = parseExpression(0);
                statement->body = parseBlock();
                statement->range.end = statement->body->range.end;
                return statement;
            }

            int precedence(Token::Type type) const noexcept
            {
                switch (type)
                {
                case Token::Type::EQUALS: return 1;
                case Token::Type::LESS_THAN:
                case Token::Type::LESS_THAN_EQUALS:
                case Token::Type::GREATER_THAN:
                case Token::Type::GREATER_THAN_EQUALS: return 5;
                case Token::Type::EQUALS_EQUALS:
                case Token::Type::NOT_EQUALS: return 4;
                case Token::Type::PLUS:
                case Token::Type::MINUS: return 6;
                case Token::Type::STAR:
                case Token::Type::SLASH:
                case Token::Type::PERCENT: return 7;
                default: return 0;
                }
            }

            std::unique_ptr<Expr> parseExpression(int minimumPrecedence)
            {
                auto left = parseUnary();
                while (true)
                {
                    const int currentPrecedence = precedence(current().type);
                    if (currentPrecedence <= minimumPrecedence)
                        break;
                    const Token op = current();
                    consume();
                    auto right = parseExpression(op.type == Token::Type::EQUALS
                                                     ? currentPrecedence - 1
                                                     : currentPrecedence);
                    auto expression = std::make_unique<Expr>();
                    expression->kind = Expr::Kind::Binary;
                    expression->range = {left->range.begin, right->range.end};
                    expression->op = op.text;
                    expression->operands.push_back(std::move(left));
                    expression->operands.push_back(std::move(right));
                    left = std::move(expression);
                }
                return left;
            }

            std::unique_ptr<Expr> parseUnary()
            {
                if (check(Token::Type::MINUS) || check(Token::Type::BANG))
                {
                    const Token op = current();
                    consume();
                    auto expression = std::make_unique<Expr>();
                    expression->kind = Expr::Kind::Unary;
                    expression->range.begin = op.range.begin;
                    expression->op = op.text;
                    expression->operands.push_back(parseUnary());
                    expression->range.end = expression->operands.front()->range.end;
                    return expression;
                }
                return parsePostfix();
            }

            std::unique_ptr<Expr> parsePostfix()
            {
                auto expression = parsePrimary();
                while (match(Token::Type::LPAREN))
                {
                    auto call = std::make_unique<Expr>();
                    call->kind = Expr::Kind::Call;
                    call->range.begin = expression->range.begin;
                    if (expression->kind == Expr::Kind::Name)
                        call->name = expression->name;
                    else
                        error(current(), "call target must be a function name");
                    if (!check(Token::Type::RPAREN))
                    {
                        do
                            call->operands.push_back(parseExpression(0));
                        while (match(Token::Type::COMMA));
                    }
                    const Token close = expect(Token::Type::RPAREN, "')'");
                    call->range.end = close.range.end;
                    expression = std::move(call);
                }
                return expression;
            }

            std::unique_ptr<Expr> parsePrimary()
            {
                const Token token = current();
                if (check(Token::Type::NUMBER))
                {
                    auto expression = std::make_unique<Expr>();
                    expression->kind = Expr::Kind::Integer;
                    expression->range = token.range;
                    expression->integerValue = token.integerValue;
                    consume();
                    return expression;
                }
                if (check(Token::Type::TRUE) || check(Token::Type::FALSE))
                {
                    auto expression = std::make_unique<Expr>();
                    expression->kind = Expr::Kind::Boolean;
                    expression->range = token.range;
                    expression->booleanValue = check(Token::Type::TRUE);
                    consume();
                    return expression;
                }
                if (check(Token::Type::IDENTIFIER))
                {
                    auto expression = std::make_unique<Expr>();
                    expression->kind = Expr::Kind::Name;
                    expression->range = token.range;
                    expression->name = token.text;
                    consume();
                    return expression;
                }
                if (match(Token::Type::LPAREN))
                {
                    auto expression = parseExpression(0);
                    const Token close = expect(Token::Type::RPAREN, "')'");
                    expression->range.begin = token.range.begin;
                    expression->range.end = close.range.end;
                    return expression;
                }

                error(token, "expected expression, got " + std::string(tokenKindName(token.type)));
                auto expression = std::make_unique<Expr>();
                expression->kind = Expr::Kind::Error;
                expression->range = token.range;
                if (!check(Token::Type::_EOF))
                    consume();
                return expression;
            }

            TokenBuffer tokens_;
            TokenBuffer *tokensView_ = nullptr;
            std::size_t index_ = 0;
            std::vector<ParseDiagnostic> *diagnostics_ = nullptr;
        };
    } // namespace

    ParsedUnit Parser::parse(TokenBuffer tokens)
    {
        return ParserImpl(std::move(tokens)).run();
    }

} // namespace cora
