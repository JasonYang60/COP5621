%{
    void yyerror (char *s);
    int yylex();
    #include <stdio.h>
    #include <stdlib.h>
    #include "ast.h"
    #include "SymbolTable.h"
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
%type <ival>deffunint localvarint
%type <ival>varint funint

%type <ival>inputlist
%type <ival>inputlistentry
%type <ival>addTerm
%type <ival>minusTerm
%type <ival>multiplyTerm
%type <ival>divTerm
%type <ival>modTerm
%type <ival>oneExprOrMore
%type <ival>definefun
%type <ival>exprlist
%type <ival>entry
%type <ival>empty
%%

program : definefun program  {get_order("start");}
        | entry PRINT expr RIGHT {
                insert_children(2, $1, $3); insert_node("entry", 1);
                checkPrint();

        }
entry : LEFT {$$ = insert_node("print", 0);
                push("print");
                addFunc();
                }
definefun : LEFT DEFINEFUN deffunint inputlistentry retexprType expr RIGHT {
                insert_children(4, $3, $4, $5, $6); 
                $$ = insert_node("definefun", 0);

                get_order("def fun");

                //char* str = (char*)malloc(64 * sizeof(char)); strcpy(str, "unknown_name");
                //struct SymbolTable* sym = createSymbolTableEntry(str);
                //printSymbolTable(sym);
                //printf("%d\n", get_current_scope());
                addFunc();
                checkFunc();
                }

inputlistentry  : inputlist {insert_child($1); $$ = insert_node("inputlist", 0); }
inputlist: empty {$$ = $1;}
        | LEFT varint exprType RIGHT  {
                $$ = $2; 
                get_order("inputlist");
                incCounter();
                } 
        | LEFT varint exprType RIGHT inputlist {
                $$ = $2; 
                insert_child($5);
                get_order("inputlist");
                incCounter();
                } 
exprType: INT {setType(1);}
        | BOOL  {setType(2);}
retexprType: INT  {$$ = insert_node_type("ret INT", 3, 1); }
        | BOOL {$$ = insert_node_type("ret BOOL", 4, 2); }
expr    : CONST {char* str = (char*)malloc(12 * sizeof(char)); sprintf(str, "%d", $1); $$ = insert_node_type(str,1,1); }
        | varint { $$ = $1;}
        | LEFT GETINT RIGHT{
                $$ = insert_node_type("GETINT", 6, 1);
                }
        | TRUE{$$ = insert_node_type("true", 7, 2); }
        | FALSE{$$ = insert_node_type("false", 8, 2); }
        | LEFT GETBOOL RIGHT{ $$ = insert_node_type("GETBOOL", 9, 2); }
        | addTerm{$$ = $1;}
        | multiplyTerm{$$ = $1; }
        | minusTerm{$$ = $1; }
        | divTerm{$$ = $1; }
        | modTerm{$$ = $1; }
        | LEFT IF expr expr expr RIGHT{insert_child($3); insert_child($4);insert_child($5);$$ = insert_node("if", 10);}
        | LEFT funint exprlist RIGHT{ 
                insert_children(2, $2, $3); 
                $$ = insert_node("call func", 1);

                printFunc();
                checkCall();
                }
        | LEFT LET LEFT localvarint expr RIGHT expr RIGHT {
                insert_children(3, $4, $5, $7);
                $$ = insert_node("let", 12);

                get_order("let");
                checkLocal();
                }
        | LEFT EQUAL expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node_type("=", 13, 2);}
        | LEFT SMALLER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node_type("<", 14, 2);}
        | LEFT GREATER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node_type(">", 15, 2);}
        | LEFT NOTGREATER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node_type("<=", 16, 2);}
        | LEFT NOTSMALLER expr expr RIGHT{insert_child($3); insert_child($4);$$ = insert_node_type(">=", 17, 2);}
        | LEFT NOT expr RIGHT{insert_child($3);$$ = insert_node_type("not", 18, 2);}
        | LEFT AND expr oneExprOrMore RIGHT{insert_child($3);insert_child($4);$$ = insert_node_type("and", 19, 2);}
        | LEFT OR expr oneExprOrMore RIGHT{insert_child($3);insert_child($4);$$ = insert_node_type("or", 20, 2);}

       
oneExprOrMore   : expr{$$ = $1;}
                | expr oneExprOrMore{$$ = $1; insert_child($2);}
addTerm : LEFT ADD expr oneExprOrMore RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node_type("+", 0, 1); 
        }
multiplyTerm    : LEFT MUL expr oneExprOrMore RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node_type("*", 0, 1); 
        }
minusTerm   : LEFT MINUS expr expr RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node_type("-", 0, 1); 
        }
divTerm : LEFT DIV expr expr RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node_type("div", 0, 1); 
        }
modTerm : LEFT MOD expr expr RIGHT{
        insert_child($3); 
        insert_child($4);
        $$ = insert_node_type("mod", 0, 1); 
        }

exprlist    : empty {$$ = $1;}
            | expr {$$ = $1; get_order("exprlist"); incArgCounter(); }
            | expr exprlist {
                $$ = $1; insert_child($2);
                get_order("exprlist");
                incArgCounter();
                }
      
funint : fun {char* str = (
        char*)malloc(12 * sizeof(char)); strcpy(str, $1);  $$ = insert_node(str, 1); get_order(str); 
        push(str);
        LocalTablePush();
        }
deffunint : fun {
        char* str = (char*)malloc(12 * sizeof(char)); strcpy(str, $1);  $$ = insert_node(str, 1); 
        
        get_order(str);
        push(str);
        }
varint : var {
        char* str = (char*)malloc(12 * sizeof(char)); strcpy(str, $1);  $$ = insert_node(str, 1);
        
        get_order(str); 
        push(str);
        
        }
localvarint : var {
        char* str = (char*)malloc(12 * sizeof(char)); strcpy(str, $1);  $$ = insert_node(str, 1);
        
        get_order(str); 
        push(str);
        LocalTablePush();
        }
empty : /* empty */ {$$ = insert_node("none", 1); }
fun : VARNAME 
var : VARNAME 
%%
//
//
//
//int main(void) { return yyparse();}

void yyerror (char *s) {printf ("%s\n", s); }