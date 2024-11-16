#include "SymbolTable.h"

struct FuncTable* funcRoot;
struct FuncTable* currentFunc;
struct SymbolTable* current;

int get_current_scope() {
    return scope;
}

void entry_scope() {
    scope++;
}

void exit_scope() {
    scope--;
}

void push(char* name) {
    if(var_called == 0){
        root = (struct SymbolTable*)malloc(sizeof(struct SymbolTable));
        root->name = name;
        root->type = UNKNOWNTYPE;
        root->scope = scope;
        root->next = NULL;
        root->last = NULL;

        current = root;
        var_called = 1;
    } else {
        current->next = (struct SymbolTable*)malloc(sizeof(struct SymbolTable));
        struct SymbolTable* temp = current;
        current = temp->next;
        current->next = NULL;
        current->last = temp;

        current->name = name;
        current->scope = scope;
    }

}

void removeSymbol(struct SymbolTable* ptr) {//root can not be removed
    if(current == ptr) {
        current = ptr->last;
    }
    if(ptr){
        struct SymbolTable* last = ptr->last;
        struct SymbolTable* next = ptr->next;
        last->next = next;
        if(next) {
            next->last = last;
        }
        free(ptr);
    }
}

void LocalTablePush() {
    if(localEmpty == 1) {
        localEmpty = 0;
        localRoot = (struct LocalTable*)malloc(sizeof(struct LocalTable));
        localRoot->last = NULL;
        localRoot->entry = current;
        localRoot->cnt = 0;

        currentLocal = localRoot;
    } else {
        struct LocalTable* temp = (struct LocalTable*)malloc(sizeof(struct LocalTable));
        temp->last = currentLocal;
        temp->entry = current;
        temp->cnt = 0;

        currentLocal = temp;
    }
}

void printLocal() {
    struct LocalTable* ptr = currentLocal;
    printf("\n print local stack info(inversed order):\n");
    while (ptr)
    {
        printf("name: %s, scope: %d\n", ptr->entry->name, ptr->entry->scope);
        ptr = ptr->last;
    }
    printf("\n");

}

void popLocal() {
    if(currentLocal->last) {
        struct LocalTable* temp = currentLocal->last;
        free(currentLocal);
        currentLocal = temp;
    } else {
        free(localRoot);
        localEmpty = 1;
        localRoot = NULL;
        currentLocal = NULL;
    }
}


void checkLocal() {
    struct SymbolTable* top = current;
    struct SymbolTable* btm = currentLocal->entry;
    
    struct SymbolTable* ptr = top;
    while(ptr && ptr != btm){

        int defined = 0;
        if(strcmp(ptr->name, btm->name) == 0){
            defined = 1;
        }

        ptr = ptr->last;
        if(defined) { removeSymbol(ptr->next);}
    }
    removeSymbol(currentLocal->entry);
    popLocal();
    
}

void checkCall() {
    struct SymbolTable* top = current;
    struct SymbolTable* btm = currentLocal->entry;

    struct FuncTable* f = funcRoot;

    // check if function has been declared.
    int defined = 0;
    while (f)
    {
        if(strcmp(f->entry->name, btm->name) == 0) {
            defined = 1;
            break;
        }
        f = f->next;
    }
    if(!defined) {
        printf("function %s has not declared", btm->name);
        perror("Function name undeclared. \n");
        exit(EXIT_FAILURE);
    }

    // check arg list.
    struct SymbolTable* temp = btm->next;
    int cnt = 0;
    for(int i = 0; i < f->arities; i++){

        if(temp) {
            cnt++;
            temp = temp->next;

            removeSymbol(temp->last);
        } else {
            break;
        }
    }

    
    if(currentLocal->cnt != f->arities){
        printf("numbers of arguement doesn't match with function %s\n", f->entry->name);
        perror("arguement list conflicts. \n");
        exit(EXIT_FAILURE);
    }
    else{
        DataType inputarglist[f->arities];
        DataType funarglist[f->arities];
        struct SymbolTable *temp1 = btm->next;
        for (int i = 0; i < f->arities;i++){
            if(temp1) {
                inputarglist[i] = temp1->type;
                temp1 = temp1->next;
            }else{
                break;
            }
        }
        struct SymbolTable* argtop = f->entry;
        for(int i = 0; i < f->arities; i++) {
            argtop = argtop->next;
            funarglist[i] = argtop->type;

        }
    //      for (int i = 0; i < f->arities; i++) {
    //     printf("%d ", inputarglist[i]); 
    // }
    //  printf("\n");
    //          for (int i = 0; i < f->arities; i++) {
    //     printf("%d ", funarglist[i]); 
    // }
    // printf("\n");
    //     for (int i = 0; i < f->arities; i++) {
    //         if(inputarglist[i] != (funarglist[i] - 1))
    //         {        printf("argument type doesn't not match\n");
    //     perror("arguement type conflicts. \n");} 
    //     exit(EXIT_FAILURE);
    // }

    }


    
    // struct SymbolTable* ptr = top;
    // while(ptr && ptr != btm){

    //     int defined = 0;
    //     if(strcmp(ptr->name, btm->name) == 0){
    //         defined = 1;
    //     }

    //     ptr = ptr->last;
    //     if(defined) { removeSymbol(ptr->next);}
    // }
    removeSymbol(currentLocal->entry);
    popLocal();
    
}


