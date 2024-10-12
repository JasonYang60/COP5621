%{
    void yyerror (char *s);
    int yylex();
    #include <stdio.h>
%}

%union {
    int ival;
    char *sval;
}

%start program
%token DEFINEFUN
%token PRINT
%token <ival> CONST
%token LEFT RIGHT
%token <sval> VARNAME
%token TRUE FALSE
%token INT BOOL
%token GETINT GETBOOL
%token ADD MINUS MUL DIV MOD
%token SMALLER GREATER NOTGREATER NOTSMALLER EQUAL
%token AND OR NOT
%token IF
%token LET
%type expr
%type term
%type fla
%type exprType
%type var fun

%type inputlist
%type addTerm
%type minusTerm
%type multiplyTerm
%type divTerm
%type modTerm
%type oneTermOrMore
%type oneFlaOrMore
%type exprlist
%%

program : LEFT DEFINEFUN fun inputlist exprType expr RIGHT program 
        | LEFT PRINT expr RIGHT 
inputlist   :/* empty */
            | LEFT var exprType RIGHT inputlist
exprType: INT
        | BOOL
expr    : term
        | fla
term    : CONST
        | var
        | LEFT GETINT RIGHT
        | addTerm
        | multiplyTerm
        | minusTerm
        | divTerm
        | modTerm
        | LEFT IF fla term term RIGHT
        | LEFT fun exprlist RIGHT
        | LEFT LET LEFT var expr RIGHT term RIGHT 
oneTermOrMore   : term
                | term oneTermOrMore
addTerm : LEFT ADD term oneTermOrMore RIGHT
multiplyTerm    : LEFT MUL term oneTermOrMore RIGHT
minusTerm   : LEFT MINUS term term RIGHT
divTerm : LEFT DIV term term RIGHT
modTerm : LEFT MOD term term RIGHT
fla     : TRUE
        | FALSE
        | var
        | LEFT GETBOOL RIGHT
        | LEFT EQUAL term term RIGHT
        | LEFT SMALLER term term RIGHT
        | LEFT GREATER term term RIGHT
        | LEFT NOTGREATER term term RIGHT
        | LEFT NOTSMALLER term term RIGHT
        | LEFT NOT fla RIGHT
        | LEFT AND fla oneFlaOrMore RIGHT
        | LEFT OR fla oneFlaOrMore RIGHT
        | LEFT IF fla fla fla RIGHT
        | LEFT fun exprlist RIGHT
        | LEFT LET LEFT var expr RIGHT fla RIGHT
oneFlaOrMore    : fla
                | fla oneFlaOrMore
exprlist    : /* empty */
            | exprlist expr
fun : VARNAME
var : VARNAME
%%

int main(void) { return yyparse();}

void yyerror (char *s) {printf ("%s\n", s); }
