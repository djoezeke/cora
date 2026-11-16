#include <Cora/Parser/Parser.h>

#include <Cora/Diag/DiagnosticCodes.h>
#include <Cora/Lexer/TokenNames.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cora::parser
{
    namespace
    {
        using ast::Node;
        using Tag = Token::Tag;

        [[nodiscard]] bool isNode(Node::NIndex index) noexcept
        {
            return index != Node::NNull;
        }

        [[nodiscard]] bool isClosing(Tag tag) noexcept
        {
            return tag == Tag::T_RPAREN || tag == Tag::T_RBRACKET ||
                   tag == Tag::T_RCURLY;
        }

        [[nodiscard]] bool isOpen(Tag tag) noexcept
        {
            return tag == Tag::T_LPAREN || tag == Tag::T_LBRACKET ||
                   tag == Tag::T_LCURLY;
        }

        [[nodiscard]] bool isLiteral(Tag tag) noexcept
        {
            return tag == Tag::T_INTLIT || tag == Tag::T_FLTLIT ||
                   tag == Tag::T_STRLIT || tag == Tag::T_CHARLIT ||
                   tag == Tag::T_TRUE || tag == Tag::T_FALSE ||
                   tag == Tag::T_NULLLIT;
        }

        [[nodiscard]] bool isAssignmentOperator(Tag tag) noexcept
        {
            switch (tag)
            {
            case Tag::T_ASSIGN:
            case Tag::T_PLUSEQ:
            case Tag::T_MINUSEQ:
            case Tag::T_MULEQ:
            case Tag::T_DIVEQ:
            case Tag::T_MODEQ:
            case Tag::T_ANDEQ:
            case Tag::T_OREQ:
            case Tag::T_XOREQ:
            case Tag::T_SHLEQ:
            case Tag::T_SHREQ:
            case Tag::T_COLONEQ:
                return true;
            default:
                return false;
            }
        }

        [[nodiscard]] bool isUnaryPrefix(Tag tag) noexcept
        {
            switch (tag)
            {
            case Tag::T_MINUS:
            case Tag::T_PLUS:
            case Tag::T_NOT:
            case Tag::T_BITNOT:
            case Tag::T_BITAND:
            case Tag::T_MUL:
            case Tag::T_INC:
            case Tag::T_DEC:
                return true;
            default:
                return false;
            }
        }

        [[nodiscard]] bool isStatementBoundary(Tag tag) noexcept
        {
            switch (tag)
            {
            case Tag::T_SEMI:
            case Tag::T_RCURLY:
            case Tag::T_EOF:
                return true;
            default:
                return false;
            }
        }

        [[nodiscard]] bool isDeclarationStarter(Tag tag) noexcept
        {
            switch (tag)
            {
            case Tag::T_IMPORT:
            case Tag::T_STRUCT:
            case Tag::T_ENUM:
            case Tag::T_TYPE:
            case Tag::T_FN:
            case Tag::T_EXPORT:
            case Tag::T_EXTERN:
            case Tag::T_LET:
            case Tag::T_VAR:
            case Tag::T_CONST:
                return true;
            default:
                return false;
            }
        }

        [[nodiscard]] bool isExpressionStarter(Tag tag) noexcept
        {
            if (isLiteral(tag) || isUnaryPrefix(tag))
            {
                return true;
            }
            switch (tag)
            {
            case Tag::T_IDENT:
            case Tag::T_LPAREN:
            case Tag::T_LBRACKET:
            case Tag::T_LCURLY:
            case Tag::T_FN:
                return true;
            default:
                return false;
            }
        }
    } // namespace

    // -----------------------------------------------------------------------
    // Construction / lifecycle
    // -----------------------------------------------------------------------
    Parser::Parser(SourceManager &sm, DiagnosticEngine &de, Config config)
        : m_SM(sm), m_DE(de), m_config(config)
    {
    }

    Parser::~Parser() = default;

    void Parser::reset()
    {
        m_lexer.reset();
        m_stream.clear();
        m_ast = ast::AST{};
        m_incremental = {};
        m_fileId = INVALID_FILE_ID;
        m_hasAST = false;
        m_limit = 0;
        m_hitLimit = false;
        m_emitting = true;
        m_topLevel.clear();
    }

    void Parser::prepareLexer(FileID file)
    {
        m_fileId = file;
        m_lexer = std::make_unique<lexer::Lexer>(file, m_SM, m_DE,
                                                 m_config.lexer);
    }

    const ast::AST &Parser::parse(FileID file)
    {
        m_incremental = {};
        prepareLexer(file);
        m_stream.clear();
        m_lexer->lex(m_stream);
        m_hasAST = false;
        buildFull();
        m_hasAST = true;
        return m_ast;
    }

    const ast::AST &Parser::parse(std::string_view source, FileID file)
    {
        if (m_hasAST && m_lexer != nullptr &&
            (file == INVALID_FILE_ID || file == m_fileId))
        {
            return update(source);
        }

        m_incremental = {};
        if (m_lexer == nullptr || m_fileId != file)
        {
            prepareLexer(file);
        }

        m_stream.clear();
        (void)m_lexer->update(source, m_stream);
        m_hasAST = false;
        buildFull();
        m_hasAST = true;
        return m_ast;
    }

    const ast::AST &Parser::update(std::string_view source)
    {
        if (!m_hasAST || m_lexer == nullptr)
        {
            return parse(source, m_fileId);
        }

        m_incremental = {};
        const lexer::Lexer::IncrementalResult lexed =
            m_lexer->update(source, m_stream);
        m_incremental.changed = lexed.changed;
        m_incremental.prefix_tokens = lexed.prefix_tokens;
        m_incremental.suffix_tokens = lexed.suffix_tokens;
        m_incremental.lexed_tokens = lexed.relexed_tokens;
        m_incremental.lexed_bytes = lexed.relexed_bytes;

        if (!lexed.changed)
        {
            m_incremental.reused_ast = true;
            m_incremental.prefix_decls = m_ast.decls.size();
            m_incremental.reused_nodes = m_ast.nodeCount();
            m_incremental.reused_tokens = m_ast.contentTokenCount();
            m_incremental.reparsed_token_start = m_ast.contentTokenCount();
            m_incremental.reparsed_token_end = m_ast.contentTokenCount();
            return m_ast;
        }

        const std::size_t old_content = m_ast.contentTokenCount();
        const std::size_t new_content = contentTokenCount();
        const std::size_t prefix_end =
            std::min(lexed.prefix_tokens, std::min(old_content, new_content));
        const std::size_t suffix_tokens =
            std::min(lexed.suffix_tokens, std::min(old_content, new_content));
        const std::size_t old_suffix_start = old_content - suffix_tokens;
        const std::size_t new_suffix_start = new_content - suffix_tokens;
        const std::int64_t token_delta =
            static_cast<std::int64_t>(new_content) -
            static_cast<std::int64_t>(old_content);
        const std::int64_t byte_delta =
            static_cast<std::int64_t>(m_lexer->source().size()) -
            static_cast<std::int64_t>(m_ast.source.size());

        ast::AST previous = std::move(m_ast);
        m_ast = ast::AST{};

        if (m_config.reuseIncremental && !lexed.full_relex &&
            !previous.decls.empty())
        {
            std::vector<const ast::DeclInfo *> prefix_decls;
            std::size_t prefix_count = 0;
            for (const ast::DeclInfo &decl : previous.decls)
            {
                if (decl.first_token == Node::TNull ||
                    decl.last_token == Node::TNull ||
                    decl.last_token >= prefix_end)
                {
                    break;
                }
                prefix_decls.push_back(&decl);
                ++prefix_count;
            }

            std::vector<const ast::DeclInfo *> suffix_decls;
            for (std::size_t index = previous.decls.size();
                 index > prefix_count; --index)
            {
                const ast::DeclInfo &decl = previous.decls[index - 1];
                if (decl.first_token == Node::TNull ||
                    decl.last_token == Node::TNull ||
                    decl.first_token < old_suffix_start)
                {
                    break;
                }
                suffix_decls.push_back(&decl);
            }
            std::reverse(suffix_decls.begin(), suffix_decls.end());

            std::size_t parse_start = prefix_end;
            const std::size_t suffix_first_index =
                previous.decls.size() - suffix_decls.size();
            if (prefix_count < previous.decls.size())
            {
                const ast::DeclInfo &next = previous.decls[prefix_count];
                if (next.first_token != Node::TNull)
                {
                    if (prefix_count >= suffix_first_index)
                    {
                        // Everything after the prefix is reused suffix, so
                        // there is nothing left to parse.
                        parse_start = std::min(
                            new_content,
                            static_cast<std::size_t>(
                                static_cast<std::int64_t>(next.first_token) +
                                token_delta));
                    }
                    else
                    {
                        parse_start = std::min(
                            parse_start,
                            static_cast<std::size_t>(next.first_token));
                    }
                }
            }

            std::size_t parse_end = new_suffix_start;
            if (!suffix_decls.empty())
            {
                parse_end = static_cast<std::size_t>(suffix_decls.front()
                                                         ->first_token +
                                                     token_delta);
            }

            parse_end = std::min(parse_end, new_content);
            parse_start = std::min(parse_start, parse_end);

            if (buildIncrementalAttempt(previous, prefix_decls, suffix_decls,
                                        parse_start, parse_end, token_delta,
                                        byte_delta))
            {
                m_hasAST = true;
                return m_ast;
            }
        }

        m_incremental = {};
        m_incremental.changed = true;
        m_incremental.prefix_tokens = lexed.prefix_tokens;
        m_incremental.suffix_tokens = lexed.suffix_tokens;
        m_incremental.lexed_tokens = lexed.relexed_tokens;
        m_incremental.lexed_bytes = lexed.relexed_bytes;
        buildFull();
        m_hasAST = true;
        return m_ast;
    }

    ast::AST Parser::take()
    {
        ast::AST result = std::move(m_ast);
        m_ast = ast::AST{};
        m_hasAST = false;
        return result;
    }

    // -----------------------------------------------------------------------
    // Full parse / AST assembly
    // -----------------------------------------------------------------------
    void Parser::beginAST()
    {
        m_ast.clear();
        m_ast.file_id = m_fileId;
        m_topLevel.clear();
        m_limit = contentTokenCount();
        m_hitLimit = false;
        m_emitting = true;
    }

    void Parser::copyTokensToAST()
    {
        m_ast.copyTokens(m_stream.pool());
        m_ast.source.assign(m_lexer->source());
        m_ast.file_id = m_fileId;
    }

    void Parser::buildFull()
    {
        beginAST();
        copyTokensToAST();
        (void)m_stream.reset(0);
        m_limit = contentTokenCount();
        parseTopLevelList();
        finishRoot();

        m_incremental.full_parse = true;
        m_incremental.reparsed_decls = m_ast.decls.size();
        m_incremental.reparsed_nodes = m_ast.nodeCount();
        m_incremental.reparsed_tokens = m_limit;
        m_incremental.reparsed_token_start = 0;
        m_incremental.reparsed_token_end = m_limit;
    }

    bool Parser::buildIncrementalAttempt(
        const ast::AST &previous,
        const std::vector<const ast::DeclInfo *> &prefix_decls,
        const std::vector<const ast::DeclInfo *> &suffix_decls,
        std::size_t parse_start, std::size_t parse_end,
        std::int64_t token_delta, std::int64_t byte_delta)
    {
        if (parse_start > parse_end || parse_end > contentTokenCount())
        {
            return false;
        }

        beginAST();
        copyTokensToAST();

        std::size_t reused_nodes = 0;
        for (const ast::DeclInfo *decl : prefix_decls)
        {
            const ast::DeclInfo copied =
                m_ast.appendDeclaration(previous, *decl, 0, 0);
            if (copied.node == Node::NNull || copied.node_begin >= copied.node_end)
            {
                return false;
            }
            reused_nodes += copied.node_end - copied.node_begin;
            addCopiedDecl(copied);
        }

        copyPrefixErrors(previous, prefix_decls);

        m_limit = parse_end;
        m_stream.reset(parse_start);
        parseTopLevelList();

        // A recovery operation must not bleed into the reused suffix.
        if (cursor() > parse_end)
        {
            m_stream.reset(parse_end);
        }

        copySuffixErrors(previous, suffix_decls, byte_delta);

        for (const ast::DeclInfo *decl : suffix_decls)
        {
            const ast::DeclInfo copied = m_ast.appendDeclaration(
                previous, *decl, token_delta, byte_delta);
            if (copied.node == Node::NNull || copied.node_begin >= copied.node_end)
            {
                return false;
            }
            reused_nodes += copied.node_end - copied.node_begin;
            addCopiedDecl(copied);
        }

        finishRoot();

        const std::size_t total_decls = m_ast.decls.size();
        m_incremental.prefix_decls = prefix_decls.size();
        m_incremental.suffix_decls = suffix_decls.size();
        m_incremental.reparsed_decls =
            total_decls - prefix_decls.size() - suffix_decls.size();
        m_incremental.reused_nodes = reused_nodes;
        m_incremental.reparsed_nodes =
            m_ast.nodeCount() > reused_nodes + 1
                ? m_ast.nodeCount() - reused_nodes - 1
                : 0;
        m_incremental.reparsed_tokens = parse_end - parse_start;
        m_incremental.reparsed_token_start = parse_start;
        m_incremental.reparsed_token_end = parse_end;
        m_incremental.reused_tokens =
            parse_start + (contentTokenCount() - parse_end);
        m_incremental.full_parse = false;
        return true;
    }

    void Parser::finishRoot()
    {
        const ast::AST::EIndex extras = m_ast.addExtra(m_topLevel);
        Node::Data data{};
        data.extra_data.at = extras;
        data.extra_data.count = static_cast<std::uint32_t>(m_topLevel.size());

        Node::TIndex first = Node::TNull;
        Node::TIndex last = Node::TNull;
        if (!m_topLevel.empty())
        {
            first = m_ast.firstToken(m_topLevel.front());
            last = m_ast.lastToken(m_topLevel.back());
        }

        m_ast.root = m_ast.addNode(Node::Tag::Root, Node::TNull, data, first,
                                   last);
    }

    void Parser::addParsedDecl(ast::Node::NIndex node, std::size_t node_begin,
                               std::size_t extra_begin)
    {
        if (!isNode(node))
        {
            return;
        }

        Node::TIndex first = m_ast.firstToken(node);
        Node::TIndex last = m_ast.lastToken(node);
        if (first == Node::TNull)
        {
            first = m_ast.mainToken(node);
        }
        if (last == Node::TNull)
        {
            last = first;
        }

        ast::DeclInfo decl;
        decl.node = node;
        decl.node_begin = static_cast<Node::NIndex>(node_begin);
        decl.node_end = static_cast<Node::NIndex>(m_ast.nodeCount());
        decl.extra_begin = static_cast<Node::EIndex>(extra_begin);
        decl.extra_end = static_cast<Node::EIndex>(m_ast.extra_data.size());
        decl.first_token = first;
        decl.last_token = last;

        if (first != Node::TNull)
        {
            decl.start_offset = static_cast<std::uint32_t>(
                m_ast.token(first).loc.start);
        }
        if (last != Node::TNull)
        {
            decl.end_offset =
                static_cast<std::uint32_t>(m_ast.token(last).loc.end);
        }

        m_ast.decls.push_back(decl);
        m_topLevel.push_back(node);
    }

    void Parser::addCopiedDecl(const ast::DeclInfo &decl)
    {
        if (decl.node == Node::NNull)
        {
            return;
        }
        m_ast.decls.push_back(decl);
        m_topLevel.push_back(decl.node);
    }

    void Parser::copyPrefixErrors(
        const ast::AST &previous,
        const std::vector<const ast::DeclInfo *> &decls)
    {
        for (const ast::ParseError &error : previous.errors)
        {
            for (const ast::DeclInfo *decl : decls)
            {
                if (error.span.start_offset >= decl->start_offset &&
                    error.span.end_offset <= decl->end_offset)
                {
                    m_ast.errors.push_back(error);
                    break;
                }
            }
        }
    }

    void Parser::copySuffixErrors(
        const ast::AST &previous,
        const std::vector<const ast::DeclInfo *> &decls,
        std::int64_t byte_delta)
    {
        for (const ast::ParseError &error : previous.errors)
        {
            for (const ast::DeclInfo *decl : decls)
            {
                if (error.span.start_offset < decl->start_offset ||
                    error.span.end_offset > decl->end_offset)
                {
                    continue;
                }

                ast::ParseError shifted = error;
                shifted.span.start_offset = static_cast<std::uint32_t>(
                    static_cast<std::int64_t>(error.span.start_offset) +
                    byte_delta);
                shifted.span.end_offset = static_cast<std::uint32_t>(
                    static_cast<std::int64_t>(error.span.end_offset) +
                    byte_delta);
                m_ast.errors.push_back(std::move(shifted));
                break;
            }
        }
    }

    // -----------------------------------------------------------------------
    // Cursor / token helpers
    // -----------------------------------------------------------------------
    Token Parser::peek(std::size_t lookahead) const
    {
        const std::size_t limit = std::min(m_limit, m_stream.size());
        const std::size_t index = m_stream.cursor() + lookahead;
        if (index < limit)
        {
            return m_stream.token(index);
        }

        Token eof{};
        eof.tag = Tag::T_EOF;
        std::size_t offset = m_lexer != nullptr ? m_lexer->source().size()
                                                : m_ast.source.size();
        if (limit < m_stream.size())
        {
            offset = m_stream.start(limit);
        }
        eof.loc.start = offset;
        eof.loc.end = offset;
        eof.flag = 0;
        return eof;
    }

    Token::Tag Parser::peekTag(std::size_t lookahead) const
    {
        return peek(lookahead).tag;
    }

    Token Parser::advance()
    {
        if (atEnd())
        {
            return peek();
        }
        return m_stream.consume();
    }

    bool Parser::check(Token::Tag tag) const
    {
        return peekTag() == tag;
    }

    bool Parser::match(Token::Tag tag)
    {
        if (!check(tag))
        {
            return false;
        }
        (void)advance();
        return true;
    }

    bool Parser::expect(Token::Tag tag)
    {
        if (match(tag))
        {
            return true;
        }
        errorExpected(tag);
        return false;
    }

    bool Parser::atEnd()
    {
        const std::size_t limit = std::min(m_limit, m_stream.size());
        return m_stream.cursor() >= limit;
    }

    ast::Node::TIndex Parser::currentIndex() const noexcept
    {
        const std::size_t limit = std::min(m_limit, m_stream.size());
        if (m_stream.cursor() >= limit)
        {
            return Node::TNull;
        }
        return static_cast<Node::TIndex>(m_stream.cursor());
    }

    ast::Node::TIndex Parser::previousIndex() const noexcept
    {
        if (m_stream.cursor() == 0)
        {
            return Node::TNull;
        }
        return static_cast<Node::TIndex>(m_stream.cursor() - 1);
    }

    std::size_t Parser::contentTokenCount() const noexcept
    {
        if (m_stream.size() == 0)
        {
            return 0;
        }
        return m_stream.tag(m_stream.size() - 1) == Tag::T_EOF
                   ? m_stream.size() - 1
                   : m_stream.size();
    }

    void Parser::markLimitIfNeeded()
    {
        m_limit = contentTokenCount();
    }

    bool Parser::isIdentifierText(std::size_t index, std::string_view text) const
    {
        if (index >= m_stream.size())
        {
            return false;
        }
        return m_stream.spelling(index) == text;
    }

    bool Parser::looksLikeGenericArgs() const
    {
        if (!check(Tag::T_LT))
        {
            return false;
        }

        // Require `name<...` to be adjacent, which makes ordinary comparisons
        // such as `a < b` much less likely to be mistaken for generic args.
        const std::size_t open = m_stream.cursor();
        if (open == 0 || m_stream.end(open - 1) != m_stream.start(open))
        {
            return false;
        }

        std::size_t index = open + 1;
        int depth = 1;
        const std::size_t limit = std::min(m_limit, m_stream.size());
        const std::size_t scan_end = std::min(limit, index + 128);

        for (; index < scan_end; ++index)
        {
            const Tag tag = m_stream.tag(index);
            switch (tag)
            {
            case Tag::T_LT:
                ++depth;
                break;
            case Tag::T_GT:
                --depth;
                break;
            case Tag::T_SHR:
                depth -= 2;
                break;
            case Tag::T_IDENT:
            case Tag::T_TBOOL:
            case Tag::T_INTLIT:
            case Tag::T_STRLIT:
            case Tag::T_CHARLIT:
            case Tag::T_COMMA:
            case Tag::T_COLONCOLON:
            case Tag::T_DOT:
            case Tag::T_DOTDOT:
            case Tag::T_LBRACKET:
            case Tag::T_RBRACKET:
            case Tag::T_LPAREN:
            case Tag::T_RPAREN:
            case Tag::T_MUL:
            case Tag::T_BITAND:
            case Tag::T_QUESTION:
            case Tag::T_CONST:
            case Tag::T_VAR:
            case Tag::T_FN:
                break;
            default:
                return false;
            }

            if (depth <= 0)
            {
                const Tag next =
                    index + 1 < limit ? m_stream.tag(index + 1) : Tag::T_EOF;
                return next == Tag::T_LPAREN || next == Tag::T_LCURLY ||
                       next == Tag::T_COLONCOLON || next == Tag::T_DOT;
            }
        }
        return false;
    }

    // -----------------------------------------------------------------------
    // Diagnostics
    // -----------------------------------------------------------------------
    void Parser::report(DiagID id, SourceSpan span, std::string message,
                        Severity severity)
    {
        if (!m_emitting)
        {
            return;
        }

        if (m_config.maxErrors != 0 && m_ast.errors.size() >= m_config.maxErrors)
        {
            m_emitting = false;
            return;
        }

        ast::ParseError error;
        error.severity = severity;
        error.id = id;
        error.span = span;
        error.message = std::move(message);
        m_ast.errors.push_back(std::move(error));

        if (!m_config.emitDiagnostics || !m_DE.is_enabled(id))
        {
            return;
        }

        const std::string &text = m_ast.errors.back().message;
        DiagnosticBuilder builder = m_DE.diag(id, span);
        if (builder.active())
        {
            builder.message(text);
        }
    }

    void Parser::errorExpected(Token::Tag expected)
    {
        const Token found = peek();
        const std::string message = format_diag(
            DiagID::err_expected_token,
            std::string_view(token_display_name(expected)),
            std::string_view(token_display_name(found.tag)));
        report(DiagID::err_expected_token, tokenSpan(found), message);
    }

    void Parser::errorUnexpectedEof()
    {
        report(DiagID::err_unexpected_eof, tokenSpan(peek()),
               format_diag(DiagID::err_unexpected_eof));
    }

    void Parser::errorUnclosed(Token::Tag delimiter)
    {
        const std::string message = format_diag(
            DiagID::err_unclosed_delimiter,
            std::string_view(token_display_name(delimiter)));
        report(DiagID::err_unclosed_delimiter, tokenSpan(peek()), message);
    }

    void Parser::warnMissingSemicolon()
    {
        report(DiagID::warn_missing_semicolon, tokenSpan(peek()),
               format_diag(DiagID::warn_missing_semicolon), Severity::Warning);
    }

    SourceSpan Parser::tokenSpan(ast::Node::TIndex index) const
    {
        if (index == Node::TNull || index >= m_stream.size())
        {
            const Token eof = peek();
            return SourceSpan{m_fileId, static_cast<std::uint32_t>(eof.loc.start),
                              static_cast<std::uint32_t>(eof.loc.end)};
        }
        return SourceSpan{m_fileId,
                          static_cast<std::uint32_t>(m_stream.start(index)),
                          static_cast<std::uint32_t>(m_stream.end(index))};
    }

    SourceSpan Parser::tokenSpan(const Token &token) const
    {
        return SourceSpan{m_fileId, static_cast<std::uint32_t>(token.loc.start),
                          static_cast<std::uint32_t>(token.loc.end)};
    }

    // -----------------------------------------------------------------------
    // Recovery
    // -----------------------------------------------------------------------
    void Parser::synchronizeStatement()
    {
        const std::size_t start = cursor();
        int depth = 0;
        while (!atEnd())
        {
            const Tag tag = peekTag();
            if (isOpen(tag))
            {
                ++depth;
            }
            else if (isClosing(tag))
            {
                if (depth == 0)
                {
                    if (tag == Tag::T_RCURLY)
                    {
                        return;
                    }
                    (void)advance();
                    return;
                }
                --depth;
            }
            else if (depth == 0 && tag == Tag::T_SEMI)
            {
                (void)advance();
                return;
            }
            else if (depth == 0 && cursor() > start &&
                     (isDeclarationStarter(tag) || tag == Tag::T_IF ||
                      tag == Tag::T_WHILE || tag == Tag::T_FOR ||
                      tag == Tag::T_RETURN || tag == Tag::T_BREAK ||
                      tag == Tag::T_CONTINUE || tag == Tag::T_DEFER))
            {
                return;
            }
            (void)advance();
        }
    }

    void Parser::synchronizeTopLevel()
    {
        const std::size_t start = cursor();
        int depth = 0;
        while (!atEnd())
        {
            const Tag tag = peekTag();
            if (isOpen(tag))
            {
                ++depth;
            }
            else if (isClosing(tag))
            {
                if (depth == 0)
                {
                    (void)advance();
                    return;
                }
                --depth;
            }
            else if (depth == 0 && tag == Tag::T_SEMI)
            {
                (void)advance();
                return;
            }
            else if (depth == 0 && cursor() > start &&
                     (isDeclarationStarter(tag) ||
                      (m_config.allowTopLevelExpressions &&
                       isExpressionStarter(tag))))
            {
                return;
            }
            (void)advance();
        }
    }

    void Parser::synchronizeMember()
    {
        int depth = 0;
        while (!atEnd())
        {
            const Tag tag = peekTag();
            if (isOpen(tag))
            {
                ++depth;
            }
            else if (isClosing(tag))
            {
                if (depth == 0)
                {
                    return;
                }
                --depth;
            }
            else if (depth == 0 && (tag == Tag::T_COMMA || tag == Tag::T_SEMI))
            {
                (void)advance();
                return;
            }
            (void)advance();
        }
    }

    // -----------------------------------------------------------------------
    // Top-level declarations
    // -----------------------------------------------------------------------
    void Parser::parseTopLevelList()
    {
        while (!atEnd())
        {
            if (match(Tag::T_SEMI))
            {
                continue;
            }

            const std::size_t node_begin = m_ast.nodeCount();
            const std::size_t extra_begin = m_ast.extra_data.size();
            const std::size_t token_start = cursor();
            const ast::Node::NIndex node = parseTopLevelDecl();
            if (cursor() == token_start && !atEnd())
            {
                (void)advance();
            }
            if (!isNode(node))
            {
                continue;
            }

            if (m_ast.firstToken(node) == Node::TNull)
            {
                m_ast.setNodeRange(node, static_cast<Node::TIndex>(token_start),
                                   previousIndex());
            }
            addParsedDecl(node, node_begin, extra_begin);
        }
    }

    ast::Node::NIndex Parser::parseTopLevelDecl()
    {
        switch (peekTag())
        {
        case Tag::T_IMPORT:
            return parseImportDecl();
        case Tag::T_STRUCT:
            return parseStructDecl(true);
        case Tag::T_TYPE:
            return parseTypeDecl();
        case Tag::T_FN:
            return parseFnDecl();
        case Tag::T_LET:
        case Tag::T_VAR:
        case Tag::T_CONST:
            return parseVarDecl(true);
        case Tag::T_IDENT:
            if (peekTag(1) == Tag::T_COLONEQ &&
                m_config.allowTopLevelExpressions)
            {
                return parseShorthandDecl(true);
            }
            break;
        case Tag::T_EXPORT:
        case Tag::T_EXTERN:
        {
            const std::size_t start = cursor();
            (void)advance();
            const ast::Node::NIndex inner = parseTopLevelDecl();
            if (isNode(inner))
            {
                m_ast.setNodeRange(inner, static_cast<Node::TIndex>(start),
                                   m_ast.lastToken(inner));
            }
            return inner;
        }
        default:
            break;
        }

        if (m_config.allowTopLevelExpressions)
        {
            return parseExpressionStatement();
        }

        errorExpected(Tag::T_STRUCT);
        synchronizeTopLevel();
        return makeMalformed();
    }

    ast::Node::NIndex Parser::parseImportDecl()
    {
        const std::size_t start = cursor();
        (void)advance();

        Node::TIndex target = Node::TNull;
        if (check(Tag::T_STRLIT) || check(Tag::T_IDENT))
        {
            target = currentIndex();
            (void)advance();
        }
        else if (match(Tag::T_LT))
        {
            int depth = 1;
            while (!atEnd() && depth > 0)
            {
                if (check(Tag::T_LT))
                {
                    ++depth;
                }
                else if (check(Tag::T_GT))
                {
                    --depth;
                }
                (void)advance();
            }
            if (depth > 0)
            {
                errorUnclosed(Tag::T_LT);
            }
        }
        else
        {
            errorExpected(Tag::T_STRLIT);
        }

        (void)match(Tag::T_SEMI);

        Node::Data data{};
        return makeNode(Node::Tag::ImportDecl, target, data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseStructDecl(bool named)
    {
        const std::size_t start = cursor();
        (void)match(Tag::T_STRUCT);

        Node::TIndex name = Node::TNull;
        if (named)
        {
            if (check(Tag::T_IDENT))
            {
                name = currentIndex();
                (void)advance();
            }
            else
            {
                errorExpected(Tag::T_IDENT);
            }
        }

        skipGenericParams();

        std::vector<Node::NIndex> fields;
        if (!expect(Tag::T_LCURLY))
        {
            synchronizeMember();
            Node::Data data{};
            data.extra_data.at = Node::ENull;
            data.extra_data.count = 0;
            return makeNode(Node::Tag::StructDecl, name, data,
                            static_cast<Node::TIndex>(start), previousIndex());
        }

        while (!atEnd() && !check(Tag::T_RCURLY))
        {
            if (match(Tag::T_COMMA) || match(Tag::T_SEMI))
            {
                continue;
            }

            const std::size_t before = cursor();
            const ast::Node::NIndex field = parseStructMember();
            if (isNode(field))
            {
                fields.push_back(field);
            }
            if (cursor() == before)
            {
                (void)advance();
            }
        }

        if (!expect(Tag::T_RCURLY))
        {
            errorUnclosed(Tag::T_LCURLY);
        }
        (void)match(Tag::T_SEMI);

        Node::Data data{};
        data.extra_data.at = m_ast.addExtra(fields);
        data.extra_data.count = static_cast<std::uint32_t>(fields.size());
        return makeNode(Node::Tag::StructDecl, name, data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseStructMember()
    {
        if (check(Tag::T_IDENT) &&
            (isIdentifierText(cursor(), "pub") ||
             isIdentifierText(cursor(), "comptime")) &&
            (peekTag(1) == Tag::T_FN || peekTag(1) == Tag::T_STRUCT ||
             peekTag(1) == Tag::T_TYPE))
        {
            (void)advance();
        }

        switch (peekTag())
        {
        case Tag::T_FN:
            return parseFnDecl();
        case Tag::T_STRUCT:
            return parseStructDecl(true);
        case Tag::T_TYPE:
            return parseTypeDecl();
        default:
            break;
        }

        if (!check(Tag::T_IDENT))
        {
            errorExpected(Tag::T_IDENT);
            synchronizeMember();
            return makeMalformed();
        }

        const Node::TIndex name = currentIndex();
        (void)advance();

        Node::NIndex type = Node::NNull;
        if (match(Tag::T_COLON))
        {
            type = parseType();
        }
        else
        {
            errorExpected(Tag::T_COLON);
        }

        Node::NIndex init = Node::NNull;
        if (match(Tag::T_ASSIGN))
        {
            init = parseExpression();
        }

        Node::Data data{};
        data.let_decl.type = type;
        data.let_decl.init = init;
        return makeNode(Node::Tag::FieldDecl, name, data, name,
                        previousIndex());
    }

    ast::Node::NIndex Parser::parseTypeDecl()
    {
        const std::size_t start = cursor();
        (void)advance();

        Node::TIndex name = Node::TNull;
        if (check(Tag::T_IDENT))
        {
            name = currentIndex();
            (void)advance();
        }
        else
        {
            errorExpected(Tag::T_IDENT);
        }

        skipGenericParams();
        (void)expect(Tag::T_ASSIGN);

        Node::NIndex target = Node::NNull;
        if (check(Tag::T_STRUCT))
        {
            target = parseStructDecl(false);
        }
        else
        {
            target = parseType();
        }

        (void)match(Tag::T_SEMI);

        Node::Data data{};
        data.lhs = target;
        return makeNode(Node::Tag::TypeDecl, name, data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseFnDecl()
    {
        const std::size_t start = cursor();
        (void)advance();
        return parseFnRest(static_cast<Node::TIndex>(start), true);
    }

    ast::Node::NIndex Parser::parseFnRest(ast::Node::TIndex start_token,
                                          bool allow_name)
    {
        // Destructors are written `func ~Name()`, keep the `~` in the range.
        if (match(Tag::T_BITNOT))
        {
        }

        Node::TIndex name = Node::TNull;
        if (allow_name && check(Tag::T_IDENT))
        {
            name = currentIndex();
            (void)advance();
        }
        else if (allow_name)
        {
            errorExpected(Tag::T_IDENT);
        }

        skipGenericParams();
        const std::vector<Node::NIndex> params = parseParamList(true);

        Node::NIndex return_type = Node::NNull;
        if (match(Tag::T_ARROW))
        {
            return_type = parseType();
        }

        Node::NIndex body = Node::NNull;
        if (check(Tag::T_LCURLY))
        {
            body = parseBody();
        }
        else
        {
            (void)match(Tag::T_SEMI);
        }

        std::vector<Node::NIndex> extras = params;
        extras.push_back(return_type);
        Node::Data data{};
        data.fn_decl.params = m_ast.addExtra(extras);
        data.fn_decl.param_count = static_cast<std::uint32_t>(params.size());
        data.fn_decl.body = body;

        return makeNode(Node::Tag::FnDecl, name, data, start_token,
                        previousIndex());
    }

    std::vector<ast::Node::NIndex> Parser::parseParamList(bool named)
    {
        std::vector<Node::NIndex> params;
        if (!expect(Tag::T_LPAREN))
        {
            return params;
        }

        while (!atEnd() && !check(Tag::T_RPAREN))
        {
            if (match(Tag::T_COMMA) || match(Tag::T_SEMI))
            {
                continue;
            }

            const std::size_t before = cursor();
            const Node::NIndex param = parseParam(named);
            if (isNode(param))
            {
                params.push_back(param);
            }
            if (cursor() == before)
            {
                (void)advance();
            }
        }

        if (!expect(Tag::T_RPAREN))
        {
            errorUnclosed(Tag::T_LPAREN);
        }
        return params;
    }

    ast::Node::NIndex Parser::parseParam(bool named)
    {
        const std::size_t start = cursor();

        while (check(Tag::T_CONST) || check(Tag::T_VAR))
        {
            (void)advance();
        }

        bool variadic = false;
        if (check(Tag::T_DOTDOT))
        {
            variadic = true;
            (void)advance();
            if (check(Tag::T_DOT))
            {
                (void)advance();
            }
        }

        Node::TIndex name = Node::TNull;
        Node::NIndex type = Node::NNull;

        if (named && check(Tag::T_IDENT) && peekTag(1) == Tag::T_COLON)
        {
            name = currentIndex();
            (void)advance();
            (void)advance();
            type = parseType();
        }
        else
        {
            type = parseType();
            if (named && check(Tag::T_IDENT) && peekTag(1) == Tag::T_COLON)
            {
                // Recovery for a misplaced type: treat the trailing name as
                // the binding and keep the already parsed type.
                name = currentIndex();
                (void)advance();
                (void)advance();
                (void)parseType();
            }
        }

        Node::NIndex default_value = Node::NNull;
        if (match(Tag::T_ASSIGN))
        {
            default_value = parseExpression();
        }

        Node::Data data{};
        data.param.type = type;
        data.param.default_value = default_value;
        const Node::TIndex main =
            name != Node::TNull ? name : static_cast<Node::TIndex>(start);
        (void)variadic;
        return makeNode(Node::Tag::Param, main, data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    void Parser::skipGenericParams()
    {
        if (!check(Tag::T_LT))
        {
            return;
        }

        (void)advance();
        int depth = 1;
        while (!atEnd() && depth > 0)
        {
            const Tag tag = peekTag();
            if (tag == Tag::T_LT)
            {
                ++depth;
            }
            else if (tag == Tag::T_GT)
            {
                --depth;
            }
            else if (tag == Tag::T_SHR)
            {
                depth -= 2;
            }
            (void)advance();
        }
        if (depth > 0)
        {
            errorUnclosed(Tag::T_LT);
        }
    }

    void Parser::skipGenericArgs()
    {
        skipGenericParams();
    }

    // -----------------------------------------------------------------------
    // Statements / declarations
    // -----------------------------------------------------------------------
    ast::Node::NIndex Parser::parseVarDecl(bool consume_semicolon)
    {
        const std::size_t start = cursor();
        const Node::Tag tag =
            peekTag() == Tag::T_CONST ? Node::Tag::ConstDecl
                                      : Node::Tag::LetDecl;
        (void)advance();

        Node::TIndex name = Node::TNull;
        if (check(Tag::T_IDENT))
        {
            name = currentIndex();
            (void)advance();
        }
        else
        {
            errorExpected(Tag::T_IDENT);
        }

        Node::NIndex type = Node::NNull;
        if (match(Tag::T_COLON))
        {
            type = parseType();
        }

        Node::NIndex init = Node::NNull;
        if (match(Tag::T_ASSIGN) || match(Tag::T_COLONEQ))
        {
            init = parseExpression();
        }

        if (consume_semicolon && !match(Tag::T_SEMI))
        {
            if (!isStatementBoundary(peekTag()))
            {
                warnMissingSemicolon();
                synchronizeStatement();
            }
        }

        Node::Data data{};
        data.let_decl.type = type;
        data.let_decl.init = init;
        return makeNode(tag, name, data, static_cast<Node::TIndex>(start),
                        previousIndex());
    }

    ast::Node::NIndex Parser::parseShorthandDecl(bool consume_semicolon)
    {
        const std::size_t start = cursor();
        const Node::TIndex name = currentIndex();
        (void)advance();
        (void)expect(Tag::T_COLONEQ);
        const Node::NIndex init = parseExpression();

        if (consume_semicolon && !match(Tag::T_SEMI))
        {
            if (!isStatementBoundary(peekTag()))
            {
                warnMissingSemicolon();
                synchronizeStatement();
            }
        }

        Node::Data data{};
        data.let_decl.type = Node::NNull;
        data.let_decl.init = init;
        return makeNode(Node::Tag::LetDecl, name, data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseStatement()
    {
        switch (peekTag())
        {
        case Tag::T_LCURLY:
            return parseBlock();
        case Tag::T_IF:
            return parseIf();
        case Tag::T_WHILE:
            return parseWhile();
        case Tag::T_FOR:
            return parseFor();
        case Tag::T_RETURN:
            return parseReturn();
        case Tag::T_BREAK:
            return parseBreak(true);
        case Tag::T_CONTINUE:
            return parseContinue();
        case Tag::T_DEFER:
            return parseDefer();
        case Tag::T_LET:
        case Tag::T_VAR:
        case Tag::T_CONST:
            return parseVarDecl(true);
        case Tag::T_IMPORT:
            return parseImportDecl();
        case Tag::T_STRUCT:
            return parseStructDecl(true);
        case Tag::T_TYPE:
            return parseTypeDecl();
        case Tag::T_FN:
            return parseFnDecl();
        case Tag::T_IDENT:
            if (peekTag(1) == Tag::T_COLONEQ)
            {
                return parseShorthandDecl(true);
            }
            break;
        default:
            break;
        }

        return parseExpressionStatement();
    }

    ast::Node::NIndex Parser::parseBlock()
    {
        const std::size_t start = cursor();
        if (!expect(Tag::T_LCURLY))
        {
            return makeMalformed();
        }

        std::vector<Node::NIndex> statements;
        while (!atEnd() && !check(Tag::T_RCURLY))
        {
            if (match(Tag::T_SEMI))
            {
                continue;
            }

            const std::size_t before = cursor();
            const Node::NIndex statement = parseStatement();
            if (isNode(statement))
            {
                statements.push_back(statement);
            }
            if (cursor() == before)
            {
                (void)advance();
            }
        }

        if (!expect(Tag::T_RCURLY))
        {
            errorUnclosed(Tag::T_LCURLY);
        }

        Node::Data data{};
        data.extra_data.at = m_ast.addExtra(statements);
        data.extra_data.count =
            static_cast<std::uint32_t>(statements.size());
        return makeNode(Node::Tag::Block, static_cast<Node::TIndex>(start),
                        data, static_cast<Node::TIndex>(start),
                        previousIndex());
    }

    ast::Node::NIndex Parser::parseBody()
    {
        if (check(Tag::T_LCURLY))
        {
            return parseBlock();
        }
        return parseStatement();
    }

    ast::Node::NIndex Parser::parseIf()
    {
        const std::size_t start = cursor();
        (void)advance();

        Node::NIndex cond = Node::NNull;
        if (check(Tag::T_LPAREN))
        {
            cond = parseParen();
        }
        else
        {
            cond = parseExpression(1, false);
        }

        Node::NIndex then_body = parseBody();
        Node::NIndex else_body = Node::NNull;
        if (match(Tag::T_ELSE))
        {
            if (check(Tag::T_IF))
            {
                else_body = parseIf();
            }
            else
            {
                else_body = parseBody();
            }
        }

        Node::Data data{};
        if (else_body == Node::NNull)
        {
            data.cond_then.cond = cond;
            data.cond_then.then_body = then_body;
            return makeNode(Node::Tag::IfSimple, static_cast<Node::TIndex>(start),
                            data, static_cast<Node::TIndex>(start),
                            previousIndex());
        }

        data.cond_then_else.cond = cond;
        data.cond_then_else.then_body = then_body;
        data.cond_then_else.else_body = else_body;
        return makeNode(Node::Tag::IfFull, static_cast<Node::TIndex>(start),
                        data, static_cast<Node::TIndex>(start),
                        previousIndex());
    }

    ast::Node::NIndex Parser::parseWhile()
    {
        const std::size_t start = cursor();
        (void)advance();

        Node::NIndex cond = Node::NNull;
        if (check(Tag::T_LPAREN))
        {
            cond = parseParen();
        }
        else
        {
            cond = parseExpression(1, false);
        }

        const Node::NIndex body = parseBody();

        Node::Data data{};
        data.cond_then.cond = cond;
        data.cond_then.then_body = body;
        return makeNode(Node::Tag::While, static_cast<Node::TIndex>(start), data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseFor()
    {
        const std::size_t start = cursor();
        (void)advance();

        std::vector<Node::NIndex> statements;
        if (!expect(Tag::T_LPAREN))
        {
            (void)match(Tag::T_RPAREN);
            const Node::NIndex body = parseBody();
            Node::Data data{};
            data.for_c.parts = m_ast.addExtra(statements);
            data.for_c.part_count = 0;
            data.for_c.body = body;
            return makeNode(Node::Tag::ForC, static_cast<Node::TIndex>(start),
                            data, static_cast<Node::TIndex>(start),
                            previousIndex());
        }

        // `for (item in iterable)`
        if (check(Tag::T_IDENT) && peekTag(1) == Tag::T_IN)
        {
            const std::size_t binding_start = cursor();
            const Node::TIndex binding_token = currentIndex();
            (void)advance();
            (void)advance();
            const Node::NIndex iterable = parseExpression();
            (void)expect(Tag::T_RPAREN);
            const Node::NIndex body = parseBody();

            Node::Data binding_data{};
            binding_data.let_decl.type = Node::NNull;
            binding_data.let_decl.init = Node::NNull;
            const Node::NIndex binding = makeNode(
                Node::Tag::LetDecl, binding_token, binding_data,
                static_cast<Node::TIndex>(binding_start),
                static_cast<Node::TIndex>(binding_start));

            Node::Data data{};
            data.for_range.binding = binding;
            data.for_range.iterable = iterable;
            data.for_range.body = body;
            return makeNode(Node::Tag::ForRange,
                            static_cast<Node::TIndex>(start), data,
                            static_cast<Node::TIndex>(start),
                            previousIndex());
        }

        // `for (item : iterable)` is a range-for whose binding is annotated
        // with an expression rather than a type.
        bool colon_range = false;
        if (check(Tag::T_IDENT) && peekTag(1) == Tag::T_COLON)
        {
            int depth = 0;
            for (std::size_t index = m_stream.cursor() + 2;
                 index < m_limit; ++index)
            {
                const Tag tag = m_stream.tag(index);
                if (isOpen(tag))
                {
                    ++depth;
                }
                else if (isClosing(tag))
                {
                    if (depth == 0 && tag == Tag::T_RPAREN)
                    {
                        colon_range = true;
                        break;
                    }
                    if (depth > 0)
                    {
                        --depth;
                    }
                }
                else if (depth == 0 && tag == Tag::T_SEMI)
                {
                    break;
                }
            }
        }

        if (colon_range)
        {
            const std::size_t binding_start = cursor();
            const Node::TIndex binding_token = currentIndex();
            (void)advance();
            (void)advance();
            const Node::NIndex iterable = parseExpression();
            (void)expect(Tag::T_RPAREN);
            const Node::NIndex body = parseBody();

            Node::Data binding_data{};
            binding_data.let_decl.type = Node::NNull;
            binding_data.let_decl.init = Node::NNull;
            const Node::NIndex binding = makeNode(
                Node::Tag::LetDecl, binding_token, binding_data,
                static_cast<Node::TIndex>(binding_start),
                static_cast<Node::TIndex>(binding_start));

            Node::Data data{};
            data.for_range.binding = binding;
            data.for_range.iterable = iterable;
            data.for_range.body = body;
            return makeNode(Node::Tag::ForRange,
                            static_cast<Node::TIndex>(start), data,
                            static_cast<Node::TIndex>(start),
                            previousIndex());
        }

        const Node::NIndex init = parseForInit();
        if (!isNode(init))
        {
            (void)match(Tag::T_SEMI);
        }
        else
        {
            (void)expect(Tag::T_SEMI);
        }

        Node::NIndex cond = Node::NNull;
        if (!check(Tag::T_SEMI) && !check(Tag::T_RPAREN))
        {
            cond = parseExpression(1, false);
        }
        (void)expect(Tag::T_SEMI);

        Node::NIndex step = Node::NNull;
        if (!check(Tag::T_RPAREN) && !atEnd())
        {
            step = parseExpression(1, false);
        }
        (void)expect(Tag::T_RPAREN);

        const Node::NIndex body = parseBody();
        statements.push_back(init == Node::NNull ? Node::NNull : init);
        statements.push_back(cond == Node::NNull ? Node::NNull : cond);
        statements.push_back(step == Node::NNull ? Node::NNull : step);

        Node::Data data{};
        data.for_c.parts = m_ast.addExtra(statements);
        data.for_c.part_count =
            static_cast<std::uint32_t>(statements.size());
        data.for_c.body = body;
        return makeNode(Node::Tag::ForC, static_cast<Node::TIndex>(start), data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseForInit()
    {
        if (check(Tag::T_LET) || check(Tag::T_VAR) || check(Tag::T_CONST))
        {
            return parseVarDecl(false);
        }

        if (check(Tag::T_IDENT) && peekTag(1) == Tag::T_COLONEQ)
        {
            return parseShorthandDecl(false);
        }

        if (check(Tag::T_IDENT) && peekTag(1) == Tag::T_COLON)
        {
            const std::size_t start = cursor();
            const Node::TIndex name = currentIndex();
            (void)advance();
            (void)advance();
            const Node::NIndex type = parseType();
            Node::NIndex init = Node::NNull;
            if (match(Tag::T_ASSIGN))
            {
                init = parseExpression();
            }

            Node::Data data{};
            data.let_decl.type = type;
            data.let_decl.init = init;
            return makeNode(Node::Tag::LetDecl, name, data,
                            static_cast<Node::TIndex>(start), previousIndex());
        }

        if (check(Tag::T_SEMI) || check(Tag::T_RPAREN) || atEnd())
        {
            return Node::NNull;
        }

        return parseExpression(1, false);
    }

    ast::Node::NIndex Parser::parseReturn()
    {
        const std::size_t start = cursor();
        (void)advance();

        Node::NIndex value = Node::NNull;
        if (!isStatementBoundary(peekTag()))
        {
            value = parseExpression();
        }
        (void)match(Tag::T_SEMI);

        Node::Data data{};
        data.lhs = value;
        return makeNode(Node::Tag::Return, static_cast<Node::TIndex>(start),
                        data, static_cast<Node::TIndex>(start),
                        previousIndex());
    }

    ast::Node::NIndex Parser::parseBreak(bool is_break)
    {
        const std::size_t start = cursor();
        (void)advance();
        (void)match(Tag::T_SEMI);

        Node::Data data{};
        return makeNode(is_break ? Node::Tag::Break : Node::Tag::Continue,
                        static_cast<Node::TIndex>(start), data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseContinue()
    {
        return parseBreak(false);
    }

    ast::Node::NIndex Parser::parseDefer()
    {
        const std::size_t start = cursor();
        (void)advance();
        const Node::NIndex body = parseStatement();

        Node::Data data{};
        data.lhs = body;
        return makeNode(Node::Tag::Defer, static_cast<Node::TIndex>(start), data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseExpressionStatement()
    {
        const std::size_t start = cursor();
        const Node::NIndex value = parseExpression();
        if (!match(Tag::T_SEMI))
        {
            if (!isStatementBoundary(peekTag()))
            {
                warnMissingSemicolon();
                synchronizeStatement();
            }
        }

        Node::Data data{};
        data.lhs = value;
        return makeNode(Node::Tag::ExprStmt, static_cast<Node::TIndex>(start),
                        data, static_cast<Node::TIndex>(start),
                        previousIndex());
    }

    // -----------------------------------------------------------------------
    // Expressions
    // -----------------------------------------------------------------------
    ast::Node::NIndex Parser::parseExpression(int min_precedence,
                                              bool allow_struct_init)
    {
        Node::NIndex lhs = parseUnary(allow_struct_init);

        for (;;)
        {
            const Tag tag = peekTag();
            if (isAssignmentOperator(tag) && min_precedence <= 1)
            {
                const Node::TIndex op = currentIndex();
                (void)advance();
                const Node::NIndex rhs = parseExpression(1, allow_struct_init);
                Node::Data data{};
                data.node_and_node.lhs = lhs;
                data.node_and_node.rhs = rhs;
                lhs = makeNode(Node::Tag::Assign, op, data,
                               m_ast.firstToken(lhs), m_ast.lastToken(rhs));
                continue;
            }

            const int prec = precedence(tag);
            if (prec == 0 || prec < min_precedence)
            {
                break;
            }

            const Node::TIndex op = currentIndex();
            (void)advance();
            const int next_min = rightAssociative(tag) ? prec : prec + 1;
            const Node::NIndex rhs = parseExpression(next_min, allow_struct_init);

            Node::Data data{};
            data.node_and_node.lhs = lhs;
            data.node_and_node.rhs = rhs;
            lhs = makeNode(Node::Tag::Binary, op, data, m_ast.firstToken(lhs),
                           m_ast.lastToken(rhs));
        }

        return lhs;
    }

    ast::Node::NIndex Parser::parseUnary(bool allow_struct_init)
    {
        if (isUnaryPrefix(peekTag()))
        {
            const std::size_t start = cursor();
            const Node::TIndex op = currentIndex();
            (void)advance();
            const Node::NIndex operand = parseUnary(allow_struct_init);

            Node::Data data{};
            data.lhs = operand;
            return makeNode(Node::Tag::Unary, op, data,
                            static_cast<Node::TIndex>(start),
                            m_ast.lastToken(operand));
        }

        return parsePostfix(parsePrimary(allow_struct_init), allow_struct_init);
    }

    ast::Node::NIndex Parser::parsePostfix(ast::Node::NIndex base,
                                           bool allow_struct_init)
    {
        for (;;)
        {
            switch (peekTag())
            {
            case Tag::T_LPAREN:
            {
                const std::size_t open = cursor();
                const std::vector<Node::NIndex> args = parseArguments();
                Node::Data data{};
                data.call.callee = base;
                data.call.args = m_ast.addExtra(args);
                data.call.arg_count = static_cast<std::uint32_t>(args.size());
                base = makeNode(Node::Tag::Call,
                                static_cast<Node::TIndex>(open), data,
                                m_ast.firstToken(base), previousIndex());
                break;
            }
            case Tag::T_DOT:
            case Tag::T_ARROW:
            {
                (void)advance();
                Node::TIndex field = Node::TNull;
                if (check(Tag::T_IDENT))
                {
                    field = currentIndex();
                    (void)advance();
                }
                else
                {
                    errorExpected(Tag::T_IDENT);
                }
                Node::Data data{};
                data.field_access.base = base;
                data.field_access.field = field;
                base = makeNode(Node::Tag::FieldAccess, field, data,
                                m_ast.firstToken(base), previousIndex());
                break;
            }
            case Tag::T_LBRACKET:
            {
                const std::size_t open = cursor();
                (void)advance();
                const Node::NIndex index = parseExpression();
                (void)expect(Tag::T_RBRACKET);
                Node::Data data{};
                data.node_and_node.lhs = base;
                data.node_and_node.rhs = index;
                base = makeNode(Node::Tag::IndexExpr,
                                static_cast<Node::TIndex>(open), data,
                                m_ast.firstToken(base), previousIndex());
                break;
            }
            case Tag::T_INC:
            case Tag::T_DEC:
            {
                const Node::TIndex op = currentIndex();
                (void)advance();
                Node::Data data{};
                data.lhs = base;
                base = makeNode(Node::Tag::Unary, op, data,
                                m_ast.firstToken(base), previousIndex());
                break;
            }
            case Tag::T_LCURLY:
                if (allow_struct_init)
                {
                    base = parseStructInit(base, allow_struct_init);
                    break;
                }
                return base;
            default:
                return base;
            }
        }
    }

    ast::Node::NIndex Parser::parsePrimary(bool allow_struct_init)
    {
        const std::size_t start = cursor();
        const Tag tag = peekTag();

        if (isLiteral(tag))
        {
            const Node::TIndex token = currentIndex();
            (void)advance();
            Node::Tag node_tag = Node::Tag::Malformed;
            switch (tag)
            {
            case Tag::T_INTLIT:
                node_tag = Node::Tag::IntLit;
                break;
            case Tag::T_FLTLIT:
                node_tag = Node::Tag::FloatLit;
                break;
            case Tag::T_STRLIT:
                node_tag = Node::Tag::StringLit;
                break;
            case Tag::T_CHARLIT:
                node_tag = Node::Tag::CharLit;
                break;
            case Tag::T_TRUE:
            case Tag::T_FALSE:
                node_tag = Node::Tag::BoolLit;
                break;
            case Tag::T_NULLLIT:
                node_tag = Node::Tag::NullLit;
                break;
            default:
                break;
            }

            Node::Data data{};
            return makeNode(node_tag, token, data, token, token);
        }

        switch (tag)
        {
        case Tag::T_IDENT:
        {
            const Node::TIndex name = currentIndex();
            (void)advance();

            if (check(Tag::T_LT) && looksLikeGenericArgs())
            {
                skipGenericArgs();
                if (check(Tag::T_LCURLY) && allow_struct_init)
                {
                    return parseStructInit(
                        makeNode(Node::Tag::Identifier, name, Node::Data{},
                                 name, previousIndex()),
                        allow_struct_init);
                }
            }

            Node::Data data{};
            const Node::NIndex identifier = makeNode(
                Node::Tag::Identifier, name, data, name, previousIndex());
            if (check(Tag::T_LCURLY) && allow_struct_init)
            {
                return parseStructInit(identifier, allow_struct_init);
            }
            return identifier;
        }
        case Tag::T_LPAREN:
            return parseParen();
        case Tag::T_LBRACKET:
            return parseArrayInit();
        case Tag::T_LCURLY:
            if (allow_struct_init)
            {
                return parseStructInit(Node::NNull, allow_struct_init);
            }
            break;
        case Tag::T_FN:
        {
            (void)advance();
            return parseFnRest(static_cast<Node::TIndex>(start), false);
        }
        default:
            break;
        }

        errorExpected(Tag::T_IDENT);
        return makeMalformed();
    }

    ast::Node::NIndex Parser::parseStructInit(ast::Node::NIndex type,
                                              bool allow_struct_init)
    {
        const std::size_t start = cursor();
        if (!expect(Tag::T_LCURLY))
        {
            return makeMalformed();
        }

        std::vector<Node::NIndex> fields;
        while (!atEnd() && !check(Tag::T_RCURLY))
        {
            if (match(Tag::T_COMMA) || match(Tag::T_SEMI))
            {
                continue;
            }

            if (!check(Tag::T_IDENT))
            {
                errorExpected(Tag::T_IDENT);
                synchronizeMember();
                continue;
            }

            const Node::TIndex field = currentIndex();
            (void)advance();

            Node::NIndex value = Node::NNull;
            if (match(Tag::T_COLON) || match(Tag::T_ASSIGN))
            {
                value = parseExpression(1, allow_struct_init);
            }
            else if (check(Tag::T_COMMA) || check(Tag::T_RCURLY) ||
                     check(Tag::T_SEMI))
            {
                Node::Data data{};
                value = makeNode(Node::Tag::Identifier, field, data, field,
                                 field);
            }
            else
            {
                errorExpected(Tag::T_COLON);
                value = makeMalformed();
            }

            fields.push_back(field);
            fields.push_back(value);

            if (!match(Tag::T_COMMA) && !match(Tag::T_SEMI) &&
                !check(Tag::T_RCURLY))
            {
                errorExpected(Tag::T_COMMA);
                synchronizeMember();
            }
        }

        if (!expect(Tag::T_RCURLY))
        {
            errorUnclosed(Tag::T_LCURLY);
        }

        Node::Data data{};
        data.struct_init.type = type;
        data.struct_init.fields = m_ast.addExtra(fields);
        data.struct_init.field_count =
            static_cast<std::uint32_t>(fields.size() / 2);

        const Node::TIndex first =
            type != Node::NNull ? m_ast.firstToken(type)
                                : static_cast<Node::TIndex>(start);
        return makeNode(Node::Tag::StructInit,
                        static_cast<Node::TIndex>(start), data, first,
                        previousIndex());
    }

    ast::Node::NIndex Parser::parseParen()
    {
        const std::size_t start = cursor();
        if (!expect(Tag::T_LPAREN))
        {
            return makeMalformed();
        }

        const Node::NIndex inner = parseExpression();
        if (!expect(Tag::T_RPAREN))
        {
            errorUnclosed(Tag::T_LPAREN);
        }

        Node::Data data{};
        data.lhs = inner;
        return makeNode(Node::Tag::Paren, static_cast<Node::TIndex>(start), data,
                        static_cast<Node::TIndex>(start), previousIndex());
    }

    ast::Node::NIndex Parser::parseArrayInit()
    {
        const std::size_t start = cursor();
        if (!expect(Tag::T_LBRACKET))
        {
            return makeMalformed();
        }

        std::vector<Node::NIndex> elements;
        while (!atEnd() && !check(Tag::T_RBRACKET))
        {
            if (match(Tag::T_COMMA) || match(Tag::T_SEMI))
            {
                continue;
            }

            const std::size_t before = cursor();
            elements.push_back(parseExpression());
            if (cursor() == before)
            {
                (void)advance();
                continue;
            }

            if (!match(Tag::T_COMMA) && !match(Tag::T_SEMI) &&
                !check(Tag::T_RBRACKET))
            {
                errorExpected(Tag::T_COMMA);
                synchronizeStatement();
            }
        }

        if (!expect(Tag::T_RBRACKET))
        {
            errorUnclosed(Tag::T_LBRACKET);
        }

        Node::Data data{};
        data.extra_data.at = m_ast.addExtra(elements);
        data.extra_data.count = static_cast<std::uint32_t>(elements.size());
        return makeNode(Node::Tag::ArrayInit, static_cast<Node::TIndex>(start),
                        data, static_cast<Node::TIndex>(start),
                        previousIndex());
    }

    std::vector<ast::Node::NIndex> Parser::parseArguments()
    {
        std::vector<Node::NIndex> args;
        if (!expect(Tag::T_LPAREN))
        {
            return args;
        }

        while (!atEnd() && !check(Tag::T_RPAREN))
        {
            if (match(Tag::T_COMMA) || match(Tag::T_SEMI))
            {
                continue;
            }

            const std::size_t before = cursor();
            args.push_back(parseExpression());
            if (cursor() == before)
            {
                (void)advance();
                continue;
            }

            if (!match(Tag::T_COMMA) && !match(Tag::T_SEMI) &&
                !check(Tag::T_RPAREN))
            {
                errorExpected(Tag::T_COMMA);
                synchronizeStatement();
            }
        }

        if (!expect(Tag::T_RPAREN))
        {
            errorUnclosed(Tag::T_LPAREN);
        }
        return args;
    }

    // -----------------------------------------------------------------------
    // Types
    // -----------------------------------------------------------------------
    ast::Node::NIndex Parser::parseType()
    {
        const std::size_t start = cursor();

        while (check(Tag::T_CONST) || check(Tag::T_VAR))
        {
            (void)advance();
        }

        Node::NIndex type = Node::NNull;

        if (check(Tag::T_MUL) || check(Tag::T_BITAND))
        {
            const Node::TIndex op = currentIndex();
            (void)advance();
            const Node::NIndex pointee = parseType();
            Node::Data data{};
            data.lhs = pointee;
            type = makeNode(Node::Tag::TypePointer, op, data,
                            static_cast<Node::TIndex>(start),
                            m_ast.lastToken(pointee));
        }
        else if (check(Tag::T_DOTDOT))
        {
            (void)advance();
            (void)match(Tag::T_DOT);
            type = parseType();
        }
        else if (check(Tag::T_LBRACKET))
        {
            const Node::TIndex open = currentIndex();
            (void)advance();
            if (match(Tag::T_RBRACKET))
            {
                Node::Data data{};
                data.node_and_node.lhs = Node::NNull;
                data.node_and_node.rhs = Node::NNull;
                type = makeNode(Node::Tag::TypeArray, open, data, open,
                                previousIndex());
            }
            else
            {
                const Node::NIndex element = parseType();
                Node::NIndex length = Node::NNull;
                if (match(Tag::T_SEMI))
                {
                    length = parseExpression(1, true);
                }
                (void)expect(Tag::T_RBRACKET);
                Node::Data data{};
                data.node_and_node.lhs = element;
                data.node_and_node.rhs = length;
                type = makeNode(Node::Tag::TypeArray, open, data, open,
                                previousIndex());
            }
        }
        else if (check(Tag::T_FN))
        {
            type = parseTypeFn();
        }
        else if (check(Tag::T_LPAREN))
        {
            (void)advance();
            type = parseType();
            (void)expect(Tag::T_RPAREN);
        }
        else if (check(Tag::T_IDENT) || check(Tag::T_TBOOL))
        {
            const Node::TIndex name = currentIndex();
            (void)advance();

            while (match(Tag::T_COLONCOLON))
            {
                if (check(Tag::T_IDENT) || check(Tag::T_TBOOL))
                {
                    (void)advance();
                }
                else
                {
                    errorExpected(Tag::T_IDENT);
                    break;
                }
            }

            if (check(Tag::T_LT))
            {
                skipGenericArgs();
            }

            Node::Data data{};
            type = makeNode(Node::Tag::TypeName, name, data, name,
                            previousIndex());
        }
        else
        {
            errorExpected(Tag::T_IDENT);
            type = makeMalformed();
        }

        for (;;)
        {
            if (match(Tag::T_LBRACKET))
            {
                const Node::TIndex open = previousIndex();
                if (match(Tag::T_RBRACKET))
                {
                    Node::Data data{};
                    data.node_and_node.lhs = type;
                    data.node_and_node.rhs = Node::NNull;
                    type = makeNode(Node::Tag::TypeArray, open, data,
                                    m_ast.firstToken(type), previousIndex());
                }
                else
                {
                    const Node::NIndex length = parseExpression(1, true);
                    (void)expect(Tag::T_RBRACKET);
                    Node::Data data{};
                    data.node_and_node.lhs = type;
                    data.node_and_node.rhs = length;
                    type = makeNode(Node::Tag::TypeArray, open, data,
                                    m_ast.firstToken(type), previousIndex());
                }
                continue;
            }

            if (match(Tag::T_QUESTION))
            {
                m_ast.setNodeRange(type, static_cast<Node::TIndex>(start),
                                   previousIndex());
                continue;
            }
            break;
        }

        if (type != Node::NNull)
        {
            m_ast.setNodeRange(type, static_cast<Node::TIndex>(start),
                               m_ast.lastToken(type));
        }
        return type;
    }

    ast::Node::NIndex Parser::parseTypeFn()
    {
        const std::size_t start = cursor();
        (void)advance();

        std::vector<Node::NIndex> params;
        if (match(Tag::T_LPAREN))
        {
            while (!atEnd() && !check(Tag::T_RPAREN))
            {
                if (match(Tag::T_COMMA) || match(Tag::T_SEMI))
                {
                    continue;
                }
                params.push_back(parseType());
                (void)(match(Tag::T_COMMA) || match(Tag::T_SEMI));
            }
            (void)expect(Tag::T_RPAREN);
        }

        Node::NIndex return_type = Node::NNull;
        if (match(Tag::T_ARROW))
        {
            return_type = parseType();
        }

        std::vector<Node::NIndex> extras = params;
        extras.push_back(return_type);
        Node::Data data{};
        data.fn_decl.params = m_ast.addExtra(extras);
        data.fn_decl.param_count = static_cast<std::uint32_t>(params.size());
        data.fn_decl.body = Node::NNull;
        return makeNode(Node::Tag::TypeFn, static_cast<Node::TIndex>(start),
                        data, static_cast<Node::TIndex>(start),
                        previousIndex());
    }

    // -----------------------------------------------------------------------
    // Generic / helper routines
    // -----------------------------------------------------------------------
    ast::Node::NIndex Parser::makeMalformed()
    {
        const Node::TIndex token = currentIndex();
        const Tag tag = peekTag();
        if (token != Node::TNull && tag != Tag::T_EOF && !isClosing(tag))
        {
            (void)advance();
        }

        Node::Data data{};
        return makeNode(Node::Tag::Malformed, token, data, token, token);
    }

    ast::Node::NIndex Parser::makeNode(ast::Node::Tag tag,
                                       ast::Node::TIndex main_token,
                                       const ast::Node::Data &data,
                                       ast::Node::TIndex first,
                                       ast::Node::TIndex last)
    {
        if (last == Node::TNull)
        {
            last = first;
        }
        return m_ast.addNode(tag, main_token, data, first, last);
    }

    int Parser::precedence(Token::Tag tag) noexcept
    {
        switch (tag)
        {
        case Tag::T_OROR:
            return 1;
        case Tag::T_ANDAND:
            return 2;
        case Tag::T_BITOR:
            return 3;
        case Tag::T_XOR:
            return 4;
        case Tag::T_BITAND:
            return 5;
        case Tag::T_EQ:
        case Tag::T_NEQ:
            return 6;
        case Tag::T_LT:
        case Tag::T_GT:
        case Tag::T_LTEQ:
        case Tag::T_GTEQ:
            return 7;
        case Tag::T_SHL:
        case Tag::T_SHR:
            return 8;
        case Tag::T_PLUS:
        case Tag::T_MINUS:
            return 9;
        case Tag::T_MUL:
        case Tag::T_DIV:
        case Tag::T_MOD:
            return 10;
        default:
            return 0;
        }
    }

    bool Parser::rightAssociative(Token::Tag tag) noexcept
    {
        return isAssignmentOperator(tag);
    }

    bool Parser::isAssignment(Token::Tag tag) noexcept
    {
        return isAssignmentOperator(tag);
    }

    bool Parser::startsExpression(Token::Tag tag) noexcept
    {
        return isExpressionStarter(tag);
    }

    bool Parser::startsDeclaration(Token::Tag tag) noexcept
    {
        return isDeclarationStarter(tag);
    }

} // namespace cora::parser
