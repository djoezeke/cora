%{

#include <stack>
#include <string>

#include "ast/node.hpp"
#include "lexer/tokenize.hpp"
#include "parser/parser.hpp"

#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define SAVE_INTEGER yylval.integer = std::stoll(std::string(yytext, yyleng))
#define SAVE_DOUBLE yylval.number = std::stod(std::string(yytext, yyleng))
#define SAVE_BOOLEAN yylval.boolean = std::string(yytext, yyleng) == "true" ? 1 : 0
#define SAVE_CHARATER yylval.charater = std::string(yytext, yyleng).c_str()[0]

#define TOKEN(t) (yylval.token = t)

#define KEY_TOKEN(t) TOKEN(t)
#define SYM_TOKEN(t) TOKEN(t)
#define OPP_TOKEN(t) TOKEN(t)

#ifdef _MSC_VER
int isatty(int) {return 0;};
#endif

extern "C" int yywrap() { return 1; }

int yycolumn = 1;

#define YYERROR_VERBOSE 1
int yyerror(char const * s )
{
    printf("ERROR %s in '%s' at line %d col %d\n", s, yytext, yylineno, yycolumn);
    printf("  parsed %s %d.%d-%d.%d\n", yylloc.file_name.c_str(), yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column);
    return 1;
}

std::string g_str;
int parsing_error = 0;

std::stack<std::string> fileNames;
std::stack<int> lineNo;
std::vector<std::string> libPaths;

#define YY_USER_ACTION do { \
    if( yylloc.last_line < yylineno ) yycolumn = 1 ; \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + (int)yyleng - 1; \
    yycolumn += (int)yyleng; \
    yylloc.file_name = fileNames.top(); \
    } while(0) ;

int first_time = 1 ;

%}

%option yylineno

%s normal 
%x str qstr
%x comment
%x include

