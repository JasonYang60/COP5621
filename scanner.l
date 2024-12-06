%{
#include <stdio.h>
#include <errno.h>
#include "y.tab.h"
%}

%%

\(      {return LEFT;}
\)      {return RIGHT;}
\+        {return ADD;}
\-        {return MINUS;}
\*        {return MUL;}
div        {return DIV;}
mod        {return MOD;}

\<   {return SMALLER;}
\>   {return GREATER;}
\<=  {return NOTGREATER;}
\>=  {return NOTSMALLER;}
=   {return EQUAL;}

if          {return IF;}
true        {return TRUE;}
false          {return FALSE;}
and         {return AND;}
or          {return OR;}
not          {return NOT;}
let          {return LET;}
int     {return INT;}
bool          {return BOOL;}

define\-fun     { return DEFINEFUN;}
get\-int        {yylval.sval = yytext;return VARNAME;}
get\-bool        {yylval.sval = yytext;return VARNAME;}

print          { return PRINT;}

[0-9]+     {yylval.ival = atoi(yytext); return CONST;}
[a-zA-Z][a-zA-Z0-9]*  {yylval.sval = yytext;return VARNAME;}

[0-9][^ \t\n\(\)]+  {printf("Error: invalid variable name: '%s'\n", yytext); perror("Lexical Error."); exit(-1);}

([0-9a-zA-Z]+)([^ \t\n\(\)0-9a-zA-Z]+)([0-9a-zA-Z]*) {printf("Error: invalid variable name: '%s'\n", yytext); perror("Lexical Error."); exit(-1);}
([0-9a-zA-Z]*)([^ \t\n\(\)0-9a-zA-Z]+)([0-9a-zA-Z]+) {printf("Error: invalid variable name: '%s'\n", yytext); perror("Lexical Error."); exit(-1);}

[ \t\n]+    { /* ignored */ }
.         { printf("Error: Unexpected character: '%s'\n", yytext); perror("Lexical Error."); exit(-1);}
%%

//int main(void) {
//    yylex();
//    return 0;
//}
