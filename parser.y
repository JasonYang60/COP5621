%{
    void yyerror (char *s);
    int yylex();
    #include <stdio.h>
    #include <stdlib.h>
    #include "ast.h"
%}

%union {
    int ival;
    char * sval;
}

%start program
%token DEFINEFUN
%token PRINT
%token <ival> CONST
%token LEFT RIGHT
%token <sval> VARNAME
%token <sval>TRUE
%token <sval>FALSE
%token INT BOOL
%token <sval>GETINT 
%token <sval>GETBOOL
%token ADD MINUS MUL DIV MOD
%token SMALLER GREATER NOTGREATER NOTSMALLER EQUAL
%token AND OR NOT
%token IF
%token LET
%token <sval> STRCONST
%type <ival>expr

%type <ival>retexprType
%type <ival> exprType
%type <sval>var fun
%type <ival>varint

%type <ival>inputlist
%type <ival>addTerm
%type <ival>minusTerm
%type <ival>multiplyTerm
%type <ival>divTerm
%type <ival>modTerm
%type <ival>oneExprOrMore
%type <ival>funname
%type <ival>printmain

%type <ival>exprlist
%%

program : LEFT DEFINEFUN funname inputlist retexprType expr RIGHT program  { insert_child($3); insert_child($4); insert_child($5); insert_child($6); insert_node("DEF-FUN", 0);}
        | LEFT PRINT expr RIGHT {insert_child($3);insert_node("PRINT", 1);}
funname : fun {char* str = (char*)malloc(12 * sizeof(char)); strcpy(str, $1);  $$ = insert_node(str, 1);}
inputlist: LEFT varint exprType RIGHT  {$$ = $2;} 
        | LEFT varint exprType RIGHT inputlist { $$ = $2; insert_child($5);} 
varint : var {char* str = (char*)malloc(12 * sizeof(char)); strcpy(str, $1);  $$ = insert_node(str, 1);}
exprType: INT 
        | BOOL 
retexprType: INT {$$ = insert_node("ret INT", 3); }
        | BOOL {$$ = insert_node("ret BOOL", 4);}
expr    : CONST {char* str = (char*)malloc(12 * sizeof(char)); sprintf(str, "%d", $1); $$ = insert_node(str,1); }
        | var { char* str = (char*)malloc(12 * sizeof(char)); strcpy(str, $1);  $$ = insert_node(str, 1);}
        | LEFT GETINT RIGHT{
                $$ = insert_node("GETINT", 6);
                }
        | TRUE{$$ = insert_node($1, 7); }
        | FALSE{$$ = insert_node($1, 8); }
        | LEFT GETBOOL RIGHT{ $$ = insert_node("GETBOOL", 9); }
        | addTerm{$$ = $1;}
        | multiplyTerm{$$ = $1; }
        | minusTerm{$$ = $1; }
        | divTerm{$$ = $1; }
        | modTerm{$$ = $1; }
        | LEFT IF expr expr expr RIGHT{insert_child($3); insert_child($4);insert_child($5);$$ = insert_node("if", 10);}
        | LEFT fun exprlist RIGHT{ insert_child($3); char* str = (char*)malloc(12 * sizeof(char)); strcpy(str, $2);  $$ = insert_node("name", 1);}
        | LEFT LET LEFT varint expr RIGHT expr RIGHT {insert_child($4);insert_child($5);insert_child($7);$$ = insert_node("let", 12);}
        | LEFT EQUAL expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("equal", 13);}
        | LEFT SMALLER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("smaller", 14);}
        | LEFT GREATER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("greater", 15);}
        | LEFT NOTGREATER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("notgreater", 16);}
        | LEFT NOTSMALLER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node("notsmallar", 17);}
        | LEFT NOT expr RIGHT{insert_child($3);$$ = insert_node("not", 18);}
        | LEFT AND expr oneExprOrMore RIGHT{insert_child($3);insert_child($4);$$ = insert_node("and", 19);}
        | LEFT OR expr oneExprOrMore RIGHT{insert_child($3);insert_child($4);$$ = insert_node("or", 20);}

       
oneExprOrMore   : expr{$$ = $1;}
                | expr oneExprOrMore{$$ = $1; insert_child($2);}
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

exprlist    : expr {$$ = $1;}
            | exprlist expr {$$ = $2; insert_child($1);}

fun : VARNAME 
var : VARNAME 
%%

//{char* space_pos = strchr(str, ' '); if (space_pos != NULL) {*space_pos = '\0'; }}

//int main(void) { return yyparse();}

void yyerror (char *s) {printf ("%s\n", s); }

