%option yylineno
%option noyywrap
%option stack
%option verbose
%option warn
%option nounistd

%{

#include <stdio.h>

enum TokenType
{

  // DATA TYPES
  T_IDENTIFIER,
  T_INTEGER,

  // OPERATOR
  T_PLUS,
  T_MINUS,
  T_STAR,
  T_SLASH,

}

%}

%%

[ \t\n]         {/* whitespace */}
[a-z][a-z0-9]*  { return T_IDENTIFIER; }

"+"             { return T_IDENTIFIER; }
"-"             { return T_IDENTIFIER; }
"*"             { return T_IDENTIFIER; }
"/"             { return T_IDENTIFIER; }

[0-9]+          { return T_INTEGER; }

%%

int main(void) {
    yyin = stdin;
    int kind;

    while (kind = yylex() != 0){

        printf("%d:%s",kind,yytext);
    }
    return 0;
}
