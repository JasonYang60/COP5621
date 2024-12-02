#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct CFGNode {
    int id; 
    struct CFGNode* pred1; 
    struct CFGNode* pred2; 
    struct CFGNode* succ1; 
    struct CFGNode* succ2; 
    char* CFGInfo;
    bool visited;   
} CFGNode;

typedef enum {
    LEFTPOINT,   
    MIDDLEPOINT, 
    RIGHTPOINT,   
} Position;

typedef struct Stack {
    char** data;    
    int top;        
    int capacity;   
} Stack;




void create_CFG(struct ast* temp_root, int* root_id, int* dot_id);
void middle_traverse_ast(struct ast* temp_root, char** result, int* size);
void printCFG(CFGNode* CFGroot);

