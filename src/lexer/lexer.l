%{

#include <stack>
#include <string>

#include "ast/node.hpp"
#include "lexer/tokens.hpp"
#include "lexer/source.hpp"
#include "parser/parser.hpp"

#define YYSTYPE Token   /* Define own Token Structure */

#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define SAVE_INTEGER yylval.integer = std::stoll(std::string(yytext, yyleng))
#define SAVE_DOUBLE yylval.number = std::stod(std::string(yytext, yyleng))
#define SAVE_BOOLEAN yylval.boolean = std::string(yytext, yyleng) == "true" ? 1 : 0
#define SAVE_CHARATER yylval.charater = std::string(yytext, yyleng).c_str()[0]

#define TOKEN(t) (yylval.token = t)

#define KEY_TOKEN(t) TOKEN(t)
#define SYM_TOKEN(t) TOKEN(t)
#define OPP_TOKEN(t) TOKEN(t)

extern "C" int yywrap() { return 1; }

#define YYERROR_VERBOSE 1

int yyerror(char const * s )
{
    printf("ERROR %s in '%s' at line %d col %d\n", s, yytext, yylineno, yycolumn);
    printf("  parsed %s %d.%d-%d.%d\n", yylloc.file_name.c_str(), yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column);
    return 1;
}

/* yychar - The current lookahead token */
/* yylvar - The value of current token */
/* yylloc - The location of current token */
/* yyin - Function Pointer for Input */
/* yyout - Function Pointer for Output */

std::string g_str;

std::stack<sourcefile> sourceFile;

size_t yycolumn  = 1 ;       /* To Track Column Number */


#define YY_USER_ACTION do { \
        if( yylloc.end_line < yylineno )\
        {\
            yycolumn = 1 ; \
        }\
        yylloc.start_line = yylloc.endl_line = yylineno; \
        yylloc.start_column = yycolumn;\
        yylloc.end_column = yycolumn + (int)yyleng - 1; \
        yycolumn += (int)yyleng; \
    } while(0) ;


%}

%option yylineno    /* Enable line counting */
%option noyywrap    /* Disable yywrap() function */
%option stack       /* Enable use of stack to store DFA Table */
%option verbose     /* Generate Verbose Output */
%option warn        /* Enable Warnnings */

%s normal           /* %s - Define Exclusive Start Condition */
%x str qstr         /* %x - Define Inclusive Start Condition */
%x comment
%x include

whitespace  [ \f\t]*
linecomment #[^\r\n]*
ignore      {whitespace} + (\\\r?\n'|{whitespace})({linecomment})?
name        \w+

hexnumber   0[xX](?:_?[0-9a-fA-F])+
binnumber   0[bB](?:_?[01])+
octnumber   0[oO](?:_?[0-7])+
decnumber   (?:0(?:_?0)*|[1-9](?:_?[0-9])*)
exponent    [eE][-+]?[0-9](?:_?[0-9])*

intnumber   ({hexnumber}|{binnumber}|{octnumber}|{decnumber})
pointfloat  ([0-9](?:_?[0-9])*\.(?:[0-9](?:_?[0-9])*)?|\.[0-9](?:_?[0-9])*')({exponent})?
expfloat    [0-9](?:_?[0-9])*{exponent}
floatnumber (pointfloat|exponent)
imagnumber  ([0-9](?:_?[0-9])*[jJ]|{floatnumber}[jJ])
number      ({imagnumber}|{floatnumber}|{intnumber})

%%

\n              { yylval = CreateToken(EOL,yytext,yylineno,column);yylineno++; column = 1;return T_EOL;}    
.               {column++;}         

"if"             { yylval = CreateToken(KEYWORD,yytext,yylineno,column); column+=yyleng;return T_IF;}

import              BEGIN(include);
<include>[ \t]*        /* eat the whitespace */
<include>[^ \t\n\r]+   {
                        /* got the include file name */
                        std::string fileName = yytext;
                        std::size_t pos = fileName.find(".toy");

                        if( pos == std::string::npos ) {
                            fileName += ".toy";
                        }

                        for( auto libpath : filePaths ) {
                            yyin = fopen( (libpath + fileName).c_str() , "r" );
                            if( yyin )
                                break;
                        }

                        if ( !yyin ) {
                            printf( "%s in %s line %d\n", (std::string("Failed to load import file ") + fileName).c_str(), sourceFile.top().c_str(), yylineno );
                            parsing_error = 1;
                            yyterminate();
                        } else {
                            sourceFile.push(yytext);
                            lineNumber.push(yylineno);
                            yylineno = yycolumn = 1;
                            yylloc.first_line = yylloc.first_column =  yylloc.last_line = yylloc.last_column = 1;
                            yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE ));
                        }
                        BEGIN(normal);
                    }

\@\{                BEGIN(comment);
<comment>\@\}.*     BEGIN(normal);
<comment>.*         ;/* eat everything */
<comment>\n         ;/* eat everything */

\"      g_str = ""; BEGIN(str);
'       g_str = ""; BEGIN(qstr);

<str>\" {   BEGIN(normal);
            yylval.string = new std::string(g_str);
            return T_STRING;
        }

<qstr>' {   BEGIN(normal);
            yylval.string = new std::string(g_str);
            return T_STRING;
        }

<str,qstr>\\n   g_str += "\n";
<str,qstr>\\t   g_str += "\t";
<str,qstr>\\r   g_str += "\r";
<str>\\\"       g_str += "\"";
<qstr>\\\'      g_str += "'";

<str,qstr>\\(.|\n)  g_str += yytext[1];

<str>[^\\\"]+  g_str += std::string(yytext);
<qstr>[^\\']+  g_str += std::string(yytext);


<normal>"\n"    { yycolumn = 1; }

<<EOF>>         /* Define an Action for the EOF Condition */ 
                {
                   if( curr_indents.size() > 1 ) {
                        curr_indents.pop();
                        return TOKEN(UNINDENT);
                   }
                   if(lineNumber.size() > 1 ) {
                       yypop_buffer_state();
                       sourceFile.pop();
                       yylineno = lineNumber.top();
                       lineNumber.pop();
                   } else {
                        yyterminate();
                   }
                }

{name}                  SAVE_TOKEN; return T_IDENTIFIER;
{intnumber}             SAVE_INTEGER; return T_INTEGER;
{floatnumber} 			SAVE_TOKEN; return T_DOUBLE;

{ignore}                /* ignore spaces and others */;
{linecomment}           /* single line comments ignore */;

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
