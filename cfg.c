
#include "ast.h"
#include "cfg.h"


const int MAX_CAPACITY = 100;
CFGNode* cfgNodeList[1024] = {NULL};
int cfgNodeList_idx = 0;


CFGNode* createNode(int id) {
    CFGNode* node = (CFGNode*)malloc(sizeof(CFGNode)); 
    cfgNodeList[id] = node;
    node->id = id;
    node->pred1 = NULL;
    node->pred2 = NULL;
    node->succ1 = NULL;
    node->succ2 = NULL;
    node->visited = false;

    node->traverse = NULL;
    node->value = 0;
    
    node->CFGInfo = (char*)malloc(MAX_CAPACITY + 1);
    node->value = (char*)malloc(MAX_CAPACITY + 1);
    node->ori_id = -1;

    return node;
}

void freeNode(CFGNode* node) {
    if (node) {
        free(node->CFGInfo);  
        free(node);  
    }
}

int printCFG(CFGNode* node){
    if (node == NULL) {
        return 1;
    }
    char* name1 = node->succ1 ? node->succ1->CFGInfo : "null";
    char* name2 = node->succ2 ? node->succ2->CFGInfo : "null";
    char* name3 = node->pred1 ? node->pred1->CFGInfo : "null";
    char* name4 = node->pred2 ? node->pred2->CFGInfo : "null";
    printf("Node ID: %d, Info: %s \n\tsucc1: %s, succ2: %s\n\tpred1: %s, pred2 = %s\n", node->id, (node->CFGInfo ? node->CFGInfo : "NULL"), name1, name2, name3, name4);

    char* name5 = node->traverse ? node->traverse->CFGInfo : "null";
    printf("\ttraverse: %s\n", name5);
    return 0;
}


Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;  
    stack->data = (char**)malloc(capacity * sizeof(char*));
    return stack;
}


void push(Stack* stack, char* str) {
    stack->top++;
    stack->data[stack->top] = (char*)malloc((strlen(str) + 1) * sizeof(char));
    strcpy(stack->data[stack->top], str);
}

char* pop(Stack* stack) {
    if (stack->top == -1) {
        return NULL; 
    }

    char* popped = stack->data[stack->top];  
    stack->top--; 
    return popped;
}

void printStack(Stack* stack) {
    for (int i = 0; i <= stack->top; i++) {
        printf("%s\n", stack->data[i]);
    }
}

void freeStack(Stack* stack) {
    for (int i = 0; i <= stack->top; i++) {
        free(stack->data[i]); 
    }
    free(stack->data); 
    free(stack);  
}


const char* positionToString(Position pos) {
    switch (pos) {
        case LEFTPOINT: return "LEFTPOINT";
        case MIDDLEPOINT: return "MIDDLEPOINT";
        case RIGHTPOINT: return "RIGHTPOINT";
        default: return "UNKNOWN";
    }
}


                
void dot_printConnections(FILE *fp, CFGNode* node, CFGNode* next1, CFGNode* next2) {
    if(strcmp(node->CFGInfo, "EXIT") == 0) {return;}
    if (next1) {
        fprintf(fp,"%d->%d\n", node->id, next1->id);
    }
    if (next2) {
        fprintf(fp,"%d->%d\n", node->id, next2->id);
    }
}

void dot_dfs(FILE *fp,CFGNode* node) {
    if (node == NULL) {
        return; 
    }
    fprintf(fp, "%d [label=\"%s\", fontname=\"monospace\"];\n ", node->id, node->CFGInfo);
    dot_printConnections(fp,node, node->succ1, node->succ2);

    if (node->succ1) {
        if(node->succ1->pred2 && (node->succ1->pred2 == node)) {
            return;
        }
        dot_dfs(fp,node->succ1);
    }
    if (node->succ2) {
        dot_dfs(fp,node->succ2);
    }
    
}


void dot_CFG(CFGNode* CFG_root,FILE* fp){
    CFGNode* tempCFGNode = CFG_root;
    dot_dfs(fp,tempCFGNode);
}




