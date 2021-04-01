#include <stdio.h>
#include <stdlib.h>

int begin_asm(FILE *file){
    FILE *file;
    if(NULL == (file = fopen("./comp.asm", "w")))
        return -1;
    fprintf(file, "section .data\n\ttable dq");
}
