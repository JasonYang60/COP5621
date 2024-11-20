/**
 * Author:    Grigory Fedyukovich, Subhadeep Bhattacharya
 * Created:   09/30/2020
 *
 * (c) FSU
 **/

#include "ast.h"

void insert_parent(struct ast* p) {
  struct ast_child* temp_ast_child_root = p->child;
  while(temp_ast_child_root != NULL){
    temp_ast_child_root->id->parent = p;
    temp_ast_child_root = temp_ast_child_root->next;
  }
}

void fill_ast_node(struct ast** t, int val, char* token, bool is_leaf, int ntoken){
  (*t) = (struct ast*)malloc(sizeof(struct ast));
  (*t)->id = val;                                     //set value into id field
  (*t)->next = NULL;                                  //set next pointer to NULL
  (*t)->token = token;
  (*t)->is_leaf = is_leaf;
  (*t)->ntoken = ntoken;
  (*t)->type = UNKNOWNTYPE;
  if (ast_child_root != NULL){                          //if child doesnot exist,
    (*t)->child = ast_child_root;                     //set current child root pointer to child field
    ast_child_root = NULL;                              //Set the child root to NULL as we intend to set a new list
    insert_parent(*t);
  } else {
    (*t)->child = NULL;                               //
  }
}

void fill_ast_node_type(struct ast** t, int val, char* token, bool is_leaf, int ntoken, DataType type){
  (*t) = (struct ast*)malloc(sizeof(struct ast));
  (*t)->id = val;                                     //set value into id field
  (*t)->next = NULL;                                  //set next pointer to NULL
  (*t)->token = token;
  (*t)->is_leaf = is_leaf;
  (*t)->ntoken = ntoken;
  (*t)->type = type;

  if (ast_child_root != NULL){                          //if child doesnot exist,
    (*t)->child = ast_child_root;                     //set current child root pointer to child field
    ast_child_root = NULL;                              //Set the child root to NULL as we intend to set a new list
    insert_parent(*t);
  } else {
    (*t)->child = NULL;                               //
  }
}

void insert_node_tmp(int val, char* token, bool is_leaf, int ntoken){
  if (ast_list_root == NULL){                               //Create a new node and set the value if root is empty
    fill_ast_node(&ast_list_root, val, token, is_leaf, ntoken);
  } else {                                                  //Else create a ast graph node and assign it to next
    struct ast* ptr;                                        //Create a temp pointer
    fill_ast_node(&ptr, val, token, is_leaf, ntoken);

    //Traverse root and set the new pointer at the next of last node
    struct ast** tmp = &ast_list_root;
    while ((*tmp)->next != NULL) {
      tmp = &((*tmp)->next);    // last_node will store the pointer to last node
    }
    (*tmp)->next = ptr;
  }
}

void insert_node_tmp_type(int val, char* token, bool is_leaf, int ntoken, DataType type){
  if (ast_list_root == NULL){                               //Create a new node and set the value if root is empty
    fill_ast_node_type(&ast_list_root, val, token, is_leaf, ntoken, type);
  } else {                                                  //Else create a ast graph node and assign it to next
    struct ast* ptr;                                        //Create a temp pointer
    fill_ast_node_type(&ptr, val, token, is_leaf, ntoken, type);

    //Traverse root and set the new pointer at the next of last node
    struct ast** tmp = &ast_list_root;
    while ((*tmp)->next != NULL) {
      tmp = &((*tmp)->next);    // last_node will store the pointer to last node
    }
    (*tmp)->next = ptr;
  }
}

int insert_node(char* token, int ntoken) {
  current_node_id++;
  insert_node_tmp(current_node_id, token, ast_child_root == NULL, ntoken);
  return current_node_id;
}

int insert_node_type(char* token, int ntoken, DataType type) {
  current_node_id++;
  insert_node_tmp_type(current_node_id, token, ast_child_root == NULL, ntoken, type);
  return current_node_id;
}

void insert_child(int val){        //This function helps to fill the child list
  struct ast* node = find_ast_node(val);
  if (ast_child_root == NULL) {     //If null list
    (ast_child_root) = (struct ast_child*)malloc(sizeof(struct ast_child));  //allocate memory
    // Set id and next field of the ast_child_root
    (ast_child_root)->id = node;
    (ast_child_root)->next = NULL;
  } else {
    //Create the node
    struct ast_child* ptr;
    ptr = (struct ast_child*)malloc(sizeof(struct ast_child));
    ptr->id = node;
    ptr->next = NULL;

    //Traverse the list and set the node at top
    struct ast_child** temp = &ast_child_root;     // Set child root in a temporary variable
    struct ast_child** last_child_node = NULL;
    while ((*temp)->next != NULL) {           // Until next is NULL traverse the list
      temp = &((*temp)->next);              // temp will hold the address of next node
    }
    (*temp)->next = ptr;
  }
}

