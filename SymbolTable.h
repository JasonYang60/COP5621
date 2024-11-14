#include <stdlib.h>
#include <string.h>
#include <stddef.h> 
#include <stdio.h>

typedef enum {
    UNKNOWNTYPE = 0,
    INTTYPE = 1,
    BOOLTYPE= 2
} DataType;

static int order = 0;

void get_order(char* name);

static int scope = 0;
static int var_called = 0;
static int fun_called = 0;


int get_current_scope();

void entry_scope();

void exit_scope();

struct SymbolTable
{
    char* name;
    DataType type;
    int scope;
    struct SymbolTable* next;
    struct SymbolTable* last;
    
};


struct FuncTable
{
    struct SymbolTable* entry;
    int arities;
    struct FuncTable* next;
};

struct LocalTable
{
    struct SymbolTable* entry;
    struct LocalTable* last;
};

void LocalTablePush();
void printLocal();
static int localEmpty = 1;
void popLocal();
void checkLocal();
// struct SymbolTable* createSymbolTableEntry(char* name);
void destroySymbolTableEntry(struct SymbolTable* entry);

void printSymbolTable(struct SymbolTable* sym);
void changeName(char* name);

static const int tableListLimit = 128;

static struct SymbolTable* root;
static struct SymbolTable* current;

static struct FuncTable* funcRoot;
static struct FuncTable* currentFunc;

static struct LocalTable* localRoot;
static struct LocalTable* currentLocal;


void printStack();
void push(char* name);

static int tableList_scope = 0;
static int arityCounter = 0;

void incCounter();
void addFunc();
int checkSemanticError(struct FuncTable* f, struct SymbolTable* top);
void printFunc();

void removeSymbol(struct SymbolTable* ptr);
