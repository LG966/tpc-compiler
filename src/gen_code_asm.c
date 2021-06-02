#include "gen_code_asm.h"

void begin_texte_asm(FILE *file){
    fprintf(file, "section .text\nglobal _start\n_start:\n");
    /*puis appel fonction traduction*/
}

void begin_data_asm(FILE *file, STentry *symbolTable){
    int i = 0;
    fprintf(file, "section .data\n");
    for(i = 0; i < globalSTsize; i++){
        if(symbolTable[i].kind == native){
            if (strcmp(getCharFromNativeType(symbolTable[i].type.native), "int") == 0)
                fprintf(file, "\t%s : dq 0\n", symbolTable[i].name);
            else
                fprintf(file, "\t%s : db 0\n", symbolTable[i].name);
            }
        /*faire le cas de type structure ?*/
    }
    fprintf(file, "\n");
}

void write_expresion(FILE *file, Node *node ){
  if(node == NULL){
    return;
  }
  for ( Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
      write_expresion(file, child);
  }
  switch (node->kind) {
    case IntLiteral: fprintf(file, "\tpush qword %d\n", node->u.integer); break;
    case BinaryOperator :
        fprintf(file, "\tpop rcx\n\tpop rbx\n");
        switch(getStringFromOperator(node->u.operator)[0]){
          case '-': fprintf(file, "\tsub rbx, rcx\n"); break;
          case '*': fprintf(file, "\timul rbx, rcx\n"); break;
          case '+': fprintf(file, "\tadd rbx, rcx\n"); break;
          case '/': fprintf(file, "\tmov rax, rbx\n\tmov rdx, 0\n\tidiv rcx\n\tmov rbx, rax\n"); break;
          case '%': fprintf(file, "\tmov rax, rbx\n\tmov rdx, 0\n\tidiv rcx\n\tmov rbx, rdx\n"); break;
          default: fprintf(file, "%s", getStringFromOperator(node->u.operator));break;
        };
        fprintf(file, "\tpush rbx\n");
        break;
    case Identifier: fprintf(file, "\tpush qword [%s]\n", node->u.identifier); break;
    default: break;
  }
}

void gen_code_by_tree(FILE *file, STentry *symbolTable, Node *node){
  if(node == NULL){
    return;
  }
  for ( Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
    gen_code_by_tree(file, symbolTable, child);
  }

  switch(node->kind){
    case Assignement: write_expresion(file, node); fprintf(file, "\tpop qword [%s]\n", node->firstChild->u.identifier); break;
    default: break;
  }
}
