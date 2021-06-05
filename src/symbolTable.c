#include "symbolTable.h"

STentry globalST[MAXSYMBOLS];
STentry funcST[MAXSYMBOLS];

int globalSTsize=0;
int funcSTsize=0;

void create_STFun_with_tree(Node *node){
  if(node == NULL){
    return;
  }
  for ( Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
      create_STFun_with_tree(child);
  }
  switch(node->kind){
      case Type:
          if(node->firstChild->kind != Declarateurs)
              for(Node *child = node->firstChild; child != NULL;child = child->nextSibling){
                      addfuncVar_native(child->u.identifier, node->u.type);
              }
          else{
            for(Node *child = node->firstChild->firstChild; child != NULL;child = child->nextSibling){
                    addfuncVar_native(child->u.identifier, node->u.type);
            }
          }
          break;
      default: break;
  }
}

int findGlobalSymbol(const char name[MAXNAME]){
    int i = 0;
    for (i = 0; i < globalSTsize; i++){
        if(strcmp(globalST[i].name, name) == 0){
            return 1;
        }
    }
    return 0;
}

void printSTSymbols(STentry * table, int size){
    int i;
    /* printf("%%--- Symbol table ---%%\n"); */
    for(i=0; i!=size; i++){
        if (table[i].kind == native)
        {
            printf("\t%d -- %s of native type %s\n", i, table[i].name, getCharFromNativeType(table[i].type.native));
        } else {
            printf("\t%d -- %s of struct type %s\n", i, table[i].name, getStructNameFromIndex(table[i].type.native));
        }
    }
}

int addVarToST_native(const char name[], native_t type, STentry * table, int * size){
    int count;
    for (count=0;count<*size;count++) {
        if (!strcmp(table[count].name,name)) {
            /* printf("semantic error, redefinition of variable %s near line %d\n",
            name, lineno); */
            return 1;
        }
    }
    if (*size+1 > MAXSYMBOLS) {
        /* printf("too many variables near line %d\n", lineno);
        exit(1); */ return 2;
    }
    table[*size].kind = native;
    strncpy(table[*size].name, name, MAXNAME - 1);
    table[*size].type.native = type;

    (*size)++;

    return 0;
}

int addVarToST_struct(const char name[], unsigned char struct_type, STentry * table, int * size){
    int count;
    for (count=0;count<*size;count++) {
        if (!strcmp(table[count].name,name)) {
            /* printf("semantic error, redefinition of variable %s near line %d\n",
            name, lineno); */
            return 1;
        }
    }
    if (*size+1 > MAXSYMBOLS) {
        /* printf("too many variables near line %d\n", lineno);
        exit(1); */ return 2;
    }
    table[*size].kind = struc;
    strncpy(table[*size].name, name, MAXNAME - 1);
    table[*size].type.struc = struct_type;

    (*size)++;

    return 0;
}


int addfuncVar_native(const char name[], native_t type){
    if(findGlobalSymbol(name) == 1)
        return 3; /*Variable déjà définie en global*/
    return addVarToST_native(name, type, funcST, &funcSTsize);
}

int addglobalVar_native(const char name[], native_t type){
    return addVarToST_native(name, type, globalST, &globalSTsize);
}


int addfuncVar_struct(const char name[], unsigned char struct_type){
    return addVarToST_struct(name, struct_type, funcST, &funcSTsize);
}

int addglobalVar_struct(const char name[], unsigned char struct_type){
    return addVarToST_struct(name, struct_type, globalST, &globalSTsize);
}

void printglobalST(){
    printf("\n******* GLOBALS ********\n");
    printSTSymbols(globalST, globalSTsize);
}

void printfuncST(){
    printf("******* LOCALS ********\n");
    printSTSymbols(funcST, funcSTsize);
}

int getSymbolIndexFromVarName(char *name){
    int i = 0;
    for(i = 0; i < funcSTsize; i++){
        if(strcmp(name, funcST[i].name) == 0)
            return i;
    }
    return -1; /*variable not in funcST*/
}

void emptyfuncST(){
    funcSTsize=0;
}
