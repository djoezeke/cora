#ifndef CORA_PARSER_TOKEN_H
#define CORA_PARSER_TOKEN_H

#include "Cora/Basic/SourceLocation.h"
#include <string>

namespace cora
{
	using namespace basic;

	namespace parser
	{

		enum class TokenKind
		{

			T_UNKNOW = 257, // Parser generators reserve 0-256 for char literals

			// others
			T_NEWLINE,
			T_INDENT,
			T_DEDENT,
			T_IGNORE,
			T_COMMENT,

			// DATA TYPES
			T_IDENTIFIER,
			T_INTEGER,
			T_NUMBER,
			T_CHARATER,
			T_BOOLEAN,
			T_BYTE,
			T_DOC,
			T_STRING,
			T_RAWSTRING,
			T_BYTESTRING,
			T_BYTERAWSTRING,

			// symbols
			T_LPAR,
			T_RPAR,
			T_LSQB,
			T_RSQB,
			T_COLON,
			T_COMMA,
			T_SEMI,
			T_UNDERSCORE,

			// operators
			T_PLUS,
			T_MINUS,
			T_STAR,
			T_SLASH,
			T_VBAR,
			T_AMPER,
			T_LESS,
			T_GREATER,
			T_EQUAL,
			T_DOT,
			T_PERCENT,
			T_LBRACE,
			T_RBRACE,
			T_EQEQUAL,
			T_NOTEQUAL,
			T_LESSEQUAL,
			T_GREATEREQUAL,
			T_TILDE,
			T_LEFTSHIFT,
			T_RIGHTSHIFT,
			T_DOUBLESTAR,
			T_PLUSEQUAL,
			T_MINEQUAL,
			T_STAREQUAL,
			T_SLASHEQUAL,
			T_PERCENTEQUAL,
			T_AMPEREQUAL,
			T_VBAREQUAL,
			T_CIRCUMFLEXEQUAL,
			T_LEFTSHIFTEQUAL,
			T_RIGHTSHIFTEQUAL,
			T_DOUBLESTAREQUAL,
			T_DOUBLESLASH,
			T_DOUBLESLASHEQUAL,
			T_AT,
			T_ATEQUAL,
			T_RARROW,
			T_ELLIPSIS,
			T_COLONEQUAL,
			T_EXCLAMATION,

			// keywords
			T_INT,
			T_FLOAT,
			T_DOUBLE,
			T_CHAR,
			T_BOOL,
			T_VOID,

			// MODIFIERS
			T_SIGNED,
			T_UNSIGNED,
			T_SHORT,
			T_LONG,

			// CONTROL FLOW
			T_IF,
			T_ELSE,
			T_SWITCH,
			T_CASE,
			T_DEFAULT,
			T_FOR,
			T_WHILE,
			T_GOTO,
			T_DO,
			T_BREAK,
			T_CONTINUE,

			//  CLASS
			T_CLASS,
			T_STRUCT,
			T_UNION,
			T_PRIVATE,
			T_PROTECTED,
			T_PUBLIC,
			T_SELF,

			// EXCEPTIONS
			T_TRY,
			T_CATCH,
			T_THROW,

			// OPERATORS
			T_SIZEOF,
			T_TYPEID,
			T_TYPEOF,
			T_OFFSETOF,

			// MEMORY
			T_NEW,
			T_DELETE,

			// FUNCTION
			T_FUN,
			T_RETURN,

			// STORAGE CLASSES
			T_STATIC,
			T_EXTERN,
			T_CONST,

			T_EOF,

		} TokenType;

		class Token
		{
		public:
			Token() = default;

			Token(TokenKind tokenkind, std::string tokentext) { m_Kind = tokenkind, m_TokenText = tokentext; };

			Token(TokenKind tokenkind) { m_Kind = tokenkind; };

			Token(TokenKind tokenkind, std::string kinsdtring)
			{
				m_Kind = tokenkind;
				m_KindString = kinsdtring;
			};

			/**
			 * @brief Compute a representation of the token.
			 * @return String representation of the token
			 */
			std::string Repr() { return GetText(); };

			/**
			 * @brief Return the token kind
			 *
			 * @return TokenKind
			 */
			TokenKind GetTokenKind() const { return m_Kind; };

			/**
			 * @brief Return the token kind string
			 *
			 * @return const std::string&
			 */
			std::string GetTokenKindString() const { return ""; };

			bool Is(TokenKind tokenkind) const { return m_Kind == tokenkind; };
			bool IsNot(TokenKind tokenkind) const { return m_Kind != tokenkind; };

			bool IsAny(TokenKind tokenkind) const
			{
				return Is(tokenkind);
			}

			template <typename... T>
			bool IsAny(TokenKind kind1, TokenKind kind2, T... tokenkinds) const
			{
				if (Is(kind1))
					return true;
				return IsAny(kind2, tokenkinds...);
			}

			template <typename... T>
			bool IsNotAny(TokenKind tokenkind, T... tokenkinds) const
			{
				return !IsAny(tokenkind, tokenkinds...);
			}

			std::string GetText() const { return m_TokenText; };

			unsigned int GetLenght() const { return m_TokenText.size(); };

			/**
			 * @brief Get the start position of the node
			 *
			 * @return Position
			 */
			SourceLocation GetStartPosition() const noexcept { return m_Range.GetStart(); };

			/**
			 * @brief Get the end position of the node
			 *
			 * @return Position
			 */
			SourceLocation GetEndPosition() const noexcept { return m_Range.GetEnd(); };

			/**
			 * @brief Get the source range of the node
			 *
			 * @return SourceRange
			 */
			SourceRange GetSourceRange() const { return m_Range; };

			void SetText(std::string text) { m_TokenText = text; };

			/**
			 * @brief Set the token kind object
			 *
			 * @param tokenkind
			 */
			void SetTokenKind(TokenKind tokenkind) noexcept { m_Kind = tokenkind; };

			/**
			 * @brief Set the token kind string
			 *
			 * @param kindstring
			 */
			void SetTokenKindString(std::string kindstring) noexcept { m_KindString = kindstring; };

			/**
			 * @brief Set the Node start position
			 *
			 * @param start
			 */
			void SetStartPosition(SourceLocation start) noexcept { m_Range.SetStart(start); };

			/**
			 * @brief Set the Node end position
			 *
			 * @param end
			 */
			void SetEndPosition(SourceLocation end) noexcept { m_Range.SetEnd(end); };

			/**
			 * @brief Set the Node source range.
			 *
			 * @param range
			 */
			void SetSourceRange(SourceRange range) noexcept { m_Range = range; };

		private:
			TokenKind m_Kind;
			SourceRange m_Range;
			std::string m_TokenText;
			std::string m_KindString;
		};

	} // namespace parser

} // namespace cora

#endif // CORA_PARSER_TOKEN_H
