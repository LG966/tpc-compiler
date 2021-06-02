/* gen_code_asm.h */
#ifndef __GEN_CODE_ASM__
#define __GEN_CODE_ASM__

#include <stdio.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "operator.h"

void begin_data_asm(FILE *file, STentry *symbolTable);
void begin_texte_asm(FILE *file);
void write_expresion(FILE *file, Node *node);
void gen_code_by_tree(FILE *file, STentry *symbolTable, Node *node);

#endif