void insert_children (int n, ...){
  va_list vl;
  va_start(vl,n);
  for (int i = 0; i < n; i++)
    insert_child(va_arg(vl, int));
  va_end(vl);
}

struct ast* find_ast_node(int id){
  struct ast* temp_root = ast_list_root;
  while(temp_root != NULL){
    if (id == temp_root->id) return temp_root;
    temp_root = temp_root->next;
  }
  return NULL;
}

struct ast* find_parent(struct ast* ast_node, struct ast* ast_parent){
  struct ast* tmp = ast_node;
  while(tmp != NULL) {
    if (tmp == ast_parent) return tmp;
    tmp = tmp->parent;
  }
  return NULL;
}

struct ast* get_root(struct ast* ast_node){
  struct ast* tmp = ast_node;
  while(tmp->parent != NULL) {
    tmp = tmp->parent;
  }
  return tmp;
}

struct ast* get_child(struct ast* ast_node, int id){
  struct ast_child* temp_child_root = ast_node->child;
  int child_num = 0;
  while(temp_child_root != NULL) {
    child_num++;
    if (child_num == id) {
      return temp_child_root->id;
    }
    temp_child_root = temp_child_root->next;
  }
  return NULL;
}

int get_child_num(struct ast* ast_node){
  struct ast_child* temp_child_root = ast_node->child;
  int child_num = 0;
  while(temp_child_root != NULL) {
    child_num++;
    temp_child_root = temp_child_root->next;
  }
  return child_num;
}

const char* dataTypeToString(DataType type) {
switch (type) {
    case UNKNOWNTYPE: return "UNKNOWN";
    case INTTYPE: return "INT";
    case BOOLTYPE: return "BOOL";
  }
}


