#include "type.h"

size_t typeSize(Type_tpc t){
    switch (t)
    {
    case tpc_int:
        return 4; //4bytes
    case tpc_char:
        return 1; //1byte
    case tpc_void:
        return 0; //en théorie...
    default:
        fprintf(stderr, "Unrecognized type was detected.\n");
        exit(EXIT_FAILURE);
        return 0;
    }
}

char * getCharFromType(Type_tpc t){
    switch (t)
    {
    case tpc_int:
        return "int"; //4bytes
    case tpc_char:
        return "char"; //1byte
    case tpc_void:
        return "void";
    default:
        fprintf(stderr, "Unrecognized type was detected.\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
}