void dfs(CFGNode* node, traverseFunc f) {
    if (node == NULL) {
        return; 
    }

    // node->visited = true;
    if(f(node)) {
        perror("error\n");
        exit(1);
    }

    if (node->succ1) {
        if(node->succ1->pred2 && (node->succ1->pred1 == node)) {
            return;
        }
        dfs(node->succ1, f);
    }
    if (node->succ2) {
        dfs(node->succ2, f);
    }

}

void reset_dfs(CFGNode* node) {
    if (node == NULL || !node->visited) {
        return; 
    }
    node->visited = false;

    if (node->succ1) {
        reset_dfs(node->succ1);
    }
    if (node->succ2) {
        reset_dfs(node->succ2);
    }
}

void middle_traverse_ast(struct ast* temp_root, char** result, int* size){
    //printf("%s\n",temp_root->token);

    result[*size] = strdup(temp_root->token);
    (*size)++;

    if(temp_root->child == NULL) return;
    else{
      struct ast_child* child = temp_root->child;
      int child_num = get_child_num(temp_root);
      for (int i = 0; i < child_num; i++){
          middle_traverse_ast(child->id,result,size);
          child = child->next;
      }
    }
}


void create_CFG(struct ast* temp_root, int* root_id, int* dot_id){
    struct CFGNode* CFG_root = createNode(*root_id);
    struct CFGNode* CFG_current = CFG_root;
    struct CFGNode* CFG_IF_FRONT[MAX_CAPACITY];
    struct CFGNode* CFG_IF_BEHIND[MAX_CAPACITY];
    int CFG_IF_FRONT_num = -1;
    int CFG_IF_BEHIND_num = -1;
    CFG_IF_FRONT[0] = NULL;
    CFG_IF_BEHIND[0] = NULL;
    struct CFGNode* CFG_left;
    struct CFGNode* CFG_right;

    // const char* filename = "cfg.dot";
    // FILE* file = fopen(filename, "w");
    // fclose(file);

    int call_func_num = 0;

    int if_symbol = 0;

    Position current_position = LEFTPOINT;
    Position* positionList = (Position*)malloc(MAX_CAPACITY * sizeof(Position));
    int positionNum = 0;

    Stack* operationStack; 
    Stack* leftStack; 
    Stack* rightStack; 
    Stack* middleStack;
    Stack* letStack; 
    operationStack = createStack(MAX_CAPACITY);
    leftStack = createStack(MAX_CAPACITY);
    rightStack = createStack(MAX_CAPACITY);
    middleStack = createStack(MAX_CAPACITY);
    letStack = createStack(MAX_CAPACITY);

    int size = 0;              
    char* AST_result[MAX_CAPACITY]; 
    middle_traverse_ast(temp_root, AST_result, &size);

    // for (int i = 0;i < size;i++){
    //     printf("%s\n",AST_result[i]);
    // }



    for (int i = 0; i < size; i++) {   
    // operation add
        if (strcmp(AST_result[i],"main") == 0){
            push(operationStack,AST_result[i]);
        }

        if (strcmp(AST_result[i],"definefun") == 0){
            push(operationStack,AST_result[i]);
            continue;
        }

        // if (strcmp(AST_result[i],"inputlist") == 0){
        //     push(operationStack,AST_result[i]);
        //     continue;
        // }

        // char temp_AST_result[4];
        // strncpy(temp_AST_result, AST_result[i], 3);
        // temp_AST_result[3] = '\0';
        // if (strcmp(temp_AST_result,"ret") == 0){
        //     push(operationStack,AST_result[i]);
        //     continue;
        // }

        if ((strcmp(AST_result[i],"add") == 0) || (strcmp(AST_result[i],"mul") == 0) || (strcmp(AST_result[i],"minus") == 0) || (strcmp(AST_result[i],"div") == 0) || (strcmp(AST_result[i],"mod") == 0) ||
        (strcmp(AST_result[i],"equal") == 0) || (strcmp(AST_result[i],"smaller") == 0) || (strcmp(AST_result[i],"greater") == 0) || (strcmp(AST_result[i],"notgreater") == 0) || (strcmp(AST_result[i],"notsmallar") == 0) || 
         (strcmp(AST_result[i],"and") == 0) || (strcmp(AST_result[i],"or") == 0)){
            positionList[positionNum] = current_position;
            positionNum += 1;
            positionList[positionNum] = '\0';
            push(operationStack,AST_result[i]);
            current_position = LEFTPOINT;
            continue;
        }

        if (strcmp(AST_result[i],"let") == 0){
            positionList[positionNum] = current_position;
            positionNum += 1;
            positionList[positionNum] = '\0';
            push(operationStack,AST_result[i]);
            current_position = LEFTPOINT;
            continue;
        }

        if (strcmp(AST_result[i],"call func") == 0){
            positionList[positionNum] = current_position;
            positionNum += 1;
            positionList[positionNum] = '\0';
            push(operationStack,AST_result[i]);
            current_position = LEFTPOINT;
            continue;
        }

        if (strcmp(AST_result[i],"not") == 0){
            positionList[positionNum] = current_position;
            positionNum += 1;
            positionList[positionNum] = '\0';
            push(operationStack,AST_result[i]);
            current_position = RIGHTPOINT;
            continue;
        } 

        if (strcmp(AST_result[i],"if") == 0){
            positionList[positionNum] = current_position;
            positionNum += 1;
            positionList[positionNum] = '\0';
            push(operationStack,AST_result[i]);

            // CFG_IF_FRONT[CFG_IF_FRONT_num] = CFG_current->pred1;
            // CFG_IF_FRONT_num++;

            current_position = LEFTPOINT;

            continue;
        }
        

        
// CFG_add_part

        if (operationStack->top == -1) {
            continue;
        }
        char* currentOperation = operationStack->data[operationStack->top]; // consider the current operation

        if (strcmp(currentOperation,"definefun") == 0){
            strcpy(CFG_current->CFGInfo,AST_result[i]);
            (*root_id)++;
            struct CFGNode* CFG_next = createNode(*root_id);
            char* tempOperationReturn = pop(operationStack);
            CFG_current ->succ1 = CFG_next;
            CFG_next ->pred1 = CFG_current;
            CFG_current = CFG_next;
            continue;
        }

        if (strcmp(currentOperation,"main") == 0){
            strcpy(CFG_current ->CFGInfo,AST_result[i]);
            (*root_id)++;
            struct CFGNode* CFG_next = createNode(*root_id);
            char* tempOperationReturn = pop(operationStack);
            CFG_current ->succ1 = CFG_next;
            CFG_next ->pred1 = CFG_current;
            CFG_current = CFG_next;
            continue;
        }

// add mul minus div mod equal smaller greater notgreater notsmallar not and or

        if ((strcmp(currentOperation,"add") == 0) || (strcmp(currentOperation,"mul") == 0) || (strcmp(currentOperation,"minus") == 0) || (strcmp(currentOperation,"div") == 0) || (strcmp(currentOperation,"mod") == 0) ||
        (strcmp(currentOperation,"equal") == 0) || (strcmp(currentOperation,"smaller") == 0) || (strcmp(currentOperation,"greater") == 0) || (strcmp(currentOperation,"notgreater") == 0) || (strcmp(currentOperation,"notsmallar") == 0) || 
         (strcmp(currentOperation,"and") == 0) || (strcmp(currentOperation,"or") == 0)){
            if (current_position == LEFTPOINT){
                int len_vname = snprintf(NULL, 0, "v%d", CFG_current->id);
                int len_CFGInfo = len_vname + strlen(":=") + strlen(AST_result[i]);
                char* vname = malloc(len_vname + 1);
                char* CFGInfo = malloc(len_CFGInfo + 1);
                sprintf(vname, "v%d", CFG_current->id);         
                sprintf(CFGInfo, "%s:=%s", vname, AST_result[i]); 

                push(leftStack,vname);
                current_position = RIGHTPOINT;

                strcpy(CFG_current->CFGInfo,CFGInfo);
                (*root_id)++;
                struct CFGNode* CFG_next = createNode(*root_id);
                CFG_current ->succ1 = CFG_next;
                CFG_next ->pred1 = CFG_current;
                CFG_current = CFG_next;
                continue;
            }
            else if(current_position == RIGHTPOINT){
                int len_vname1 = snprintf(NULL, 0, "v%d", CFG_current->id);
                int len_CFGInfo1 = len_vname1 + strlen(":=") + strlen(AST_result[i]);
                char* vname1 = malloc(len_vname1 + 1);
                char* CFGInfo1 = malloc(len_CFGInfo1 + 1);
                sprintf(vname1, "v%d", CFG_current->id);         
                sprintf(CFGInfo1, "%s:=%s", vname1, AST_result[i]); 

                push(rightStack,vname1);

                strcpy(CFG_current->CFGInfo,CFGInfo1);
                (*root_id)++;
                struct CFGNode* CFG_next1 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next1;
                CFG_next1 ->pred1 = CFG_current;
                CFG_current = CFG_next1;

            } 
        }

        if (strcmp(currentOperation,"not") == 0){
            if (current_position == RIGHTPOINT){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id - 1);
                int len_CFGInfo2 = len_vname2 + strlen(":=") + strlen(AST_result[i]);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo2 + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s", vname2, AST_result[i]);

                push(rightStack,vname2);

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                // printf("CFGinfo: %s\n",CFG_current->CFGInfo);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2; 
            }
        }

        if (strcmp(currentOperation,"let") == 0){
            if (current_position == LEFTPOINT){
                int len_CFGInfo1 = strlen(AST_result[i]);
                char* CFGInfo1 = malloc(len_CFGInfo1 + 1);
                sprintf(CFGInfo1,AST_result[i]);
                push(leftStack,CFGInfo1);
                current_position = RIGHTPOINT;
                continue;
            }

            else if (current_position == RIGHTPOINT){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id - 1);
                int len_CFGInfo2 = len_vname2 + strlen(":=") + strlen(AST_result[i]);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo2 + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s", vname2, AST_result[i]);

                push(rightStack,vname2);

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2; 
            }

        }

        if (strcmp(currentOperation,"call func") == 0){
            if (current_position == LEFTPOINT){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                int len_CFGInfo2 = len_vname2 + strlen(":=") + strlen(AST_result[i]);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo2 + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s", vname2, AST_result[i]);

                push(leftStack,vname2);

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2; 

                current_position = MIDDLEPOINT;
                continue;
            }
            

            else if (current_position == MIDDLEPOINT){


                int len_CFGInfo1 = strlen(AST_result[i]);
                char* CFGInfo1 = malloc(len_CFGInfo1 + 1);
                sprintf(CFGInfo1,AST_result[i]);
                push(middleStack,CFGInfo1);

                current_position = RIGHTPOINT;
                continue;
            }

            else if (current_position == RIGHTPOINT){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                int len_CFGInfo2 = len_vname2 + strlen(":=") + strlen(AST_result[i]);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo2 + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s", vname2, AST_result[i]);

                push(rightStack,vname2);

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2; 
            }
        }

        if (strcmp(currentOperation,"if") == 0){
            if (current_position == LEFTPOINT){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                int len_CFGInfo2 = len_vname2 + strlen(":=") + strlen(AST_result[i]);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo2 + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s", vname2, AST_result[i]);

                push(leftStack,vname2);

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                // printf("CFGinfo if: %s\n",CFG_current->CFGInfo);

                CFG_IF_FRONT_num++;
                CFG_IF_FRONT[CFG_IF_FRONT_num] = CFG_current;
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2; 
                current_position = MIDDLEPOINT;
                continue;
            }

            if (current_position == MIDDLEPOINT){
                // (*root_id)++;
                // struct CFGNode* CFG_next2 = createNode(*root_id);
                // CFG_IF_FRONT[CFG_IF_FRONT_num]->succ2 = CFG_next2;
                // CFG_next2 ->pred1 = CFG_IF_FRONT[CFG_IF_FRONT_num];
                // CFG_current = CFG_next2;


                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                int len_CFGInfo2 = len_vname2 + strlen(":=") + strlen(AST_result[i]);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo2 + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s", vname2, AST_result[i]);
                push(middleStack,vname2);
                strcpy(CFG_current->CFGInfo,CFGInfo2);
                CFG_IF_BEHIND_num++;
                CFG_IF_BEHIND[CFG_IF_BEHIND_num] = CFG_current;

                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_IF_FRONT[CFG_IF_FRONT_num]->succ2 = CFG_next2;
                CFG_next2 ->pred1 = CFG_IF_FRONT[CFG_IF_FRONT_num];  
                CFG_current = CFG_next2;             

                current_position = RIGHTPOINT;
                continue;
            }


            if (current_position == RIGHTPOINT){

                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                int len_CFGInfo2 = len_vname2 + strlen(":=") + strlen(AST_result[i]);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo2 + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s", vname2, AST_result[i]);
                push(rightStack,vname2);
                strcpy(CFG_current->CFGInfo,CFGInfo2);


                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2; 

                CFG_current->pred2 = CFG_IF_BEHIND[CFG_IF_BEHIND_num];
                CFG_IF_BEHIND[CFG_IF_BEHIND_num]->succ1 = CFG_current;

                if_symbol = 1;
            }
        }

        // if (strcmp(currentOperation,"let")){
        //     if (current_position == LEFTPOINT){
        //         int len_vname = snprintf(NULL, 0, "v%d", CFG_current->id);
        //         int len_CFGInfo = len_vname + strlen(":=") + strlen(AST_result[i]);
        //         char* vname = malloc(len_vname + 1);
        //         char* CFGInfo = malloc(len_CFGInfo + 1);
        //         sprintf(vname, "v%d", CFG_current->id);         
        //         sprintf(CFGInfo, "%s:=%s", vname, AST_result[i]); 

        //         push(leftStack,vname);
        //         current_position = RIGHTPOINT;

        //         strcpy(CFG_current->CFGInfo,CFGInfo);
        //         (*root_id)++;
        //         struct CFGNode* CFG_next = createNode(*root_id);
        //         CFG_current ->succ1 = CFG_next;
        //         CFG_next ->pred1 = CFG_current;
        //         CFG_current = CFG_next;
        //         continue;
        //     }

        // }


