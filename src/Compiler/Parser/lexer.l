%option yylineno
%option noyywrap
%option stack
%option verbose
%option warn

%{

#include <stack>
#include <string>

#include "Cora/Parser/Token.hpp"
#include "Cora/Basic/SourceManager.hpp"
#include "Cora/Basic/SourceLocation.hpp"

#define YYSTYPE Token   /* Define own Token Structure */

#define SAVE_IDENTIFIER yylval.token = identifier(yytext, yylineno, yycolumn)
#define SAVE_INTEGER yylval.token = integer(yytext, yylineno, yycolumn)
#define SAVE_NUMBER yylval.token = number(yytext, yylineno, yycolumn)
#define SAVE_BOOLEAN yylval.token = boolean(yytext, yylineno, yycolumn)
#define SAVE_TOKEN yylval.token = charater(yytext, yylineno, yycolumn)
#define SAVE_TOKEN yylval.token = strings(yytext, yylineno, yycolumn)
#define SAVE_TOKEN yylval.token = bytes(yytext, yylineno, yycolumn)
#define SAVE_TOKEN yylval.token = doc(yytext, yylineno, yycolumn)

#define SAVE_TOKEN(tokenkind) yylval.token = Token(tokenkind, #tokenkind , std::string(yytext), yylineno, yycolumn)
#define SAVE_TOKEN(type) SAVE_TOKEN(type)
#define SAVE_TOKEN(type) SAVE_TOKEN(type)
#define SAVE_TOKEN(type) SAVE_TOKEN(type)

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
std::string g_str;             

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

%s normal           
%x str qstr         
%x comment
%x include

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

\n              {yylineno++; column = 1;}    
.               {yycolumn++;}         

import              BEGIN(incl);
<incl>[ \t]*        /* eat the whitespace */
<incl>[^ \t\n\r]+   { /* got the include file name */
                    std::string fileName = yytext;
                    std::size_t pos = fileName.find(".cora");
                    if( pos == std::string::npos ) {
                        fileName += ".cora";
                    }
                    for( auto libpath : libPaths ) {
                        yyin = fopen( (libpath + fileName).c_str() , "rb" );
                        if( yyin )
                            break;
                    }
                    if ( ! yyin ) {
                       printf( "%s in %s line %d\n", (std::string("Failed to load import file ") + fileName).c_str(), fileNames.top().c_str(), yylineno );
                       parsing_error = 1;
                       yyterminate();
                    } else {
                       fileNames.push(yytext);
                       lineNo.push(yylineno);
                       yylineno = yycolumn = 1;
                       yylloc.first_line = yylloc.first_column =  yylloc.last_line = yylloc.last_column = 1;
                       yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE ));
                    }
                    BEGIN(normal);
                    }
     
\@\{                BEGIN(comment);
<comment>\@\}.*     BEGIN(normal);
<comment>.*         ;/* ignore everything */
<comment>\n         ;/* ignore everything */

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

<<EOF>>         /* Define an Action for the EOF Condition */ {
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
\r                      /* cr are ignored */
{ignore}                /* ignore spaces and others */;
{linecomment}           /* single line comments ignore */;

{name}                  {SAVE_TOKEN(T_IDENTIFIER); return T_IDENTIFIER;}

true 		        	{SAVE_BOOLEAN; return T_BOOLEAN;}
false        			{SAVE_BOOLEAN; return T_BOOLEAN;}

{charater} 			    {SAVE_TOKEN; return T_CHARATER;}
{floatnumber} 			{SAVE_TOKEN; return T_STRING;}
{floatnumber} 			{SAVE_TOKEN; return T_BYTE;}
{floatnumber} 			{SAVE_TOKEN; return T_DOC;}


int         {SAVE_TOKEN(T_IMPORT); return T_INT;}
float       {SAVE_TOKEN(T_IMPORT); return T_FLOAT;}
double      {SAVE_TOKEN(T_IMPORT); return T_DOUBLE;}
bool        {SAVE_TOKEN(T_IMPORT); return T_BOOL;}
void        {SAVE_TOKEN(T_IMPORT); return T_VOID;}

signed      {SAVE_TOKEN(T_IMPORT); return T_SIGNED;}
unsigned    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
short       {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
long        {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

if          {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
else        {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
switch      {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
case        {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
default     {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
for         {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
while       {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
goto        {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
do          {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
break       {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
continue    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

class       {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
union       {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
private     {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
protected   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
public      {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
self        {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

try         {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
catch       {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
throw       {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

sizeof      {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
typeid      {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
typeof      {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
offsetof    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

new         {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
delete      {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

fun         {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
void        {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}


static      {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
extern      {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
const       {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}


"*"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"*="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"+"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"+="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"-"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"-="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"/"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"/="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"//"   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"//="  {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"%"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"%="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"**"   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"**="  {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

"=="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
">"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"<"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
">="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"<="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

"<<"   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"<<="  {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
">>"   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
">>="  {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"^"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"^="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"&"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"&="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"!"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"!="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"|"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"|="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}


"."    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"["    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"]"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"("    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
")"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"{"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"}"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"~"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
","    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"->"   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"::"   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
":"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
":="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
";"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"="    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"@"    {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"@="   {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}
"..."  {SAVE_TOKEN(T_IMPORT); return T_IMPORT;}

%%
