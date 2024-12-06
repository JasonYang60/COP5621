#ifndef CFG_H
#define CFG_H

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
    struct CFGNode* traverse;
    char* value;
    int ori_id;
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


CFGNode* createNode(int id);

void create_CFG(struct ast* temp_root, int* root_id, int* dot_id);
void middle_traverse_ast(struct ast* temp_root, char** result, int* size);
void printCFGList(FILE* fp);
int printCFG(CFGNode* node);

extern CFGNode* cfgNodeList[1024];
extern int cfgNodeList_idx;

typedef int (*traverseFunc)(CFGNode*);
void dfs(CFGNode* node, traverseFunc f);
void reset_dfs(CFGNode* node);
void dot_CFG(CFGNode* CFG_root,FILE* fp);

#endif // CFG_H