// CFG_update_part



        while((current_position == RIGHTPOINT) && (strcmp(currentOperation,"if")) != 0){

            if (operationStack->top == -1) {
            break;
            }
            currentOperation = operationStack->data[operationStack->top];


            if (strcmp(currentOperation,"add") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" PLUS ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s PLUS %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }

                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
                
            }

            if (strcmp(currentOperation,"mul") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" MULT ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s MULT %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);


                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                // printf("operation: %s\n",operationStack->data[operationStack->top]);
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }


                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }

            if (strcmp(currentOperation,"minus") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" SUB ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s SUB %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);

                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }

 
                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }

            if (strcmp(currentOperation,"div") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" DIV ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s DIV %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }

                
                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }   
            
            if (strcmp(currentOperation,"mod") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" MOD ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s MOD %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }


                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }

                
            }
               
            if (strcmp(currentOperation,"equal") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" EQ ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s EQ %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);

                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }


                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }

            if (strcmp(currentOperation,"smaller") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" SMALLER ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s SMALLER %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }


                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }
        
            if (strcmp(currentOperation,"greater") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" GREATER ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s GREATER %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }


                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }
        
            if (strcmp(currentOperation,"notgreater") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" NOT GREATER ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s PLUS %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }


                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }
        
            if (strcmp(currentOperation,"notsmallar") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" NOT SMALLER ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s NOT SMALLER %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }

                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }      
            }
        
            if (strcmp(currentOperation,"and") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" AND ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s AND %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }

                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }
                
            if (strcmp(currentOperation,"or") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen(" OR ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:=%s OR %s", vname2, leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }


                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }

            if (strcmp(currentOperation,"let") == 0){
                
                char* leftChar = pop(leftStack);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = strlen(leftChar) + strlen(":=") + strlen(rightChar);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(CFGInfo2, "%s:=%s", leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

            }
            
            if (strcmp(currentOperation,"not") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* rightChar = pop(rightStack);
                int len_CFGInfo = len_vname2 + strlen(":= NOT ") + strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(vname2, "v%d", CFG_current->id);         
                sprintf(CFGInfo2, "%s:= NOT %s", vname2, rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                // printf("CFGinfo: %s\n",CFG_current->CFGInfo);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }


                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         printf("yes!\n");
                    //         break;
                    //     }
                    // }
                }
            }
                       
            if(strcmp(currentOperation,"call func") == 0){
                int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
                char* leftChar = pop(leftStack);
                char* middleChar = pop(middleStack);
                char* rightChar = pop(rightStack);

                int len_CFGInfo = len_vname2 + strlen(":=") + strlen(leftChar) + strlen("(") + strlen(middleChar) + strlen(")") +strlen(rightChar);
                char* vname2 = malloc(len_vname2 + 1);
                sprintf(vname2, "v%d", CFG_current->id);  
                char* CFGInfo2 = malloc(len_CFGInfo + 1);
                sprintf(CFGInfo2, "%s:=%s(%s,%s)", vname2,middleChar,leftChar,rightChar); 

                strcpy(CFG_current->CFGInfo,CFGInfo2);
                (*root_id)++;
                struct CFGNode* CFG_next2 = createNode(*root_id);
                CFG_current ->succ1 = CFG_next2;
                CFG_next2 ->pred1 = CFG_current;
                CFG_current = CFG_next2;
                char* tempOperationReturn = pop(operationStack);

                positionNum--;
                current_position = positionList[positionNum];
                positionList[positionNum] = '\0';

                if (current_position == LEFTPOINT){
                    push(leftStack,vname2);
                }
                else if(current_position == RIGHTPOINT){
                    push(rightStack,vname2);
                }
                else if(current_position == MIDDLEPOINT){
                    push(middleStack,vname2);
                }

                if (operationStack->top > -1) {
                    char* tempCurrentOperation = operationStack->data[operationStack->top];
                    if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                    (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                    (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                        if (current_position == LEFTPOINT){
                            current_position = RIGHTPOINT;
                            break;
                        }
                    }
                    // if (strcmp(tempCurrentOperation,"if") == 0) {
                    //     if (current_position == LEFTPOINT){
                    //         current_position = MIDDLEPOINT;
                    //         break;
                    //     }
                    //     if (current_position == MIDDLEPOINT){
                    //         current_position = RIGHTPOINT;
                    //         break;
                    //     }
                    // }
                }
            }
        

        }
        
    
        if (operationStack->top == -1) {
            continue;
        }
        currentOperation = operationStack->data[operationStack->top];

        if(strcmp(currentOperation,"if") == 0){
            if (current_position == LEFTPOINT){
                // printf("leftpoint.\n");
                CFG_IF_FRONT_num++;
                CFG_IF_FRONT[CFG_IF_FRONT_num] = CFG_current->pred1;
                current_position = MIDDLEPOINT;
                continue;
            }

            if (current_position == MIDDLEPOINT){
                // printf("middlepoint.\n");
                CFG_IF_BEHIND_num++;
                CFG_IF_BEHIND[CFG_IF_BEHIND_num] = CFG_current->pred1;
                CFG_current->pred1->succ1 = NULL; 

                CFG_IF_FRONT[CFG_IF_FRONT_num]->succ2 = CFG_current;
                CFG_current ->pred1 = CFG_IF_FRONT[CFG_IF_FRONT_num];
                current_position = RIGHTPOINT;
                continue;
            }
            if ((current_position == RIGHTPOINT) && (if_symbol == 0)){
                // printf("rightpoint.\n");
                CFG_current->pred2 = CFG_IF_BEHIND[CFG_IF_BEHIND_num];
                CFG_IF_BEHIND[CFG_IF_BEHIND_num]->succ1 = CFG_current;
                // printf("CFG_value:%s\n",CFG_current->CFGInfo);
                // printf("CFG_value2:%s\n",CFG_current->pred1->CFGInfo);
                // printf("CFG_value3:%s\n",CFG_current->pred2->CFGInfo);

            }

            int len_vname2 = snprintf(NULL, 0, "v%d", CFG_current->id);
            char* leftChar = pop(leftStack);
            char* middleChar = pop(middleStack);
            char* rightChar = pop(rightStack);
            // printf("leftchar: %s\n",leftChar);
            // printf("middleChar: %s\n",middleChar);
            // printf("rightChar: %s\n",rightChar);

            int len_CFGInfo = strlen("IF ") + strlen(leftChar) + strlen(" = true,then ") + len_vname2 + strlen(" := ") + strlen(middleChar) + strlen(",else ") + len_vname2 + strlen(" := ") + strlen(rightChar);
            char* vname2 = malloc(len_vname2 + 1);
            sprintf(vname2, "v%d", CFG_current->id);  
            char* CFGInfo2 = malloc(len_CFGInfo + 1);
            sprintf(CFGInfo2, "IF %s = true, then %s := %s, else %s := %s", leftChar,vname2, middleChar,vname2,rightChar); 

            strcpy(CFG_current->CFGInfo,CFGInfo2);

            // printf("step %d %s\n",i,CFG_current->pred1->CFGInfo);
            // printf("step %d %S\n",i,CFG_current->pred2->CFGInfo);
            // printf("CFGInfo2 %d: %s\n",i, CFG_current->pred1->CFGInfo);
            // printf("CFGInfo3 %d: %s\n",i, CFG_current->pred2->CFGInfo);


            // printf("CFGInfo1: %s\n",CFG_current->CFGInfo);

            //printf("CFGInfo4: %s\n",CFG_current->succ1->CFGInfo);


            (*root_id)++;
            struct CFGNode* CFG_next2 = createNode(*root_id);
            CFG_current ->succ1 = CFG_next2;
            CFG_next2 ->pred1 = CFG_current;
            CFG_current = CFG_next2;
            char* tempOperationReturn = pop(operationStack);
            // printf("current operation:%s\n.",tempOperationReturn);
            // printf("next operation:%s\n.",operationStack->data[operationStack->top]);

            positionNum--;
            current_position = positionList[positionNum];
            positionList[positionNum] = '\0';

            CFG_IF_FRONT[CFG_IF_FRONT_num] = NULL;
            CFG_IF_FRONT_num--;
            CFG_IF_BEHIND[CFG_IF_BEHIND_num] = NULL;
            CFG_IF_BEHIND_num--;
            if_symbol = 0;

            if (current_position == LEFTPOINT){
                push(leftStack,vname2);
            }
            else if(current_position == RIGHTPOINT){
                push(rightStack,vname2);
            }
            else if(current_position == MIDDLEPOINT){
                push(middleStack,vname2);
            }


            
            if (operationStack->top > -1) {
                char* tempCurrentOperation = operationStack->data[operationStack->top];
                if ((strcmp(tempCurrentOperation,"add") == 0) || (strcmp(tempCurrentOperation,"mul") == 0) || (strcmp(tempCurrentOperation,"minus") == 0) || (strcmp(tempCurrentOperation,"div") == 0) || (strcmp(tempCurrentOperation,"mod") == 0) ||
                (strcmp(tempCurrentOperation,"equal") == 0) || (strcmp(tempCurrentOperation,"smaller") == 0) || (strcmp(tempCurrentOperation,"greater") == 0) || (strcmp(tempCurrentOperation,"notgreater") == 0) || (strcmp(tempCurrentOperation,"notsmallar") == 0) || 
                (strcmp(tempCurrentOperation,"and") == 0) || (strcmp(tempCurrentOperation,"or") == 0) || (strcmp(tempCurrentOperation,"let") == 0)){
                    if (current_position == LEFTPOINT){
                        current_position = RIGHTPOINT;
                    }
                }
                if (strcmp(tempCurrentOperation,"if") == 0) {
                    if (current_position == LEFTPOINT){
                        CFG_IF_FRONT[CFG_IF_FRONT_num] = CFG_current->pred1;
                        current_position = MIDDLEPOINT;
                    }
                    else if (current_position == MIDDLEPOINT){
                        CFG_IF_BEHIND[CFG_IF_BEHIND_num] = CFG_current->pred1;
                        CFG_current->pred1->succ1 = NULL; 

                        CFG_IF_FRONT[CFG_IF_FRONT_num]->succ2 = CFG_current;
                        CFG_current ->pred1 = CFG_IF_FRONT[CFG_IF_FRONT_num];
                        current_position = RIGHTPOINT;
                    }
                }
            }
        }

    }

    {
        cfgNodeList[cfgNodeList_idx] = CFG_root;
        cfgNodeList_idx++;
    }

    
}



void printCFGList(FILE* fp) {
    for (int i = 0; i < cfgNodeList_idx; i++){
      CFGNode* CFG_root = cfgNodeList[i];
    //   setTraverse(CFG_root);
    //   reset_dfs(CFG_root);

    //   dfs(CFG_root, lr);
    //   reset_dfs(CFG_root);

      // printCFG(CFG_root);
      dot_CFG(CFG_root, fp);
      reset_dfs(CFG_root);
      printf("\n");
    }
}

