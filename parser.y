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

%type exprType
%type var fun

%type inputlist
%type addTerm
%type minusTerm
%type multiplyTerm
%type divTerm
%type modTerm
%type oneExprOrMore

%type exprlist
%%

program : LEFT DEFINEFUN fun inputlist exprType expr RIGHT program 
        | LEFT PRINT expr RIGHT 
inputlist   :/* empty */
            | LEFT var exprType RIGHT inputlist
exprType: INT
        | BOOL
expr    : CONST
        | var
        | LEFT GETINT RIGHT
        | TRUE
        | FALSE
        | LEFT GETBOOL RIGHT
        | addTerm
        | multiplyTerm
        | minusTerm
        | divTerm
        | modTerm
        | LEFT IF expr expr expr RIGHT
        | LEFT fun exprlist RIGHT
        | LEFT LET LEFT var expr RIGHT expr RIGHT 
        | LEFT EQUAL expr expr RIGHT
        | LEFT SMALLER expr expr RIGHT
        | LEFT GREATER expr expr RIGHT
        | LEFT NOTGREATER expr expr RIGHT
        | LEFT NOTSMALLER expr expr RIGHT
        | LEFT NOT expr RIGHT
        | LEFT AND expr oneExprOrMore RIGHT
        | LEFT OR expr oneExprOrMore RIGHT

       
oneExprOrMore   : expr
                | expr oneExprOrMore
addTerm : LEFT ADD expr oneExprOrMore RIGHT
multiplyTerm    : LEFT MUL expr oneExprOrMore RIGHT
minusTerm   : LEFT MINUS expr expr RIGHT
divTerm : LEFT DIV expr expr RIGHT
modTerm : LEFT MOD expr expr RIGHT

exprlist    : /* empty */
            | exprlist expr
fun : VARNAME
var : VARNAME
%%

int main(void) { return yyparse();}

void yyerror (char *s) {printf ("%s\n", s); }
