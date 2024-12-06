#include "lr.h"

CFGNode* root[1024] = {NULL};
int currentID = 0;

int cnt = 0;
int bound = 0;

int cntAst(struct ast* node){
    if(node){
        cnt++;
    }
    return 0;
}

CFGNode* graph[1024] = {NULL};
int graphCnt = 0;
void connect(int from, int to) {

    if(!root[from]->succ1) {
        root[from]->succ1 = root[to];
    } else if (!root[from]->succ2)
    {
        root[from]->succ2 = root[to];
    } else {
        perror("wa\n");
        exit(1);
    }

    if(!root[to]->pred1){
        root[to]->pred1 = root[from];
    } else if (!root[to]->pred2){
        root[to]->pred2 = root[from];
    } else {
        perror("wa\n");
        exit(1);
    }
}

struct ast* let_astlist[2048] = {NULL};
int let_astlist_idx = 0;
bool begin = false;

int argCnt = 0;
int create(struct ast* node) {
    root[node->id] = createNode(node->id);
    CFGNode* cfg = root[node->id];
    if(node->id == 1){
        if(strcmp(get_root(node)->token, "definefun") != 0) {
            begin = true;
            cnt++;
            root[cnt] = createNode(cnt);
            root[cnt]->CFGInfo = "main";
            connect(currentID, cnt);
            root[cnt]->value = "main";
        }
    }
    if(begin) {

        currentID = cnt;
        // connect(cnt, node->id);
        root[cnt]->traverse = cfg;
        begin = false;
    }

    if(//strcmp(node->token, "GETBOOL") == 0
        //|| strcmp(node->token, "GETINT") == 0
         strcmp(node->token, "call func") == 0
    ) {
        cnt++;
        root[cnt] = createNode(cnt);
        sprintf(root[cnt]->CFGInfo, "%d", cnt);
        sprintf(root[cnt]->value, "%d", cnt);
        connect(currentID, cnt);
        root[currentID]->traverse = root[cnt];
        root[cnt]->traverse = cfg;
        root[cnt]->ori_id = -2;
        currentID = cnt;
    }

    if(node->next
     && get_root(node) != get_root(node->next)
     && strcmp(get_root(node->next)->token, "definefun") != 0
    ) {
        begin = true;
        cnt++;
        root[cnt] = createNode(cnt);
        root[cnt]->CFGInfo = "main";
        connect(node->id, cnt);
        root[cnt]->value = "main";
        cfg->traverse = root[cnt];
    }
    if(node->parent){
        if(strcmp(node->token, "if") == 0) {
            sprintf(cfg->CFGInfo, "IF v%d = true, then v%d := v%d, else v%d := v%d", 
                node->child->id->id, 
                node->id, node->child->next->id->id, 
                node->id, node->child->next->next->id->id);
            connect(node->child->next->id->id, node->id);
        } else if(strcmp(node->token, "let") == 0) {

            sprintf(cfg->CFGInfo, "v%d := v%d", 
                node->id, 
                node->child->next->next->id->id);
           // printf("\t%s\n", cfg->CFGInfo);
            sprintf(cfg->value, "v%d", node->child->next->next->id->id);

        } else if(strcmp(node->parent->token, "let") == 0 && node == node->parent->child->id) {
            sprintf(cfg->CFGInfo, "%s = v%d", 
                node->token, 
                node->parent->child->next->id->id);
            sprintf(cfg->value, "v%d", node->parent->child->next->id->id);
            return 0;
        } else if (strcmp(node->token, "add") == 0
            || strcmp(node->token, "minus") == 0 
            || strcmp(node->token, "mul") == 0 
            || strcmp(node->token, "div") == 0 
            || strcmp(node->token, "mod") == 0 
            || strcmp(node->token, "equal") == 0 
            || strcmp(node->token, "smaller") == 0 
            || strcmp(node->token, "greater") == 0 
            || strcmp(node->token, "notsmaller") == 0 
            || strcmp(node->token, "notgreater") == 0 
            || strcmp(node->token, "and") == 0 
            || strcmp(node->token, "or") == 0 
        ) {
            // printf("bb%d:\n", node->id);
            char str[100];
            sprintf(str, "v%d := v%d", node->id, node->child->id->id);
            sprintf(cfg->value, "v%d", node->child->id->id);

            strcat(cfg->CFGInfo, str);

           // printf("\t%s", str);
            
            struct ast_child* astp = node->child;
            for(int i = 0; i < get_child_num(node) - 1; i++) {
                astp = astp->next;
                sprintf(str, " %s v%d", node->token, astp->id->id);
                strcat(cfg->value, str);
                strcat(cfg->CFGInfo, str);

               // printf(str);
            }
           // printf("\n");
        } else if (strcmp(node->token, "not") == 0 ) {
           // printf("bb%d:\n", node->id);
           // printf("\tv%d := not v%d\n", node->id, node->child->id->id);
            sprintf(cfg->CFGInfo, "v%d := not v%d\n", node->id, node->child->id->id);
            sprintf(cfg->value, "not v%d\n", node->id, node->child->id->id);

        // } else if(strcmp(node->parent->token, "add") == 0 
        //     || strcmp(node->parent->token, "minus") == 0 
        //     || strcmp(node->parent->token, "mul") == 0 
        //     || strcmp(node->parent->token, "div") == 0 
        //     || strcmp(node->parent->token, "mod") == 0 
        //     || strcmp(node->parent->token, "equal") == 0 
        //     || strcmp(node->parent->token, "smaller") == 0 
        //     || strcmp(node->parent->token, "greater") == 0 
        //     || strcmp(node->parent->token, "notsmaller") == 0 
        //     || strcmp(node->parent->token, "notgreater") == 0 
        //     || strcmp(node->parent->token, "not") == 0 
        //     || strcmp(node->parent->token, "and") == 0 
        //     || strcmp(node->parent->token, "or") == 0 
        //     || strcmp(node->parent->token, "if") == 0 
        //     || strcmp(node->parent->token, "let") == 0 
        // ){
        // } else {
        } else if(node->isNum || node->isVar) {

            // printf("isNum: %d\n", node->id);
           // printf("\tv%d = %s\n", node->id, node->token);
            sprintf(cfg->CFGInfo, "v%d = %s", node->id, node->token);
            sprintf(cfg->value, "%s", node->token);
            
        } 

        if(strcmp(node->parent->token, "definefun") == 0 && node == node->parent->child->id) {
            // begin = true;
            // printf("define func %s\n", node->token);
            sprintf(cfg->CFGInfo, "%s", node->token);
        } else if(strcmp(node->parent->token, "inputlist") == 0) {
            if(strcmp(node->token, "none") == 0) {
                return 0;
            } else {
                // printf("%d\n", node->id);
                sprintf(cfg->CFGInfo, "%d", node->id);
                argCnt++;
                sprintf(cfg->value, "a%d", argCnt);
                
            }
        } else if(strcmp(node->token, "inputlist") == 0
                || strcmp(node->token, "ret INT") == 0
                || strcmp(node->token, "ret BOOL") == 0
        ) {
            return 0;
        } else if(strcmp(node->token, "call func") == 0) {
            root[cnt]->traverse = cfg;

            int min = (1 << 10);
            struct ast_child* astp = node->child;
            for(int i = 0; i < get_child_num(node); i++) {
                if(astp->id->id < min){
                    min = astp->id->id;
                }
                astp = astp->next;
            }

            sprintf(cfg->CFGInfo, "v%d := %s(", node->id, find_ast_node(min)->token);
            sprintf(cfg->value, "%s(", find_ast_node(min)->token, find_ast_node(min)->token);

            for(int i = 1; i < get_child_num(node); i++) {
                
                char str[64];
                int secMin = min;
                min = (1 << 10);
                astp = node->child;

                bool find = false;
                for(int i = 0; i < get_child_num(node); i++) {
                    if(astp->id->id < min && astp->id->id > secMin){
                        min = astp->id->id;
                    }
                    astp = astp->next;
                }

                for(int bd = bound + 1; bd <= cnt; bd++){
                    if(root[bd]->ori_id == min){
                        sprintf(str, "v%d, ", bd);
                        find = true;
                        break;
                    }
                }
                // if(strcmp(str, "none, ") == 0){
                //     break;
                // }
                if(!find) {
                    break;
                }
                strcat(cfg->CFGInfo, str);
                strcat(cfg->value, str);
                


            }
            strcat(cfg->CFGInfo, ")");
            strcat(cfg->value, ")");

            sprintf(root[cnt]->value, "%s", cfg->value);

        } else if(strcmp(node->parent->token, "call func") == 0) {
            int min = (1 << 10);
            struct ast_child* astp = node->parent->child;
            for(int i = 0; i < get_child_num(node->parent); i++) {
                if(astp->id->id < min){
                    min = astp->id->id;
                }
                astp = astp->next;
            }
            
            // printf("id %d\n", node->id);
            if(node->id == min){
                return 0;
            } 
            else {
                if(strcmp(node->token, "none") == 0) {
                    root[min]->traverse = root[node->parent->id];
                    return 0;
                } else {
                    begin = true;

                    // sprintf(cfg->CFGInfo, "v%d = %s", node->id, node->token);
                    // sprintf(cfg->value, "v%d", node->id);
                    cnt++;
                    root[cnt] = createNode(cnt);
                    sprintf(root[cnt]->CFGInfo, "%d", cnt);
                    sprintf(root[cnt]->value, "%s", node->token);

                    connect(currentID, cnt);
                    root[currentID]->traverse = root[cnt];

                    root[cnt]->ori_id = node->id;
                    root[cnt]->value = cfg->value;
                    currentID = cnt;
                }
            } 
        } 

        

        printf(" ");
        
        if(currentID >= 0) {
            connect(currentID, node->id);
        }
        
        if (strcmp(node->parent->token, "if") == 0 && node == node->parent->child->next->id) {
            currentID = node->parent->child->id->id;
        } else if(strcmp(node->parent->token, "let") == 0 && node == node->parent->child->next->id) {
            connect(node->id, node->parent->child->id->id);
            currentID = node->parent->child->id->id;
        } 
        else {
            currentID = node->id;
        }
            // }
        begin = false;
    } else {
        // if(strcmp(node->token, "main") == 0) {
        //     printf("define func main\n");
        // } else {
        //     printf("define func %s\n", node->child->id->token);
        // }

        cfg->CFGInfo = "EXIT";
        connect(currentID, node->id);

        currentID = node->id;
    }
    // printCFG(root[node->id]);
    
    return 0;
}

