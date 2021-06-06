#ifndef __FUNC__
#define __FUNC__

#include "type.h"
#include "struct.h"
#include "abstract-tree.h"

#define MAXPARAMATERS 16
#define MAXFUNC 128

typedef struct func{
    //function name
    char *name;

    //return type
    type_kind return_type_kind;
    union
    {
        native_t native;
        unsigned char struc;
    }return_type;

    // number of parameters
    unsigned char param_len;

    //parameters type
    //we don't need the parameter name
    struct
    {
        type_kind parameter_type_kind;
        union
        {
            native_t native;
            unsigned char struc;
        }parameter_type;
    }parameters[MAXPARAMATERS];

}fun_prototype;


extern fun_prototype prototypes[MAXFUNC];
extern int prototypesSize;

int addFuncPrototype(Node * node);

int getIndexFromFunName(char *name);

void printPrototypes();

#endif
