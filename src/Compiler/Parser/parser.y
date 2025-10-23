%code requires {

/* alert the parser that we have our own definition */
# define YYLTYPE_IS_DECLARED 1

}

%debug 				      /* Enable Debugging */
%locations  		    /* Enable Location Tracking */
%verbose    
%start expression   /* Specifies the start symbol of grammer */

/* Define custom YYLTYPE struct to hold location information */
// %define api.location.type 

%{

  #include <cstdlib>
  #include <cstdio>

  #include "Cora/AST/ASTNode.hpp"
  #include "Cora/Parser/Token.hpp"
  #include "Cora/AST/Statements.hpp"
  #include "Cora/AST/Expressions.hpp"

  cora::ast::Program *programNode; /* the top level root node of our final AST */

  extern int yylex();
  void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }

  #define YYDEBUG 1 /* For debugging */

  # define YYLLOC_DEFAULT(Current, Rhs, N)          		                  \
      do                                                                  \
        if (N)                                                            \
          {                                                               \
            (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
            (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
            (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
            (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
            (Current).file_name = fileNames.top();            			      \
          }                                                               \
        else                                                              \
          {                                                               \
            (Current).first_line   = (Current).last_line   =              \
              YYRHSLOC (Rhs, 0).last_line;                                \
            (Current).first_column = (Current).last_column =              \
              YYRHSLOC (Rhs, 0).last_column;                              \
            (Current).file_name = fileNames.top();            			      \
          }                                                               \
      while (0);

%}



/* Define a union type for the parser's value stack.
   Represents the many different ways we can access our data
*/

%union {
  ast::Node* node;
  ast::Module* module;
  parser::Token* token;
  ast::Program* program;
  ast::Statement* statement;
  ast::Expression* expression;

}

/* Define our terminal symbols (tokens). This should
   match our lexer.l lex file. We also define the node type
   they represent.
*/

%token <token>  T_NEWLINE
%token <token>  T_INDENT
%token <token>  T_DEDENT
%token <token>  T_IGNORE
%token <token>  T_COMMENT

%token <token>  T_IDENTIFIER
%token <token>  T_INTEGER
%token <token>  T_NUMBER
%token <token>  T_CHARATER
%token <token>  T_BOOLEAN
%token <token>  T_BYTE
%token <token>  T_DOC
%token <token>  T_STRING
%token <token>  T_RAWSTRING
%token <token>  T_BYTESTRING
%token <token>  T_BYTERAWSTRING

%token <token>  T_LPAR
%token <token>  T_RPAR
%token <token>  T_LSQB
%token <token>  T_RSQB
%token <token>  T_COLON
%token <token>  T_COMMA
%token <token>  T_SEMI
%token <token>  T_UNDERSCORE

%token <token>  T_PLUS
%token <token>  T_MINUS
%token <token>  T_STAR
%token <token>  T_SLASH
%token <token>  T_VBAR
%token <token>  T_AMPER
%token <token>  T_LESS
%token <token>  T_GREATER
%token <token>  T_EQUAL
%token <token>  T_DOT
%token <token>  T_PERCENT
%token <token>  T_LBRACE
%token <token>  T_RBRACE
%token <token>  T_EQEQUAL
%token <token>  T_NOTEQUAL
%token <token>  T_LESSEQUAL
%token <token>  T_GREATEREQUAL
%token <token>  T_TILDE
%token <token>  T_LEFTSHIFT
%token <token>  T_RIGHTSHIFT
%token <token>  T_DOUBLESTAR
%token <token>  T_PLUSEQUAL
%token <token>  T_MINEQUAL
%token <token>  T_STAREQUAL
%token <token>  T_SLASHEQUAL
%token <token>  T_PERCENTEQUAL
%token <token>  T_AMPEREQUAL
%token <token>  T_VBAREQUAL
%token <token>  T_CIRCUMFLEXEQUAL
%token <token>  T_LEFTSHIFTEQUAL
%token <token>  T_RIGHTSHIFTEQUAL
%token <token>  T_DOUBLESTAREQUAL
%token <token>  T_DOUBLESLASH
%token <token>  T_DOUBLESLASHEQUAL
%token <token>  T_AT
%token <token>  T_ATEQUAL
%token <token>  T_RARROW
%token <token>  T_ELLIPSIS
%token <token>  T_COLONEQUAL
%token <token>  T_EXCLAMATION

%token <token>  T_INT
%token <token>  T_FLOAT
%token <token>  T_DOUBLE
%token <token>  T_CHAR
%token <token>  T_BOOL
%token <token>  T_VOID

%token <token>  T_SIGNED
%token <token>  T_UNSIGNED
%token <token>  T_SHORT
%token <token>  T_LONG

%token <token>  T_IF
%token <token>  T_ELSE
%token <token>  T_SWITCH
%token <token>  T_CASE
%token <token>  T_DEFAULT
%token <token>  T_FOR
%token <token>  T_WHILE
%token <token>  T_GOTO
%token <token>  T_DO
%token <token>  T_BREAK
%token <token>  T_CONTINUE

%token <token>  T_CLASS
%token <token>  T_STRUCT
%token <token>  T_UNION
%token <token>  T_PRIVATE
%token <token>  T_PROTECTED
%token <token>  T_PUBLIC
%token <token>  T_SELF

%token <token>  T_TRY
%token <token>  T_CATCH
%token <token>  T_THROW

%token <token>  T_SIZEOF
%token <token>  T_TYPEID
%token <token>  T_TYPEOF
%token <token>  T_OFFSETOF

%token <token>  T_NEW
%token <token>  T_DELETE

%token <token>  T_FUN
%token <token>  T_RETURN

%token <token>  T_STATIC
%token <token>  T_EXTERN
%token <token>  T_CONST

%token <token>  T_EOF

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type identifier) we are really
   calling an (Identifier*). It makes the compiler happy.
*/

%type <program> program modules
%type <module> module stmts block
%type <block> block stmts
%type <token> identifier
%type <expression> expr literals
%type <statement> stmt

/* Operator precedence for mathematical operators */

%left T_PLUS T_MINUS
%left T_STAR T_SLASH
%left T_AND TOR T_NOT
%left T_EQ
%left T_NE
%left T_LT
%left T_LE
%left T_GT
%left T_GE
%left T_INC T_DEC

/* $$ - Refers to the value of the LHS */
/* $1, $2 - Refers to the value of the RHS symbols */


%%

program : %empty { programNode = new ast::Program(); }
        | modules { programNode = $1; }
        ;

modules : module { $$ = new ast::Program(); $$->PushBack($<module>1); }
        | modules module { $1->PushBack($<module>2); }
        ;

module  : %empty { $$ = new ast::Module(); }
        | stmts { $$ = $1; }
        ;

stmts : stmt { $$ = new ast::Module(); $$->PushBack($<stmt>1); }
      | stmts stmt { $1->PushBack($<stmt>2); }
      ;

stmt : var_dec
     | func_dec
     | class_dec
     | expr { $$ = new ast::Expression($1); }
     ;

literals : T_INTEGER { $$ = new ast::Integer($1); }
         | T_DOUBLE { $$ = new ast::Float($1); }
         | T_STRING { $$ = new ast::String(*$1); delete $1; }
         | T_BOOL { $$ = new ast::Boolean($1); }
         | T_CHAR { $$ = new ast::Byte($1); }
         ;

%%
