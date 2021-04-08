#include "gen_code_asm.h"


int begin_data_asm(STentry *symbolTable){
    FILE *file;
    int i = 0;
    if(NULL == (file = fopen("./comp.asm", "w")))
        return -1;
    fprintf(file, "section .data\n");
    for(i = 0; i < globalSTsize; i++){
        fprintf(file, "\t%s : dq 0\n", symbolTable[i].name);
    }
    fclose(file);
    return 0;
}
