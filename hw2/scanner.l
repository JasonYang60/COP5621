%{
#include <stdio.h>
#include <errno.h>
%}

%%

\(      { printf("left parenthesis:\t%s\n", yytext); }
\)      { printf("right parenthesis:\t%s\n", yytext); }
(\+|\-|\*)        {printf("arithmetic operator:\t%s\n", yytext); }
(<|>|<=|>=|=)   { printf("arithmetic comparison:\t%s\n", yytext); }

if          { printf("conditional operator:\t%s\n", yytext); }
define\-fun     { printf("function definition keyword:\t%s\n", yytext); }
get\-int        { printf("predefined function:\t%s\n", yytext); }
print          { printf("program entry point:\t%s\n", yytext); }

[0-9]+     { printf("arithmetic const:\t%s\n", yytext); }
[a-zA-Z][a-zA-Z0-9]*  { printf("variable/function name:\t%s\n", yytext); }

[0-9][^ \t\n\(\)]+  { printf("Error: invalid variable name: '%s'\n", yytext); perror("Syntax Error.");}

([0-9a-zA-Z]+)([^ \t\n\(\)0-9a-zA-Z]+)([0-9a-zA-Z]*) { printf("Error: invalid variable name: '%s'\n", yytext); perror("Syntax Error.");}
([0-9a-zA-Z]*)([^ \t\n\(\)0-9a-zA-Z]+)([0-9a-zA-Z]+) { printf("Error: invalid variable name: '%s'\n", yytext); perror("Syntax Error.");}

[ \t\n]+    { /* ignored */ }
.         { printf("Error: Unexpected character: '%s'\n", yytext); perror("Syntax Error.");}
%%

int main(void) {
    yylex();
    return 0;
}