void destroySymbolTableEntry(struct SymbolTable* entry) {
    if (entry->name) {
        free(entry->name);
        entry->name = NULL;
    }
}



void printSymbolTable(struct SymbolTable* sym) {
    printf("name: %s\n", sym->name);
    printf("scope: %d\n", sym->scope);
}

void changeName(char* name) {

    struct SymbolTable* header = current;
    
    while(1) {

        if(header->scope != scope) {
            
            printf("varname %s is undeclared.\n", name);
            return;
        }

        if(strcmp(header->name, "unknown_name") == 0) {
            header->name = name;
            return;
        } else {
            header = header->last;
        }
    }
}

void setType(DataType type) {
    current->type = type;
}

void printStack() {

    struct SymbolTable* ptr = root;
    printf("\n print stack info:\n");
    while (ptr)
    {
        printf("name: %s, scope: %d, type: %d\n", ptr->name, ptr->scope, ptr->type);
        ptr = ptr->next;
    }
    printf("\n");
}

void get_order(char* name) {
    order++; 
    // printf("name: %s, order: %d\n", name, order);
}

void incCounter() {
    arityCounter++;
}

void incArgCounter() {
    if(currentLocal) {
        currentLocal->cnt++;
    }
}

void addFunc() {

    struct SymbolTable* ptr = current;

    // traceback # of arites of stack
    for(int i = 0; i < arityCounter; i++){
        ptr = ptr->last;
    }
    // find the nearest SymbolTable with a scope of 1

    while(ptr) {
        if(ptr->scope == 1) {
            if(fun_called == 0) {
                fun_called = 1;
                funcRoot = (struct FuncTable*)malloc(sizeof(struct FuncTable));
                funcRoot->entry = ptr;
                funcRoot->next = NULL;
                funcRoot->arities = arityCounter;
                arityCounter = 0;

                currentFunc = funcRoot;
            } else {
                currentFunc->next = (struct FuncTable*)malloc(sizeof(struct FuncTable));
                currentFunc = currentFunc->next;

                currentFunc->entry = ptr;
                currentFunc->next = NULL;
                currentFunc->arities = arityCounter;
                arityCounter = 0;
            }
            break;
        } else {
            ptr = ptr->last;
        } 
    }
}

void checkFunc() {
    checkSemanticError(funcRoot);
}

void checkFuncName(struct FuncTable* f) {
    struct SymbolTable* ptr = current;
    struct SymbolTable* argtop = f->entry;
    for(int i = 0; i < f->arities; i++) {
        argtop = argtop->next;
    }
    while (ptr)
    {
        if(ptr == argtop) {
            break;
        } else {
            struct FuncTable* s = funcRoot;
            while (s)
            {
                if(s == f) {break;}
                if(strcmp(ptr->name, s->entry->name) == 0) {
                    printf("Variable %s has the name of a defined function %s\n", ptr->name, s->entry->name);
                    perror("Variable name conflicts.\n");
                    exit(EXIT_FAILURE);
                }
                s = s->next;
            }
            
            
            struct SymbolTable* itr = f->entry;
            int defined = 0;
            for(int i = 0; i < f->arities; i++) {
                itr = itr->next;
                if(strcmp(ptr->name, itr->name) == 0) {
                    defined = 1;
                    break;
                }
            }
            if(!defined) {
                printf("Variable %s is undefined\n", ptr->name);
                perror("Variable name undefined.\n");
                exit(EXIT_FAILURE);
            }
            ptr = ptr->last;
            if(defined) {
                removeSymbol(ptr->next);
            }
        }
    }
}

void checkPrint() {
    checkFuncName(currentFunc);
}

int checkSemanticError(struct FuncTable* f) {
    struct SymbolTable* argtop = f->entry;
    for(int i = 0; i < f->arities; i++) {
        argtop = argtop->next;
        if(strcmp(argtop->name, f->entry->name) == 0) {
            printf("arguement %s has the name of a defined function %s\n", argtop->name, f->entry->name);
            perror("arguement name conflicts.\n");
            exit(EXIT_FAILURE);
        }
        struct SymbolTable* temp = argtop;
        int used = 0;
        while(temp) {
            temp = temp->last;
            if(temp == f->entry) {
                break;
            }
            if(strcmp(argtop->name, temp->name) == 0){
                used = 1;
                break;
            }
        }
        if(used) {
            printf("duplicated declaration of arguement %s.\n", argtop->name);
            perror("arguement name conflicts.\n");
            exit(EXIT_FAILURE);
        }
    }
    checkFuncName(currentFunc);


}

void printFunc() {
    struct FuncTable* ptr = funcRoot;
    printf("\n print function stack info:\n");
    while (ptr)
    {
        printf("name: %s, scope: %d, arities:%d\n", ptr->entry->name, ptr->entry->scope, ptr->arities);
        ptr = ptr->next;
    }
    printf("\n");
}
