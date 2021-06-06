#include "func.h"

int prototypesSize = 0;
fun_prototype prototypes[MAXFUNC];

static const char * getCharFromAnyType(type_kind kind, native_t n, unsigned char s){
    if (kind == native)
    {
        return getCharFromNativeType(n);
    }else{
        return getStructNameFromIndex(s);
    }
}


int addFuncPrototype(Node * node){
    //return:
    // 0 - okay
    // 1 - redeclaration
    // 2 - too many prototypes (functions) relative to MAXFUNC
    // 3 - too many parameters relative to MAXPARAMATERS

    Node * name = SIBLING(FIRSTCHILD(FIRSTCHILD(node)));
    Node * parameter = FIRSTCHILD(SIBLING(SIBLING(FIRSTCHILD(FIRSTCHILD(node)))));
    int params = 0, r;

    assert(node->kind == DeclFonct);

    //Look for a redeclaration (same function name)
    for (size_t i = 0; i < prototypesSize; i++)
    {
        if(!strcmp(name->u.identifier, prototypes[i].name)){
            return 1;
        }
    }

    //Too many prototypes
    if (prototypesSize >= MAXFUNC)
    {
        return 2;
    }

    //Return Type
    if (FIRSTCHILD(FIRSTCHILD(node))->kind == StructType)
    {
        prototypes[prototypesSize].return_type_kind = struc;
        r = getStructIndex(FIRSTCHILD(FIRSTCHILD(node))->u.identifier);
        prototypes[prototypesSize].return_type.struc = r;
        if (r == -1)
        {
            ERR_HEADER(FIRSTCHILD(FIRSTCHILD(node)));
            printf("function has unrecognized return type 'struct %s' \n", FIRSTCHILD(FIRSTCHILD(node))->u.identifier);
            exit(2);
        }
        
    }else{
        prototypes[prototypesSize].return_type_kind = native;
        if (FIRSTCHILD(FIRSTCHILD(node))->kind == Void)
        {
            prototypes[prototypesSize].return_type.native = tpc_void;
        }else{
            prototypes[prototypesSize].return_type.native = FIRSTCHILD(FIRSTCHILD(node))->u.type;
        }
    }

    //Function name
    prototypes[prototypesSize].name = SIBLING(FIRSTCHILD(FIRSTCHILD(node)))->u.identifier;

    //Parameters
    if (SIBLING(SIBLING(FIRSTCHILD(FIRSTCHILD(node))))->kind == Void)
    {
        params = 0;
    }else{
        for (; parameter != NULL; parameter = SIBLING(parameter), params++)
        {
            if (params >= MAXPARAMATERS)
            {
                return 3;
            }

            if (parameter->kind == StructType)
            {
                prototypes[prototypesSize].parameters[params].parameter_type_kind = struc;
                prototypes[prototypesSize].parameters[params].parameter_type.struc = getStructIndex(parameter->u.identifier);
            }else{
                prototypes[prototypesSize].parameters[params].parameter_type_kind = native;
                prototypes[prototypesSize].parameters[params].parameter_type.native = parameter->u.type;
            }
        }
    }
    prototypes[prototypesSize].param_len = params;
    prototypesSize++;
    return 0;
}

int getIndexFromFunName(char *name){
    int i = 0;
    for(i = 0; i < prototypesSize; i++){
        if(strcmp(name, prototypes[i].name) == 0)
            return i;
    }
    return -1; /*Func doesn't exist*/
}

void printPrototypes(){
    printf("Function prototypes (%d prototype(s)) ->\n", prototypesSize);
    for (size_t i = 0; i < prototypesSize; i++)
    {
        putchar('\t');
        if (prototypes[i].return_type_kind == native)
        {
            printf("%s ", getCharFromNativeType(prototypes[i].return_type.native));
        }else{
            printf("%s ", getStructNameFromIndex(prototypes[i].return_type.struc));
        }
        printf("%s ", prototypes[i].name);
        if (prototypes[i].param_len == 0)
        {
            printf("(void);\n");
        }else{
            printf("(");
            printf("%s", getCharFromAnyType(prototypes[i].parameters[0].parameter_type_kind, prototypes[i].parameters[0].parameter_type.native, prototypes[i].parameters[0].parameter_type.struc));
            for (size_t j = 1; j < prototypes[i].param_len; j++)
            {
                printf(", %s", getCharFromAnyType(prototypes[i].parameters[j].parameter_type_kind, prototypes[i].parameters[j].parameter_type.native, prototypes[i].parameters[j].parameter_type.struc));
            }
            printf(");\n");
        }
    }
}