int checkType(struct ast* node){
    if(strcmp(node->token, "=") == 0 
    || strcmp(node->token, ">") == 0 
    || strcmp(node->token, "<") == 0 
    || strcmp(node->token, ">=") == 0 
    || strcmp(node->token, "<=") == 0 
    ) {
        if(node->child->id->type != node->child->next->id->type) {
            perror("arg types of operator should be the same.\n");
            exit(EXIT_FAILURE);
        }
    } else if(strcmp(node->token, "+") == 0) {
        struct ast_child* tmp = node->child;
        for(int i = 0; i < get_child_num(node); i++){
            if(tmp->id->type != INTTYPE) {
                perror("arg types of + should be INT.\n");
                exit(EXIT_FAILURE);
            }
            if(tmp->next){
                tmp = tmp->next;
            }
        }
    } else if(strcmp(node->token, "-") == 0) {
        if(node->child->id->type != INTTYPE
        || node->child->next->id->type != INTTYPE) {
            perror("arg types of - should be INT.\n");
            exit(EXIT_FAILURE);
        }
    } else if(strcmp(node->token, "*") == 0) {
        struct ast_child* tmp = node->child;
        for(int i = 0; i < get_child_num(node); i++){
            if(tmp->id->type != INTTYPE) {
                perror("arg types of * should be INT.\n");
                exit(EXIT_FAILURE);
            }
            if(tmp->next){
                tmp = tmp->next;
            }
        }
    } else if(strcmp(node->token, "div") == 0) {
        if(node->child->id->type != INTTYPE
        || node->child->next->id->type != INTTYPE) {
            perror("arg types of div should be INT.\n");
            exit(EXIT_FAILURE);
        }
    } else if(strcmp(node->token, "mod") == 0) {
        if(node->child->id->type != INTTYPE
        || node->child->next->id->type != INTTYPE) {
            perror("arg types of mod should be INT.\n");
            exit(EXIT_FAILURE);
        }
    } else if(strcmp(node->token, "not") == 0) {
        if(node->child->id->type != BOOLTYPE) {
            perror("arg types of not operator should be BOOL.\n");
            exit(EXIT_FAILURE);
        }
    } else if(strcmp(node->token, "and") == 0) {
        struct ast_child* tmp = node->child;
        for(int i = 0; i < get_child_num(node); i++){
            if(tmp->id->type != BOOLTYPE) {
                perror("arg types of and operator should be BOOL.\n");
                exit(EXIT_FAILURE);
            }
            if(tmp->next){
                tmp = tmp->next;
            }
        }
    } else if(strcmp(node->token, "and") == 0) {
        struct ast_child* tmp = node->child;
        for(int i = 0; i < get_child_num(node); i++){
            if(tmp->id->type != BOOLTYPE) {
                perror("arg types of and operator should be BOOL.\n");
                exit(EXIT_FAILURE);
            }
            if(tmp->next){
                tmp = tmp->next;
            }
        }
    } else if(strcmp(node->token, "if") == 0) {
        if(node->child->id->type != BOOLTYPE) {
            perror("the first arg types of if should be BOOL.\n");
            exit(EXIT_FAILURE);
        }
        if(node->child->next->next->id->type != node->child->next->id->type) {
            perror("types of the last two args of if should be the same.\n");
            exit(EXIT_FAILURE);
        }
    } else if(strcmp(node->token,"call func") == 0){
        struct ast_child* tmp = node->child;
        char* funname = tmp->id->token;
        struct FuncTable* funtmp = funcRoot;
        int artnum = 0; 
        DataType* funarglist = (DataType*) malloc(artnum * sizeof(DataType));
        char** funargnamelist = (char**) malloc(artnum * sizeof(char*));

        // get funarglist and funargnamelist
        while(funtmp){
          if(strcmp(funtmp->entry->name,funname) == 0){
            artnum = funtmp->arities;
            struct SymbolTable* localfuntmp = funtmp->entry->next;
            for (int i = 0;i < artnum;i++){
              funarglist[i] = localfuntmp->type;
              funargnamelist[i] = localfuntmp->name;
              localfuntmp = localfuntmp->next;
            }
          }
          funtmp = funtmp->next;
        }
        DataType* inputarglist = (DataType*) malloc(artnum * sizeof(DataType));
        for (int i = 0; i < artnum; i++){
          tmp = tmp->next;
          inputarglist[i] = tmp->id->type;
        }

        // printf("print inputarglist.\n");
        // for (int i = 0; i < artnum; i++){
        //   printf("%d",inputarglist[i]);
        //   printf("\n");
        // }

        // printf("print funarglist.\n");
        // for (int i = 0; i < artnum; i++){
        //   printf("%d",funarglist[i]);
        //   printf("\n");
        // }


        for (int i = 0; i < artnum; i++){
          if (inputarglist[i] != funarglist[i]){
            printf("the argument #%d of %s does not type check with type of %s.\n",i+1,funargnamelist[i],dataTypeToString(funarglist[i]));
            perror("Argument Type Mismatch.\n");
            exit(EXIT_FAILURE);
          }
        }
    }
    return 0;
}


int visit_ast(int (*f)(struct ast* ast_node)){
  struct ast* temp_root = ast_list_root;
  while(temp_root != NULL){
    if (f (temp_root) != 0) return 1;
    temp_root = temp_root->next;
  }
  return 0;
}

FILE *fp;
int print(struct ast* temp_root) {
  if (!temp_root->is_leaf){
    fprintf(fp, "%d [label=\"%s\", fontname=\"monospace\", style=filled, fillcolor=mintcream];\n ", temp_root->id, temp_root->token);
  } else {
    fprintf(fp, "%d [label=\"%s\", fontname=\"monospace\"];\n ", temp_root->id, temp_root->token);
  }
  if (temp_root->child != NULL){
    struct ast_child* temp_ast_child_root = temp_root->child;
    while(temp_ast_child_root != NULL){
      fprintf(fp, "%d->%d\n ", temp_root->id, temp_ast_child_root->id->id);
      temp_ast_child_root = temp_ast_child_root->next;
    }
  }
  return 0;
}

void print_ast(){
  fp = fopen("ast.dot", "w");
  fprintf(fp, "digraph print {\n ");
  visit_ast(print);
  fprintf(fp, "}\n ");
  fclose(fp);
  system("dot -Tpdf ast.dot -o ast.pdf");
}

void free_ast() {
  struct ast* temp_root = ast_list_root;
  struct ast* current_root = NULL;
  while(temp_root != NULL){
    if (temp_root->child != NULL){
      struct ast_child* temp_ast_child_root = temp_root->child;
      struct ast_child* current_child = NULL;
      while(temp_ast_child_root != NULL){
        current_child = temp_ast_child_root;
        temp_ast_child_root = temp_ast_child_root->next;
        free(current_child);
      }
    }
    current_root = temp_root;
    temp_root = temp_root->next;
    free(current_root);
  }
}
int assignType(struct ast* node) {

    set_type(node);
    // printf(" %s, type:%d\n", node->token, node->type);

    return 0;
}

