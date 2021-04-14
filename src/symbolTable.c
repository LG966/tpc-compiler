#include "symbolTable.h"

STentry globalST[MAXSYMBOLS];
STentry funcST[MAXSYMBOLS];

int globalSTsize=0;
int funcSTsize=0;


void printSTSymbols(STentry * table, int size){
    int i;
    /* printf("%%--- Symbol table ---%%\n"); */
    for(i=0; i!=size; i++){
        printf("\t%d -- %s of type %s\n", i, table[i].name, getCharFromType(table[i].type));
    }
}

int addVarToST(const char name[], Type_tpc type, STentry * table, int * size){
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
    strncpy(table[*size].name, name, MAXNAME - 1);
    table[*size].type = type;

    (*size)++;

    return 0;
}


int addfuncVar(const char name[], Type_tpc type){
    return addVarToST(name, type, funcST, &funcSTsize);
}


int addglobalVar(const char name[], Type_tpc type){
    return addVarToST(name, type, globalST, &globalSTsize);
}

void printglobalST(){
    printf("\n******* GLOBALS ********\n");
    printSTSymbols(globalST, globalSTsize);
}

void printfuncST(){
    printf("******* LOCALS ********\n");
    printSTSymbols(funcST, funcSTsize);
}

void emptyfuncST(){
    funcSTsize=0;
}
