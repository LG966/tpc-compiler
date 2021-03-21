#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXNAME 64
#define MAXTYPE 64

typedef struct {
    char name[MAXNAME];
    char type[MAXTYPE]; // devrait avoir un enum consacré aux types
} STentry;

int addVar(const char name[], const char type[]);
void printSymbols();
void initSymbolTable();


#endif