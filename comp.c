#include "y.tab.h"
#include "ast.h"
#include "cfg.h"
#include "lr.h"
int yyparse();

int main (int argc, char **argv) {
  int retval = yyparse();
  print_ast();

  testGraph();

  // if (retval == 0) {
  //   print_ast();
  //   // traverse_ast();
  //   // traverse_ast_and_print();
  //   struct ast_list* alist = find_all_root();
  //   // print_ast_list(alist);
  //   int* root_id = (int*)malloc(sizeof(int)); 
  //   *root_id = 0;
  //   int* dot_id = (int*)malloc(sizeof(int)); 
  //   *dot_id = 0;

  //   const char* filename = "cfg.dot";
  //   FILE* fp = fopen(filename, "w");
  //   fprintf(fp, "digraph print {\n ");

  //   for (int i = 0; i < alist->length; i++){
  //     create_CFG(alist->astList[i],root_id,dot_id);
  //   }

  //   printCFGList(fp);
    
  //   fprintf(fp, "}\n ");
  //   fclose(fp);
  // }
  system("dot -Tpdf cfg.dot -o cfg.pdf");
  free_ast();
  return retval;
}

