%option yylineno
%option noyywrap
%option stack
%option verbose
%option warn

%{

#include <stack>
#include <string>

#include "src/lexer/tokens.hpp"
#include "src/lexer/source.hpp"
#include "src/parser/parser.hpp"

#define YYSTYPE Token   /* Define own Token Structure */

#define SAVE_IDENTIFIER yylval.token = identifier(yytext, yylineno, yycolumn)
#define SAVE_INTEGER yylval.token = integer(yytext, yylineno, yycolumn)
#define SAVE_NUMBER yylval.token = number(yytext, yylineno, yycolumn)
#define SAVE_BOOLEAN yylval.token = boolean(yytext, yylineno, yycolumn)
#define SAVE_CHARATER yylval.token = charater(yytext, yylineno, yycolumn)
#define SAVE_STRING yylval.token = strings(yytext, yylineno, yycolumn)
#define SAVE_BYTE yylval.token = bytes(yytext, yylineno, yycolumn)
#define SAVE_DOC yylval.token = doc(yytext, yylineno, yycolumn)
#define SAVE_TOKEN(type) yylval.token = token(type, yytext, yylineno, yycolumn)

#define KEY_TOKEN(type) SAVE_TOKEN(type)
#define SYM_TOKEN(type) SAVE_TOKEN(type)
#define OPP_TOKEN(type) SAVE_TOKEN(type)

#define YYERROR_VERBOSE 1

int yyerror(char const * s )
{
    printf("ERROR %s in '%s' at line %d col %d\n", s, yytext, yylineno, yycolumn);
    printf("  parsed %d.%d-%d.%d\n",yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column);
    return 1;
}

/* yychar - The current lookahead token */
/* yylvar - The value of current token */
/* yylloc - The location of current token */
/* yyin - Function Pointer for Input */
/* yyout - Function Pointer for Output */

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


/* %s - Define Exclusive Start Condition */
/* %x - Define Inclusive Start Condition */

whitespace  [ \f\t]*
linecomment #[^\r\n]*
ignore      {whitespace}(\\\r?\n'|{whitespace})({linecomment})?
name        \w+

hexnumber   0[xX](?:_?[0-9a-fA-F])+
binnumber   0[bB](?:_?[01])+
octnumber   0[oO](?:_?[0-7])+
decnumber   ?:0(?:_?0)*|[1-9](?:_?[0-9])*
exponent    [eE][-+]?[0-9](?:_?[0-9])*

intnumber   {hexnumber}|{binnumber}|{octnumber}|{decnumber}
pointfloat  ([0-9](?:_?[0-9])*\.(?:[0-9](?:_?[0-9])*)?|\.[0-9](?:_?[0-9])*')({exponent})?
expfloat    [0-9](?:_?[0-9])*{exponent}
floatnumber (pointfloat|exponent)
imagnumber  ([0-9](?:_?[0-9])*[jJ]|{floatnumber}[jJ])
number      {imagnumber}|{floatnumber}|{intnumber}

charater    \'[.]\'

%%

{ignore}                /* ignore spaces and others */;
{linecomment}           /* single line comments ignore */;

{name}                  {SAVE_IDENTIFIER; return T_IDENTIFIER;}

true 		        	{SAVE_BOOLEAN; return T_BOOLEAN;}
false        			{SAVE_BOOLEAN; return T_BOOLEAN;}

{charater} 			    {SAVE_CHARATER; return T_CHARATER;}
{floatnumber} 			{SAVE_STRING; return T_STRING;}
{floatnumber} 			{SAVE_BYTE; return T_BYTE;}
{floatnumber} 			{SAVE_DOC; return T_DOC;}


int         {KEY_TOKEN(T_IMPORT); return T_INT;}
float       {KEY_TOKEN(T_IMPORT); return T_FLOAT;}
double      {KEY_TOKEN(T_IMPORT); return T_DOUBLE;}
bool        {KEY_TOKEN(T_IMPORT); return T_BOOL;}
void        {KEY_TOKEN(T_IMPORT); return T_VOID;}

signed      {KEY_TOKEN(T_IMPORT); return T_SIGNED;}
unsigned    {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
short       {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
long        {KEY_TOKEN(T_IMPORT); return T_IMPORT;}

if          {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
else        {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
switch      {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
case        {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
default     {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
for         {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
while       {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
goto        {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
do          {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
break       {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
continue    {KEY_TOKEN(T_IMPORT); return T_IMPORT;}

class       {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
union       {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
private     {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
protected   {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
public      {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
self        {KEY_TOKEN(T_IMPORT); return T_IMPORT;}

try         {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
catch       {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
throw       {KEY_TOKEN(T_IMPORT); return T_IMPORT;}

sizeof      {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
typeid      {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
typeof      {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
offsetof    {KEY_TOKEN(T_IMPORT); return T_IMPORT;}

new         {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
delete      {KEY_TOKEN(T_IMPORT); return T_IMPORT;}

fun         {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
void        {KEY_TOKEN(T_IMPORT); return T_IMPORT;}

static      {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
extern      {KEY_TOKEN(T_IMPORT); return T_IMPORT;}
const       {KEY_TOKEN(T_IMPORT); return T_IMPORT;}


"*"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"*="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"+"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"+="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"-"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"-="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"/"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"/="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"//"   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"//="  {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"%"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"%="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"**"   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"**="  {OPP_TOKEN(T_IMPORT); return T_IMPORT;}

"=="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
">"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"<"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
">="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"<="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}

"<<"   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"<<="  {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
">>"   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
">>="  {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"^"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"^="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"&"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"&="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"!"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"!="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"|"    {OPP_TOKEN(T_IMPORT); return T_IMPORT;}
"|="   {OPP_TOKEN(T_IMPORT); return T_IMPORT;}


"."    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"["    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"]"    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"("    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
")"    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"{"    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"}"    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"~"    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
","    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"->"   {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"::"   {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
":"    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
":="   {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
";"    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"="    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"@"    {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"@="   {SYM_TOKEN(T_IMPORT); return T_IMPORT;}
"..."  {SYM_TOKEN(T_IMPORT); return T_IMPORT;}

%%
