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
    int* root_id = (int*)malloc(sizeof(int)); 
    *root_id = 0;
    int* dot_id = (int*)malloc(sizeof(int)); 
    *dot_id = 0;

    const char* filename = "cfg.dot";
    FILE* fp = fopen(filename, "w");
    fprintf(fp, "digraph print {\n ");

    for (int i = 0; i < alist->length; i++){

      create_CFG(alist->astList[i],root_id,dot_id,fp);
    }
    fprintf(fp, "}\n ");
    fclose(fp);
  }
  system("dot -Tpdf cfg.dot -o cfg.pdf");
  free_ast();
  return retval;
}

