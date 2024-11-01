%{
    void yyerror (char *s);
    int yylex();
    #include <stdio.h>
    #include "ast.h"
%}

%union {
    int ival;
    char * sval;
}

%start program
%token DEFINEFUN
%token <sval>PRINT
%token <sval> CONST
%token LEFT RIGHT
%token <sval> VARNAME
%token <sval>TRUE
%token <sval>FALSE
%token INT BOOL
%token <ival>GETINT 
%token <sval>GETBOOL
%token ADD MINUS MUL DIV MOD
%token SMALLER GREATER NOTGREATER NOTSMALLER EQUAL
%token AND OR NOT
%token IF
%token LET
%type <ival>expr

%type exprType
%type <sval>var 
%type <sval>fun

%type inputlist
%type <ival>addTerm
%type <ival>minusTerm
%type <ival>multiplyTerm
%type <ival>divTerm
%type <ival>modTerm
%type <ival>oneExprOrMore

%type <ival>exprlist
%%

program : LEFT DEFINEFUN fun inputlist exprType expr RIGHT program {insert_node("define-fun", 1);}
        | LEFT PRINT expr RIGHT {insert_child($3);insert_node("print", 1);}
inputlist   :/* empty */
            | LEFT var exprType RIGHT inputlist
exprType: INT
        | BOOL
expr    : CONST{insert_node($1, 0); }
        | var{insert_node($1, 0); }
        | LEFT GETINT RIGHT{
                $$ = insert_node("getint", 1);
                }
        | TRUE{insert_node($1, 0); }
        | FALSE{insert_node($1, 0); }
        | LEFT GETBOOL RIGHT{ $$ = insert_node("getbool", 1); }
        | addTerm{$$ = $1; }
        | multiplyTerm{$$ = $1; }
        | minusTerm{$$ = $1; }
        | divTerm{$$ = $1; }
        | modTerm{$$ = $1; }
        | LEFT IF expr expr expr RIGHT{insert_child($3); insert_child($4);insert_child($5);$$ = insert_node("if", 1);}
        | LEFT fun exprlist RIGHT{insert_child($3); $$ = insert_node($2, 1);}
        | LEFT LET LEFT var expr RIGHT expr RIGHT {insert_child($7);$$ = insert_node("let", 1);}
        | LEFT EQUAL expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("equal", 1);}
        | LEFT SMALLER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("smaller", 1);}
        | LEFT GREATER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("greater", 1);}
        | LEFT NOTGREATER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("notgreater", 1);}
        | LEFT NOTSMALLER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("notsmallar", 1);}
        | LEFT NOT expr RIGHT{insert_child($3);$$ = insert_node("not", 1);}
        | LEFT AND expr oneExprOrMore RIGHT{insert_child($3);insert_child($4);$$ = insert_node("and", 1);}
        | LEFT OR expr oneExprOrMore RIGHT{insert_child($3);insert_child($4);$$ = insert_node("or", 1);}

       
oneExprOrMore   : expr{$$ = $1;}
                | expr oneExprOrMore{$$ = $1;}
addTerm : LEFT ADD expr oneExprOrMore RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node("add", 0); 
        }
multiplyTerm    : LEFT MUL expr oneExprOrMore RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node("mul", 0); 
        }
minusTerm   : LEFT MINUS expr expr RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node("minus", 0); 
        }
divTerm : LEFT DIV expr expr RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node("div", 0); 
        }
modTerm : LEFT MOD expr expr RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node("mod", 0); 
        }

exprlist    : /* empty */
            | expr exprlist {$$ = $1;}
fun : VARNAME{insert_node($1, 1);}
var : VARNAME{insert_node($1, 1);}
%%

//int main(void) { return yyparse();}

void yyerror (char *s) {printf ("%s\n", s); }
