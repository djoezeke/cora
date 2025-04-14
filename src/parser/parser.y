%code requires {

# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */

}

%debug 				/* Enable Debugging */
%locations  		/* Enable Location Tracking */
%verbose    
%start expression 	/* Specifies the start symbol of grammer */

/* Define custom YYLTYPE struct to hold location information */
// %define api.location.type 

%{

#include <cstdlib>
#include <cstdio>

#include "src/lexer/location.hpp"
#include "src/lexer/tokens.hpp"
#include "src/ast/node.hpp"
#include "src/ast/node.hpp"

extern int yylex();
void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }


# define YYLLOC_DEFAULT(Current, Rhs, N)          		                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
          (Current).file_name = fileNames.top();            			\
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
          (Current).file_name = fileNames.top();            			\
        }                                                               \
    while (0);

%}



/* Define a union type for the parser's value stack.
   Represents the many different ways we can access our data
*/
%union {
    Token* token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
*/

%token T_NUMBER

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */

%type <token> expression 
%type <token> comparison 

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV


/* $$ - Refers to the value of the LHS */
/* $1, $2 - Refers to the value of the RHS symbols */


%%

expression : T_NUMBER {
	printf("line: %d, Col: %d \n", yylloc.start_line, yylloc.start_column);
}

%%
