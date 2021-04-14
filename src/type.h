#ifndef __TYPE__
#define __TYPE__

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define MAXNAME 64
#define MAX

typedef enum {
    tpc_int,
    tpc_char,
    tpc_void
}Type_tpc;

size_t typeSize(Type_tpc t);
char * getCharFromType(Type_tpc t);


#endif