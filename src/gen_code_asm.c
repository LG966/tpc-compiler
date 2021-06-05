#include "gen_code_asm.h"

static const char *ParametersRegister[] = {
  "rdi",
  "rsi",
  "rdx",
  "rcx",
  "r8",
  "r9"
};

StackPlace FuncPlace[MAXSYMBOLS];
int SizeFuncPlace = 0;

void begin_texte_asm(FILE *file){
    fprintf(file, "section .text\nglobal _start\n\n");
    /*puis appel fonction traduction*/
}

void begin_data_asm(FILE *file, STentry *symbolTable){
    int i = 0;
    fprintf(file, "section .data\n");
    for(i = 0; i < globalSTsize; i++){
        if(symbolTable[i].kind == native)
            fprintf(file, "\t%s : dq 0\n", symbolTable[i].name);
        /*faire le cas de type structure ?*/
    }
    fprintf(file, "\n");
}

void call_func(FILE *file, Node *node, int i){
    if(node == NULL){
      return;
    }
    for ( Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
        call_func(file, child, i);
        i = i + 1;
    }
    switch(node->kind){
      case IntLiteral:
          if(i < 6) /*mov in register if less than 6 parameters*/
              fprintf(file, "\tmov %s, %d\n", ParametersRegister[i], node->u.integer);
          break;
      case CharLiteral:
          if(i < 6) /*mov in register if less than 6 parameters*/
              fprintf(file, "\tmov %s, '%c'\n", ParametersRegister[i], node->u.character);
          /*Faire hors registre + si ident*/
          break;
      default: break;
    }
}

void write_expresion(FILE *file, Node *node ){
  int index_ident = 0;
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
        }
        fprintf(file, "\tpush rbx\n");
        break;
    case Func : /*faire nouveau bloc*/call_func(file, FIRSTCHILD(node), 0); break;
    case Identifier:
        index_ident = getPlaceFromName(node->u.identifier);
        if(findGlobalSymbol(node->u.identifier))
            fprintf(file, "\tpush qword [%s]\n", node->u.identifier);
        else if(FuncPlace[index_ident].stack == 1)
            fprintf(file, "\tpush qword [rsp + %d]\n", FuncPlace[index_ident].place);
        else
            fprintf(file, "\tpush %s\n", FuncPlace[index_ident].reg);
        break;
    default: break;
  }
}

void gen_code_by_tree(FILE *file, STentry *symbolTable, Node *node){
  int index_ident = 0;
  if(node == NULL){
    return;
  }
  for ( Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
    gen_code_by_tree(file, symbolTable, child);
  }
  switch(node->kind){
    case Assignement:
        write_expresion(file, node);
        index_ident = getPlaceFromName(FIRSTCHILD(node)->u.identifier);
        if(findGlobalSymbol(FIRSTCHILD(node)->u.identifier) == 1)
            fprintf(file, "\tpop qword [%s]\n", FIRSTCHILD(node)->u.identifier);
        else if(FuncPlace[index_ident].stack == 1)
            fprintf(file, "\tpop qword [rsp + %d]\n", FuncPlace[index_ident].place);
        else
            fprintf(file, "\tpop %s\n", FuncPlace[index_ident].reg);
        break;
    case ListTypVar: empty_FunPlace(); emptyfuncST(); create_STFun_with_tree(node); break;
    case DeclVars: create_STFun_with_tree(node); create_FuncPlace_with_FuncST(); printFuncPlace(); printfuncST(); break;
    case EnTeteFonct:
        index_func = getIndexFromFunName(SECONDCHILD(node)->u.identifier);
          if(strcmp(prototypes[index_func].name, "main") == 0)
              fprintf(file, "_start:\n");
          else
              fprintf(file, "%s:\n", prototypes[index_func].name);
          break;
    case Return:
        write_expresion(file, node);
        if(strcmp(prototypes[index_func].name, "main") == 0)
            fprintf(file, "\tpop rdi\n\tmov rax, 60\n\tsyscall");
        else
            fprintf(file, "\tpop rax\n\tret\n\n");
    break;
    default: break;
  }
}

void create_FuncPlace_with_FuncST(){
    int i = 0;
    for(i = 0; i < funcSTsize; i++){
        addFuncPlace(funcST[i].name);
    }
}

void addFuncPlace(char *name){
    /*name is a parameter*/
    int indexOfNameInST = getSymbolIndexFromVarName(name);
    strcpy(FuncPlace[SizeFuncPlace].name, name);
    FuncPlace[SizeFuncPlace].stack = 1;
    if(prototypes[index_func].param_len < 6)
        FuncPlace[SizeFuncPlace].place = (indexOfNameInST - prototypes[index_func].param_len) * 8;
    else
        FuncPlace[SizeFuncPlace].place = (indexOfNameInST - 6) * 8;

    if(indexOfNameInST < prototypes[index_func].param_len){
        /*6 registers for params*/
        if(indexOfNameInST < 6){
          FuncPlace[SizeFuncPlace].stack = 0;
          strcpy(FuncPlace[SizeFuncPlace].reg, ParametersRegister[indexOfNameInST]);
        }
    }
    SizeFuncPlace ++;
}

int getPlaceFromName(char *name){
    int i = 0;
    for(i = 0; i < SizeFuncPlace; i++){
        if(strcmp(FuncPlace[i].name, name) == 0)
            return i;
    }
    return -1; /*Variable isn't defined*/
}

void printFuncPlace(){
    int i = 0;
    for(i = 0; i < SizeFuncPlace; i++){
        if(FuncPlace[i].stack == 0)
            printf("%s est dans le registre %s\n", FuncPlace[i].name, FuncPlace[i].reg);
        else
            printf("%s est dans [rsp + %d]\n", FuncPlace[i].name, FuncPlace[i].place);
    }
}

void empty_FunPlace(){
    SizeFuncPlace = 0;
}
