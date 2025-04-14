#ifndef TOY_LEXER_TOKENS_H
#define TOY_LEXER_TOKENS_H

// %option yylineno   /* Enable line counting */
// % option noyywrap  /* Disable yywrap() function */
// % option stack     /* Enable use of stack to store DFA Table */
// % option verbose   /* Generate Verbose Output */
// % option warn      /* Enable Warnnings */

/*
 import  KEY_TOKEN(T_IMPORT); return T_IMPORT;
 
*/

#include "location.hpp"

typedef enum TokenType
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
  T_FLOAT,
  T_CHAR,
  T_BOOL,
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
  T_VOID,
  T_RETURN,

  // STORAGE CLASSES
  T_STATIC,
  T_EXTERN,
  T_CONST,

  T_UNDERSCORE,
  T_EOF,

} TokenType;

typedef struct Token
{
  TokenType type;
  Location loc;

  union
  {
    char *identifier;
    char character;
    char *string;
    long long integer;
    double number;
    int boolean;
    int token;
  } value;

} Token;

// typedef union
// {
//   Integer integer;
//   Float floatnum;
//   Double doublenum;
// } YYSTYPE;

// extern YYSTYPE yyval;

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *token(TokenType type, char *value, size_t startline, size_t startcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *doc(char *value, size_t startline, size_t endline, size_t startcolumn, size_t endcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *integer(char *value, size_t startline, size_t startcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *number(char *value, size_t startline, size_t startcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *identifier(char *value, size_t startline, size_t startcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *charater(char *value, size_t startline, size_t startcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *boolean(char *value, size_t startline, size_t startcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *bytes(char *value, size_t startline, size_t startcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Token Object
///
/// \param type Type of Token
/// \param value value of Token
/// \param startline Start Line of Token
/// \param endline End Line of Token
/// \param startcolon Token Start Column
/// \param endcolon Token End Column
//
/// \return Token Object
///////////////////////////////////////////////////////////////////////////////
Token *strings(char *value, size_t startline, size_t startcolumn);

///////////////////////////////////////////////////////////////////////////////
/// \brief String of Token Object Type
///
/// \param type Type of Token
///
/// \return String of Token Object Type
///////////////////////////////////////////////////////////////////////////////
const char *token_type_to_string(TokenType type);

///////////////////////////////////////////////////////////////////////////////
/// \brief Print a Token Type
///
/// \param type Token Type to Output
///
///////////////////////////////////////////////////////////////////////////////
void token_type_printf(TokenType type);

///////////////////////////////////////////////////////////////////////////////
/// \brief Token Output Format
///
/// \param Token Token to Output String format
///
/// \return String Format of Token
///////////////////////////////////////////////////////////////////////////////
const char *token_to_string(Token *Token);

///////////////////////////////////////////////////////////////////////////////
/// \brief Print a Token Object
///
/// \param Token Token to Output
///
///////////////////////////////////////////////////////////////////////////////
void token_printf(Token *Token);

/* {intnumber}             {SAVE_INTEGER; return T_INTEGER;} */

#endif // TOY_LEXER_TOKENS_H