void asttree(){
    // printf("fun: %d\n",get_type(ast_list_root));
    // printf("fun: %d\n",ast_list_root->next->next->next->next->type);
    // printf("fun: %d\n",ast_list_root->parent->child->next->next->id->type);
    // printf("fun: %d\n",get_type(ast_list_root->parent->child->next->next->next->id));
    // printf("%s\n", funcRoot->entry->name);

    visit_ast(assignType);

    visit_ast(checkType);


}


DataType set_type(struct ast* node) {
    if(node->type != UNKNOWNTYPE) { // type has defined;
        return node->type;
    }
    // type has not defined;
    DataType ty = UNKNOWNTYPE;

    if(!node->parent) { // entry
        node->type = set_type(node->child->next->id);
        return node->type;
    }


    // case 1, node is a function
    if(strcmp(node->token, "definefun") == 0) {
        node->type = set_type(node->child->next->next->id);
        return node->type;
    } else if(strcmp(node->parent->token, "definefun") == 0 && node->parent->child->id == node) { // funcName
        node->type = set_type(node->parent->child->next->next->id);
        // update FuncTable
        {
            struct FuncTable* f = funcRoot;
            while (f)
            {
                if(strcmp(f->entry->name, node->token) == 0) {
                    f->entry->type = node->type;
                    break;
                }
                f = f->next;
            }
        }

        return node->type;
    } else if(strcmp(node->token, "inputlist") == 0) {
        return UNKNOWNTYPE;
    } else if(strcmp(node->token, "let") == 0) {
        node->type = set_type(node->child->next->next->id);
        return node->type;
    } else if(strcmp(node->parent->token, "let") == 0 && node->parent->child->id == node) { // first child node of let
        push(node->token);
        node->type = set_type(node->parent->child->next->id); // second node
        current->type = node->type;
        return node->type; 
    } else if(strcmp(node->token, "call func") == 0) {
        node->type = set_type(node->child->id);
        return node->type;
    } else if(strcmp(node->token, "if") == 0) {
        node->type = set_type(node->child->next->id);
        return node->type; // secode child node
    } else if(strcmp(node->parent->token, "let") == 0 && node->parent->child->next->next->id == node) { // third term
        removeSymbol(current); // pop
    }

    // check func name
    int isFunc = 0;
    {  // isFunc ?
        if(strcmp(node->token, "print") == 0) {
          return UNKNOWNTYPE;
        }
        struct FuncTable* f = funcRoot;
        while(f) {
            if(strcmp(f->entry->name, node->token) == 0) {
                isFunc = 1;
                break;
            }
            f = f->next;
        }
        if(isFunc){
       //   if(node->parent->child->next->next) {
          // {
          //   node->type = set_type(node->parent->child->next->next->id);
          //   f->entry->type = node->type;
          //   printStack();
          //   printf("---name: %s, type: %d\n", node->token, ty);
          //   return node->type;
          // } 
          // // else {
          node->type = f->entry->type;
          return node->type;
        }
    }

    // check func params
    int isVar = 0;
    int inFunc = 0;
    char* funcName = NULL;
    { // inFunc ?
        struct ast* nd = node;
        while(nd) {
            if(strcmp(nd->token, "definefun") == 0) {
                inFunc = 1;
                funcName = nd->child->id->token;
                break;
            }
            nd = nd->parent;
        }
    }
    if(inFunc) { // func variables
        struct FuncTable* f = funcRoot;
        while(f) {
            if(strcmp(f->entry->name, funcName) == 0) {
                struct SymbolTable* tb = f->entry;
                for(int i = 0; i < f->arities; i++){
                    tb = tb->next;
                    if(strcmp(tb->name, node->token) == 0){
                        isVar = 1;
                        node->type = tb->type;

                        return node->type;
                    }
                }
                break;
            }
            f = f->next;
        }
    }

    // if (strcmp(node->parent->token, "let") == 0 && node->parent->child->next->id == node) { // second child node of let
    //     struct SymbolTable* tp = current->last; // could not start from the top of stack
    //     printStack();

    //     while(tp) {
    //         if(strcmp(tp->name, node->token) == 0) {
    //             node->type = tp->type;
    //             return node->type;
    //         }
    //         tp = tp->last;
    //     }
    // } 

    {
        struct SymbolTable* tp = current; // start from the top of stack
        while(tp) {
            if(strcmp(tp->name, node->token) == 0) {
                node->type = tp->type;
                break;
            }
            tp = tp->last;
        }

        return node->type;
    };

    return ty;
}



