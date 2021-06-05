#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "struct.h"

#define MAXSYMBOLS 256

typedef struct {
    char name[MAXNAME];
    type_kind kind;
    union {
        native_t native;
        unsigned char struc;
    } type;
} STentry;

extern STentry globalST[MAXSYMBOLS];
extern STentry funcST[MAXSYMBOLS];
extern int globalSTsize;
extern int funcSTsize;

void printSTSymbols(STentry * table, int size);

/* returns 1 if redeclaration, 2 if table overflow, 0 if okay */
int addVarToST_native(const char name[], native_t type, STentry * table, int * size);
int addVarToST_struct(const char name[], unsigned char struct_type, STentry * table, int * size);

int addfuncVar_native(const char name[], native_t type);
int addglobalVar_native(const char name[], native_t type);

int addfuncVar_struct(const char name[], unsigned char struct_type);
int addglobalVar_struct(const char name[], unsigned char struct_type);

void printglobalST();
void printfuncST();
void emptyfuncST();

void create_STFun_with_tree(Node *node);
int findGlobalSymbol(const char name[MAXNAME]);
int getSymbolIndexFromVarName(char *name);

#endif
