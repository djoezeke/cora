#ifndef CORA_LEXER_LEXER_H
#define CORA_LEXER_LEXER_H

#include <Cora/Core/SourceManager.h>
#include <Cora/Diag/DiagnosticEngine.h>
#include <Cora/Lexer/Config.h>
#include <Cora/Lexer/Interner.h>
#include <Cora/Lexer/Token.h>
#include <Cora/Lexer/TokenPool.h>
#include <Cora/Lexer/TokenStream.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cora::lexer
{

    using namespace cora::core;
    using namespace cora::diag;

    /// Hand-written UTF-8 lexer.
    ///
    /// The lexer is a deterministic scanner (no regex, no generated tables).
    /// It emits value tokens into a contiguous SoA `TokenStream` and is fully
    /// error tolerant: malformed input produces a diagnostic and is skipped,
    /// while unterminated literals still produce a token for the parser.
    class Lexer final
    {
    public:
        struct IncrementalResult
        {
            bool changed = false;
            bool full_relex = false;
            std::size_t prefix_tokens = 0;
            std::size_t suffix_tokens = 0;
            std::size_t relexed_tokens = 0;
            std::size_t relexed_bytes = 0;
        };

        Lexer(std::uint32_t fileId, SourceManager &sm, DiagnosticEngine &de,
              Config config = {});

        /// Tokenize one token. Returns `T_EOF` repeatedly at the end.
        Token next();

        /// Tokenize the complete source, replacing the contents of `stream`.
        void lex(TokenStream &stream);

        /// Incrementally update `stream` from the previous source to
        /// `new_source`.
        ///
        /// Tokens before the change are reused, tokens at/after the next safe
        /// line-start fence are shifted and reused, and only the middle window
        /// is re-lexed. If the old and new lexical states cannot be resynced,
        /// the lexer falls back to re-lexing the tail (never the prefix).
        IncrementalResult update(std::string_view new_source, TokenStream &stream);

        void reset();

        [[nodiscard]] std::string_view source() const noexcept { return m_source; }
        [[nodiscard]] std::uint32_t file_id() const noexcept { return m_fileId; }
        [[nodiscard]] std::size_t offset() const noexcept { return m_offset; }
        [[nodiscard]] bool at_end() const noexcept { return m_offset >= m_source.size(); }

        [[nodiscard]] StringInterner &interner() noexcept { return m_interner; }
        [[nodiscard]] const StringInterner &interner() const noexcept
        {
            return m_interner;
        }

    private:
        [[nodiscard]] Token scan_token();
        [[nodiscard]] Token scan_identifier(std::uint32_t start, std::uint8_t flag);
        [[nodiscard]] Token scan_number(std::uint32_t start, std::uint8_t flag);
        [[nodiscard]] Token scan_string(std::uint32_t start, std::uint8_t flag);
        [[nodiscard]] Token scan_char(std::uint32_t start, std::uint8_t flag);
        [[nodiscard]] Token scan_operator(std::uint32_t start, std::uint8_t flag);

        void skip_trivia();
        bool skip_block_comment();

        [[nodiscard]] std::uint32_t decode_escape();
        [[nodiscard]] std::uint32_t peek_byte(std::size_t lookahead = 0) const;

        void report_invalid_utf8(std::uint32_t start, std::uint32_t length);
        void report_digit_separator(std::uint32_t position);

        [[nodiscard]] SourceSpan span(std::uint32_t start, std::uint32_t end) const
        {
            return SourceSpan{m_fileId, start, end};
        }

    private:
        Config m_config;
        DiagnosticEngine &m_DE;

        std::string m_source;
        StringInterner m_interner;
        TokenPool m_scratch;
        std::vector<Token> m_suffix;

        std::uint32_t m_fileId = INVALID_FILE_ID;
        std::uint32_t m_offset = 0;
        std::uint32_t m_limit = 0;
        bool m_at_line_start = true;
    };

} // namespace cora::lexer

#endif // CORA_LEXER_LEXER_H
