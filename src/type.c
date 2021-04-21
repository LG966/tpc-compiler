#include "type.h"

size_t nativeTypeSize(native_t t){
    switch (t)
    {
    case tpc_int:
        return 4; //4bytes
    case tpc_char:
        return 1; //1byte
    case tpc_void:
        return 0; //en théorie...
    default:
        fprintf(stderr, "Unrecognized native type was detected.\n");
        exit(EXIT_FAILURE);
        return 0;
    }
}

char * getCharFromNativeType(native_t t){
    switch (t)
    {
    case tpc_int:
        return "int"; //4bytes
    case tpc_char:
        return "char"; //1byte
    case tpc_void:
        return "void";
    default:
        fprintf(stderr, "Unrecognized struct type was detected.\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
}

