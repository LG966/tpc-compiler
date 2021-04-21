#include "struct.h"

StructDecl structs[MAXSTRUCTS];
int structsSize;

#define DEBUG(X) printf("%d\n", X);

void initStructDecl(StructDecl * s){
    memset(s, 0, sizeof(StructDecl));
}

static int addMember(const char name[], native_t type, StructDecl * struc){
    int count;
    for (count=0 ; count < struc->n_members ; count++)
    {
        if (!strcmp(struc->members[count].name, name))
        {
            /* printf("semantic error, redefinition of variable %s near line %d\n",
            name, lineno); */
            return 1;
        }
    }

    if (struc->n_members + 1 > MAXMEMBERS)
    {
        return 2;
    }
    
    struc->members[struc->n_members].type = type;
    strcpy(struc->members[struc->n_members].name, name);
    struc->n_members++;

    return 0;
}

int addStructDecl(Node * node){
    int i, rtr;
    Node * types, * ident;
    assert(node->kind == DeclStruct);

    // too much struct declarations ?
    if(structsSize >= MAXSTRUCTS)
    {
        return 2;
    }

    // Look for a possible redeclaration
    for (i = 0; i < structsSize; i++)
    {
        if (!strcmp(structs[i].name, (FIRSTCHILD(node))->u.identifier))
        {
            return 1;
        }
    }

    types = (FIRSTCHILD(SECONDCHILD(node)));

    for(; types != NULL; types = SIBLING(types))
    {
        for (ident = FIRSTCHILD(FIRSTCHILD(types)); ident != NULL; ident = SIBLING(ident))
        {
            rtr = addMember(ident->u.identifier, types->u.type, &(structs[structsSize]));
            if (rtr) return 3; // member redeclaraction if rtr==1
        }
    }

    strcpy(structs[structsSize].name, FIRSTCHILD(node)->u.identifier);

    structsSize++;

    return 0;
}

void printStructs(){
    int i, j;
    printf("\n******* STRUCTS ********\n");
    printf("--- Number of structs : %d ---\n", structsSize);

    for (i = 0; i < structsSize; i++)
    {
        printf("\tStruct %s :\n", structs[i].name);
        for (j = 0; j < structs[i].n_members; j++)
        {
            printf("\t\t%s %s;\n", getCharFromNativeType(structs[i].members[j].type), structs[i].members[j].name);
        }
    }
}

int getStructIndex(char * name){
    int i;

    for (i = 0; i < structsSize; i++)
    {
        if (strcmp(structs[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}


const char * getStructNameFromIndex(unsigned char i){
    assert(i < structsSize);
    return structs[i].name;
}