#include "y.tab.h"
#include "ast.h"
#include "cfg.h"
int yyparse();

int main (int argc, char **argv) {
  int retval = yyparse();
  if (retval == 0) {
    print_ast();
    //traverse_ast();
    struct ast_list* alist = find_all_root();
    //print_ast_list(alist);
    for (int i = 2; i < 3; i++){

      printf("Tree: \n");
      create_CFG(alist->astList[i],i);
      printf("\n");

      // int size = 0;              
      // char* AST_result[100]; 
      // middle_traverse_ast(alist->astList[i], AST_result, &size);
      // for (int i = 0;i < size;i++){
      //     printf("%s\n",AST_result[i]);
      // }

      // printf("\n\n");
      

    }
  }
  free_ast();
  return retval;
}