CFGNode* traverseList[1024];
int traverseList_idx = 0;
int addTraverseList(CFGNode* node){
    traverseList[traverseList_idx] = node;
    traverseList_idx++;
    if(!node->succ1 && !node->succ2) {
        node->CFGInfo = "EXIT";
    }
    return 0;
}
void setTraverse(CFGNode* node) {
    dfs(node, addTraverseList);
    // printf("total: %d\n", traverseList_idx);
    for(int i = 0; i < traverseList_idx - 1; i++){
        traverseList[i]->traverse = traverseList[i + 1];
    }
}


int llrr(CFGNode* node) {
    // printf("(bb%d:, name:%s\n", node->id, node->CFGInfo);
    if(node->id > bound) {
        if(strcmp(node->CFGInfo, "main") == 0){
            printf("function main\n");
        } else {
            // if(strcmp(node->traverse->value, "GETBOOL") == 0
            //     || strcmp(node->traverse->value, "GETINT") == 0
            // ) {
            //     printf("bb%d:\n", node->id);
            //     printf("\tcall %s\n", node->traverse->value);
            //     // printf("\trv = v%d\n", node->traverse->id);
            //     printf("\tbr bb%d\n", node->traverse->id);


            // } else
            if(node->ori_id == -2) {
                printf("bb%d:\n", node->id);
                printf("\tcall %s\n", node->value);
                printf("\tbr bb%d\n", node->traverse->id);
            } else if( node->ori_id > 0
                     && find_ast_node(node->ori_id)->parent
                     && strcmp(find_ast_node(node->ori_id)->parent->token, "call func") == 0
            ) {
                printf("bb%d:\n", node->id);
                // printf("\ta%d = %s\n", ++argCnt, node->value);
                printf("\tv%d = %s\n", node->id, node->value);
                printf("\tbr bb%d\n", node->traverse->id);
            }
        }

        return 0;
    }
    if(find_ast_node(node->id)->parent && strcmp(find_ast_node(node->id)->parent->token, "definefun")==0 && node->id == find_ast_node(node->id)->parent->child->id->id){
        printf("function %s\n", node->CFGInfo);
        return 0;
    }
    printf("bb%d:\n", node->id);
    bool printbr = true;
    if(strcmp(node->CFGInfo, "EXIT")==0){
        printf("\trv = v%d\n", node->id - 1);
        printbr = false;
    } else if(find_ast_node(node->id)->parent
            && strcmp(find_ast_node(node->id)->parent->token, "call func") == 0
    ) {
        

    } else if(//strcmp(find_ast_node(node->id)->token, "GETBOOL") == 0
            //|| strcmp(find_ast_node(node->id)->token, "GETINT") == 0
        strcmp(find_ast_node(node->id)->token, "call func") == 0
    ) {
        printf("\tv%d = rv\n", node->id);
    } else if(strcmp(find_ast_node(node->id)->token, "if") == 0) {

    } else if(find_ast_node(node->id)->parent
            && strcmp(find_ast_node(node->id)->parent->token, "if") == 0
            && (find_ast_node(node->id) == find_ast_node(node->id)->parent->child->next->id || find_ast_node(node->id) == find_ast_node(node->id)->parent->child->next->next->id)
    ) {
        printf("\tv%d = %s\n", find_ast_node(node->id)->parent->id, node->value);
    }
    else {
        printf("\tv%d = %s\n", node->id, node->value);
    }

    if(printbr) {
        if(node->succ1 && node->succ2){
            printf("\tbr v%d bb%d bb%d\n", node->id, find_ast_node(node->id)->parent->child->next->id->id, find_ast_node(node->id)->parent->child->next->next->id->id);
        } else if(node->traverse){
            if(node->succ1->pred2 && (node->succ1->pred1 == node)) {
                printf("\tbr bb%d\n", node->succ1->id); 
            } else {
                printf("\tbr bb%d\n", node->traverse->id); 
            }
        }
    }
    return 0;
}



int testGraph() {
    root[0] = createNode(0);
    root[0]->CFGInfo = "null";
    printf("\n");

    visit_ast(cntAst);
    bound = cnt;
    visit_ast(create);
    printf("\n");

    // dfs(root[1], printCFG);
    int root_idx = 1;
    if(strcmp(get_root(find_ast_node(1))->token, "definefun") != 0){
        root_idx = root[1]->pred1->id;
    }

    setTraverse(root[root_idx]);
    printf("function GETINT\nfunction GETBOOL\n");
    dfs(root[root_idx], llrr);
    dfs(root[root_idx], printCFG);


    FILE* fp = fopen("cfg.dot", "w");
    fprintf(fp, "digraph print {\n ");

    // for (int i = 0; i < alist->length; i++){
    //     create_CFG(alist->astList[i],root_id,dot_id);
    // }
    dot_CFG(root[root_idx], fp);


    // printCFGList(fp);

    fprintf(fp, "}\n ");
    fclose(fp);
    // system("dot -Tpdf cfg.dot -o cfg.pdf");
    // printList();
    return 0;
}