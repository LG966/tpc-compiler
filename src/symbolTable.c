#include "symbolTable.h"

#define MAXSYMBOLS 256

STentry globalST[MAXSYMBOLS];
STentry funcST[MAXSYMBOLS];

int globalSTsize=0;
int funcSTsize=0;


static
void printSymbols(STentry * table, int size){
    int i;
    /* printf("%%--- Symbol table ---%%\n"); */
    for(i=0; i!=size; i++){
        printf("\t%d -- %s of type %s\n", i, table[i].name, table[i].type);
    }
}

static
int addVar(const char name[], const char type[], STentry * table, int size){
    int count;
    for (count=0;count<size;count++) {
        if (!strcmp(table[count].name,name)) {
            /* printf("semantic error, redefinition of variable %s near line %d\n",
            name, lineno); */
            return 1;
        }
    }
    if (size+1 > MAXSYMBOLS) {
        /* printf("too many variables near line %d\n", lineno);
        exit(1); */ return 2;
    }
    strncpy(table[size].name, name, MAXNAME - 1);
    strncpy(table[size].type, type, MAXTYPE - 1);

    return 0;
}


int addfuncVar(const char name[], const char type[]){
    int rtr = addVar(name, type, funcST, funcSTsize);
    if (rtr == 0) funcSTsize++;
    return rtr;
}


int addglobalVar(const char name[], const char type[]){
    int rtr = addVar(name, type, globalST, globalSTsize);
    if (rtr == 0) globalSTsize++;
    return rtr;
}

void printglobalST(){
    printSymbols(globalST, globalSTsize);
}

void printfuncST(){
    printSymbols(funcST, funcSTsize);
}

void emptyfuncST(){
    funcSTsize=0;
}