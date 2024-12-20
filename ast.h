/**
 * Author:    Grigory Fedyukovich, Subhadeep Bhattacharya
 * Created:   09/30/2020
 *
 * (c) FSU
 **/
#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct ast;
struct ast_child {struct ast* id; struct ast_child* next; }; //Dynamic data structure for the list of children
struct ast {int id; char *token; bool is_leaf; int ntoken; char *fun; struct ast_child* child; struct ast* parent; struct ast* next; int isNum; int isVar;}; //Dynamic data structure for the AST. Each node points to a terminal/nonterminal and its children
struct ast_list{struct ast** astList; int length; int capacity; struct ast* top; struct ast* bottom;};


struct ast_list* init_ast_list(int initial_capacity);
int push_ast(struct ast_list* list, struct ast* new_ast);
struct ast* pop_ast(struct ast_list* list); 
bool contains_ast(struct ast_list* list, struct ast* root);
void print_ast_list(const struct ast_list* list);
struct ast_list* find_all_root();

void isNum(int id);
void isVar(int id);

void insert_child(int val);       // Helper methods to specify child/children for the upcoming node
void insert_children(int, ...);   // Variadic version (expects the # of children to be given in the first place)
                                  // Note that it is not necessary to insert all children at once. You can call either insert_child or insert_children many times -- they all will be aggregated and used during the next call of insert_node/insert_node_tmp
void insert_node_tmp(int val, char * token, bool is_leaf, int ntoken);
int insert_node(char* token, int ntoken);

struct ast* find_parent(struct ast* ast_node, struct ast* ast_parent);
struct ast* get_root(struct ast* ast_node);
struct ast* find_ast_node(int id);
struct ast* get_child(struct ast* ast_node, int id);
int get_child_num(struct ast* ast_node);
int visit_ast(int (*f)(struct ast* ast_node));
void print_ast(); //    run "dot -Tpdf ast.dot -o ast.pdf" to create a PDF. Requires a preinstalled graphviz package (https://graphviz.org/download/)
void free_ast();
void traverse_ast_and_print();

// static data structures used in the project:
static int current_node_id = 0;
static struct ast_child* ast_child_root = NULL;
static struct ast* ast_list_root = NULL;

#endif // AST_H