whitespace = [ \f\t]*
comment = #[^\r\n]*
ignore = {whitespace} + (\\\r?\n'|{whitespace})({comment})?
name = \w+

hexnumber = 0[xX](?:_?[0-9a-fA-F])+
binnumber = 0[bB](?:_?[01])+
octnumber = 0[oO](?:_?[0-7])+
decnumber = (?:0(?:_?0)*|[1-9](?:_?[0-9])*)
exponent = [eE][-+]?[0-9](?:_?[0-9])*

intnumber = ({hexnumber}|{binnumber}|{octnumber}|{decnumber})
pointfloat = ([0-9](?:_?[0-9])*\.(?:[0-9](?:_?[0-9])*)?|\.[0-9](?:_?[0-9])*')({exponent})?
expfloat = [0-9](?:_?[0-9])*{exponent}
floatnumber = (pointfloat|Expfloat)
imagnumber = ([0-9](?:_?[0-9])*[jJ]|{floatnumber}[jJ])
number = ({imagnumber}|{floatnumber}|{intnumber})

%%

<<EOF>>         {
                   if( curr_indents.size() > 1 ) {
                        curr_indents.pop();
                        return TOKEN(UNINDENT);
                   }
                   if(lineNo.size() > 1 ) {
                       yypop_buffer_state();
                       fileNames.pop();
                       yylineno = lineNo.top();
                       lineNo.pop();
                   } else {
                        yyterminate();
                   }
                }
                  

[a-zA-Z_][a-zA-Z0-9_&%\$\?\-]*  SAVE_TOKEN; return T_IDENTIFIER;
-?[0-9]+                        SAVE_INTEGER; return T_INTEGER;
[0-9]+\.[0-9]* 			        SAVE_TOKEN; return T_DOUBLE;

\r                              /* cr are ignored */
[ \t\n]                         /* ignore */;
#.*                             /* single line comments ignore */

"true"                  SAVE_BOOLEAN; return T_BOOL;
"false"                 SAVE_BOOLEAN; return T_BOOL;

"extern"                return KEY_TOKEN(TEXTERN);
"return"				return KEY_TOKEN(TRETURN);
"if"                    return KEY_TOKEN(TIF);
"else"                  return KEY_TOKEN(TELSE);
"return"                return KEY_TOKEN(TRETURN);
"not"                   return KEY_TOKEN(TNOT);
"and"                   return KEY_TOKEN(TAND);
"or"                    return KEY_TOKEN(TOR);
"def"                   return KEY_TOKEN(TDEF);
"var"                   return KEY_TOKEN(TVAR);
"while"                 return KEY_TOKEN(TWHILE);

"*"                     return OPP_TOKEN(T_STAR);
"*="                    return OPP_TOKEN(T_STAREQUAL);
"+"                     return OPP_TOKEN(T_PLUS);
"+="                    return OPP_TOKEN(T_PLUSEQUAL);
"-"                     return OPP_TOKEN(T_MINUS);
"-="                    return OPP_TOKEN(T_MINEQUAL);
"/"                     return OPP_TOKEN(T_SLASH);
"/="                    return OPP_TOKEN(T_SLASHEQUAL);
"//"                    return OPP_TOKEN(T_DOUBLESLASH);
"//="                   return OPP_TOKEN(T_DOUBLESLASHEQUAL);
"%"                     return OPP_TOKEN(T_PERCENT);
"%="                    return OPP_TOKEN(T_PERCENTEQUAL);
"**"                    return OPP_TOKEN(T_DOUBLESTAR);
"**="                   return OPP_TOKEN(T_DOUBLESTAREQUAL);

"=="                    return OPP_TOKEN(T_EQEQUAL);
">"                     return OPP_TOKEN(T_GREATER);
"<"                     return OPP_TOKEN(T_LESS);
">="                    return OPP_TOKEN(T_GREATEREQUAL);
"<="                    return OPP_TOKEN(T_LESSEQUAL);


"<<"                    return OPP_TOKEN(T_LEFTSHIFT);
"<<="                   return OPP_TOKEN(T_LEFTSHIFTEQUAL);
">>"                    return OPP_TOKEN(T_RIGHTSHIFT);
">>="                   return OPP_TOKEN(T_RIGHTSHIFTEQUAL);
"^"                     return OPP_TOKEN(T_CIRCUMFLEX);
"^="                    return OPP_TOKEN(T_CIRCUMFLEXEQUAL);
"&"                     return OPP_TOKEN(T_AMPER);
"&="                    return OPP_TOKEN(T_AMPEREQUAL);
"!"                     return OPP_TOKEN(T_NOT);
"!="                    return OPP_TOKEN(T_NOTEQUAL);
"|"                     return OPP_TOKEN(T_VBAR);
"|="                    return OPP_TOKEN(T_VBAREQUAL);

"."                     return SYM_TOKEN(T_DOT);
"["                     return SYM_TOKEN(T_LSQB);
"]"                     return SYM_TOKEN(T_RSQB);
"("                     return SYM_TOKEN(T_LPAR);
")"                     return SYM_TOKEN(T_RPAR);
"{"                     return SYM_TOKEN(T_LBRACE);
"}"                     return SYM_TOKEN(T_RBRACE);
"~"                     return SYM_TOKEN(T_TILDE);
","                     return SYM_TOKEN(T_COMMA);
"->"                    return SYM_TOKEN(T_RARROW);
"::"                    return SYM_TOKEN(T_DOUBLECOLON);
":"                     return SYM_TOKEN(T_COLON);
":="                    return SYM_TOKEN(T_COLONEQUAL);
";"                     return SYM_TOKEN(T_SEMI);
"="                     return SYM_TOKEN(T_EQUAL);
"@"                     return SYM_TOKEN(T_AT);
"@="                    return SYM_TOKEN(T_ATEQUAL);
"..."                   return SYM_TOKEN(T_ELLIPSIS);

.                       printf("line %d, len %d Unknown token %s !\n", yylineno, yyleng, yytext); yyterminate();

%%
