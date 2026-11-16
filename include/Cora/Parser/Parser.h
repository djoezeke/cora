#ifndef CORA_PARSE_PARSER_H
#define CORA_PARSE_PARSER_H

#include <Cora/AST/AST.h>
#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Lexer/Lexer.h>
#include <Cora/Lexer/TokenStream.h>
#include <Cora/Parser/Config.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace cora::parser
{

    using namespace cora::core;
    using namespace cora::diag;

    /// Hand-written recursive-descent + Pratt parser.
    ///
    /// The parser consumes the lexer's contiguous `TokenStream` and emits a
    /// Zig-style AST stored in structure-of-arrays node columns with a separate
    /// `extra_data` arena. Syntax errors are recorded in the AST (span + code +
    /// message) and optionally forwarded to the diagnostic engine.
    ///
    /// Incremental updates reuse complete top-level declarations whose token
    /// ranges were untouched by the edit. Only declarations that overlap the
    /// lexer's changed window are parsed again; unchanged prefix/suffix
    /// declarations are deep-copied with rebased token/node/extra indices.
    class Parser final
    {
    public:
        struct IncrementalResult
        {
            bool changed = false;
            bool full_parse = false;
            bool reused_ast = false;
            std::size_t prefix_decls = 0;
            std::size_t suffix_decls = 0;
            std::size_t reparsed_decls = 0;
            std::size_t reused_nodes = 0;
            std::size_t reparsed_nodes = 0;
            std::size_t reused_tokens = 0;
            std::size_t reparsed_tokens = 0;
            std::size_t prefix_tokens = 0;
            std::size_t suffix_tokens = 0;
            std::size_t reparsed_token_start = 0;
            std::size_t reparsed_token_end = 0;
            std::size_t lexed_tokens = 0;
            std::size_t lexed_bytes = 0;
        };

        Parser(SourceManager &sm, DiagnosticEngine &de, Config config = {});
        ~Parser();

        Parser(const Parser &) = delete;
        Parser &operator=(const Parser &) = delete;
        Parser(Parser &&) = delete;
        Parser &operator=(Parser &&) = delete;

        /// Parse a file registered with the source manager.
        const ast::AST &parse(FileID file);

        /// Parse (or incrementally update) an in-memory source. Passing the
        /// same file id on later calls enables incremental reuse.
        const ast::AST &parse(std::string_view source,
                              FileID file = INVALID_FILE_ID);

        /// Incrementally update the current file id with new source text.
        const ast::AST &update(std::string_view source);

        [[nodiscard]] const ast::AST &ast() const noexcept { return m_ast; }
        [[nodiscard]] ast::AST take();
        [[nodiscard]] const IncrementalResult &incrementalResult() const noexcept
        {
            return m_incremental;
        }
        [[nodiscard]] FileID fileId() const noexcept { return m_fileId; }

        void reset();

    private:
        // -------- setup / incremental assembly ------------------------------
        void prepareLexer(FileID file);
        void buildFull();
        bool buildIncrementalAttempt(
            const ast::AST &previous,
            const std::vector<const ast::DeclInfo *> &prefix_decls,
            const std::vector<const ast::DeclInfo *> &suffix_decls,
            std::size_t parse_start, std::size_t suffix_start,
            std::int64_t token_delta, std::int64_t byte_delta);
        void finishRoot();
        void copyPrefixErrors(const ast::AST &previous,
                              const std::vector<const ast::DeclInfo *> &decls);
        void copySuffixErrors(const ast::AST &previous,
                              const std::vector<const ast::DeclInfo *> &decls,
                              std::int64_t byte_delta);

        void beginAST();
        void copyTokensToAST();
        void addParsedDecl(ast::Node::NIndex node, std::size_t node_begin,
                           std::size_t extra_begin);
        void addCopiedDecl(const ast::DeclInfo &decl);

        // -------- token cursor ----------------------------------------------
        [[nodiscard]] Token peek(std::size_t lookahead = 0) const;
        [[nodiscard]] Token::Tag peekTag(std::size_t lookahead = 0) const;
        [[nodiscard]] Token advance();
        [[nodiscard]] bool check(Token::Tag tag) const;
        [[nodiscard]] bool match(Token::Tag tag);
        [[nodiscard]] bool expect(Token::Tag tag);
        [[nodiscard]] bool atEnd();
        [[nodiscard]] std::size_t cursor() const noexcept
        {
            return m_stream.cursor();
        }
        [[nodiscard]] ast::Node::TIndex currentIndex() const noexcept;
        [[nodiscard]] ast::Node::TIndex previousIndex() const noexcept;
        [[nodiscard]] std::size_t tokenLimit() const noexcept
        {
            return m_limit;
        }
        [[nodiscard]] std::size_t contentTokenCount() const noexcept;
        void markLimitIfNeeded();

        // -------- diagnostics -------------------------------------------------
        void report(DiagID id, core::SourceSpan span, std::string message,
                    Severity severity = Severity::Error);
        void errorExpected(Token::Tag expected);
        void errorUnexpectedEof();
        void errorUnclosed(Token::Tag delimiter);
        void warnMissingSemicolon();
        [[nodiscard]] core::SourceSpan tokenSpan(
            ast::Node::TIndex index) const;
        [[nodiscard]] core::SourceSpan tokenSpan(const Token &token) const;

        // -------- recovery ----------------------------------------------------
        void synchronizeStatement();
        void synchronizeTopLevel();
        void synchronizeMember();

        // -------- top level ---------------------------------------------------
        void parseTopLevelList();
        ast::Node::NIndex parseTopLevelDecl();
        ast::Node::NIndex parseImportDecl();
        ast::Node::NIndex parseStructDecl(bool named);
        ast::Node::NIndex parseTypeDecl();
        ast::Node::NIndex parseFnDecl();

        // -------- declarations / statements ----------------------------------
        ast::Node::NIndex parseVarDecl(bool consume_semicolon = true);
        ast::Node::NIndex parseShorthandDecl(bool consume_semicolon = true);
        ast::Node::NIndex parseStatement();
        ast::Node::NIndex parseBlock();
        ast::Node::NIndex parseBody();
        ast::Node::NIndex parseIf();
        ast::Node::NIndex parseWhile();
        ast::Node::NIndex parseFor();
        ast::Node::NIndex parseForInit();
        ast::Node::NIndex parseReturn();
        ast::Node::NIndex parseBreak(bool is_break);
        ast::Node::NIndex parseContinue();
        ast::Node::NIndex parseDefer();
        ast::Node::NIndex parseExpressionStatement();

        // -------- functions / members -----------------------------------------
        ast::Node::NIndex parseFnRest(ast::Node::TIndex start_token,
                                      bool allow_name);
        ast::Node::NIndex parseParam(bool named);
        std::vector<ast::Node::NIndex> parseParamList(bool named);
        void skipGenericParams();
        void skipGenericArgs();
        ast::Node::NIndex parseStructMember();
        [[nodiscard]] bool looksLikeGenericArgs() const;
        [[nodiscard]] bool isIdentifierText(std::size_t index,
                                            std::string_view text) const;
        // -------- expressions --------------------------------------------------
        ast::Node::NIndex parseExpression(int min_precedence = 1,
                                          bool allow_struct_init = true);
        ast::Node::NIndex parseUnary(bool allow_struct_init);
        ast::Node::NIndex parsePostfix(ast::Node::NIndex base,
                                       bool allow_struct_init);
        ast::Node::NIndex parsePrimary(bool allow_struct_init);
        ast::Node::NIndex parseStructInit(ast::Node::NIndex type,
                                          bool allow_struct_init);
        ast::Node::NIndex parseParen();
        ast::Node::NIndex parseArrayInit();
        std::vector<ast::Node::NIndex> parseArguments();

        // -------- types --------------------------------------------------------
        ast::Node::NIndex parseType();
        ast::Node::NIndex parseTypeFn();

        // -------- helpers ------------------------------------------------------
        [[nodiscard]] ast::Node::NIndex makeMalformed();
        [[nodiscard]] ast::Node::NIndex makeNode(
            ast::Node::Tag tag, ast::Node::TIndex main_token,
            const ast::Node::Data &data, ast::Node::TIndex first,
            ast::Node::TIndex last);
        [[nodiscard]] static int precedence(Token::Tag tag) noexcept;
        [[nodiscard]] static bool rightAssociative(Token::Tag tag) noexcept;
        [[nodiscard]] static bool isAssignment(Token::Tag tag) noexcept;
        [[nodiscard]] static bool startsExpression(Token::Tag tag) noexcept;
        [[nodiscard]] static bool startsDeclaration(Token::Tag tag) noexcept;

    private:
        SourceManager &m_SM;
        DiagnosticEngine &m_DE;
        Config m_config;

        std::unique_ptr<lexer::Lexer> m_lexer;
        lexer::TokenStream m_stream;
        ast::AST m_ast;
        IncrementalResult m_incremental;

        FileID m_fileId = INVALID_FILE_ID;
        bool m_hasAST = false;
        std::size_t m_limit = 0;
        bool m_hitLimit = false;
        bool m_emitting = true;
        std::vector<ast::Node::NIndex> m_topLevel;
    };

} // namespace cora::parser

#endif // CORA_PARSE_PARSER_H
