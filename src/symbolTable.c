#include "symbolTable.h"

#define MAXSYMBOLS 256
STentry *symbolTable;
int STsize=0;


void printSymbols(){
    int i;
    printf("%%--- Symbol table ---%%\n");
    for(i=0; i!=STsize; i++){
        printf("\t%d -- %s of type %s\n", i, symbolTable[i].name, symbolTable[i].type);
    }
}

int addVar(const char name[], const char type[]){
    int count;
    for (count=0;count<STsize;count++) {
        if (!strcmp(symbolTable[count].name,name)) {
            /* printf("semantic error, redefinition of variable %s near line %d\n",
            name, lineno); */
            return 1;
        }
    }
    if (++STsize>MAXSYMBOLS) {
        /* printf("too many variables near line %d\n", lineno);
        exit(1); */ return 2;
    }
    strncpy(symbolTable[STsize - 1].name, name, MAXNAME - 1);
    strncpy(symbolTable[STsize - 1].type, type, MAXTYPE - 1);

    return 0;
}

void initSymbolTable(){
    symbolTable=malloc(MAXSYMBOLS * sizeof(* symbolTable));
    if (!symbolTable) {
        printf("Run out of memory\n");
        exit(1);
    }
}
