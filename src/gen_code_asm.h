/* gen_code_asm.h */
#ifndef __GEN_CODE_ASM__
#define __GEN_CODE_ASM__

#include <stdio.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "operator.h"
#include "func.h"

typedef struct stackPlace  {
    char name[MAXNAME];
    int place;  /* Place for variable */
    char reg[4]; /* Register for paremeters */
    int stack; /* 1 for variable in stack */
} StackPlace;

int index_func;

void begin_data_asm(FILE *file, STentry *symbolTable);
void begin_texte_asm(FILE *file);
void write_expresion(FILE *file, Node *node);
void gen_code_by_tree(FILE *file, STentry *symbolTable, Node *node);

void create_FuncPlace_with_FuncST();
int getPlaceFromName(char *name);
void addFuncPlace(char *name);
void printFuncPlace();
void empty_FunPlace();

#endif
