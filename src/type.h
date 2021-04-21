#ifndef __TYPE__
#define __TYPE__

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define MAXNAME 64

typedef enum {
    struc,
    native
}type_kind;

typedef enum {
    tpc_int,
    tpc_char,
    tpc_void
}native_t;

size_t nativeTypeSize(native_t t);
char * getCharFromNativeType(native_t t);

#endif