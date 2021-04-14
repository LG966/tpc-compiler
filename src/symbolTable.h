#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"

#define MAXSYMBOLS 256

typedef struct {
    char name[MAXNAME];
    Type_tpc type;
} STentry;

extern STentry globalST[MAXSYMBOLS];
extern STentry funcST[MAXSYMBOLS];
extern int globalSTsize;
extern int funcSTsize;

void printSTSymbols(STentry * table, int size);

/* returns 1 if redeclaration, 2 if table overflow, 0 if okay */
int addVarToST(const char name[], Type_tpc type, STentry * table, int * size);
int addfuncVar(const char name[], Type_tpc type);
int addglobalVar(const char name[], Type_tpc type);
void printglobalST();
void printfuncST();
void emptyfuncST();


#endif
