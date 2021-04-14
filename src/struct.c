#include "struct.h"

StructDecl structs[MAXSTRUCTS];
int structsSize;

#define DEBUG(X) printf("%d\n", X);

void initStructDecl(StructDecl * s){
    memset(s, 0, sizeof(StructDecl));
}

int addStructDecl(Node * node){
    int i;
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

        if (strcmp(structs[i].name, (FIRSTCHILD(node))->u.identifier))
        {
            return 1;
        }
    }

    types = (FIRSTCHILD(SECONDCHILD(node)));

    for(; types != NULL; types = SIBLING(types))
    {
        for (ident = FIRSTCHILD(FIRSTCHILD(types)); ident != NULL; ident = SIBLING(ident))
        {
            //printf("ident = %s - n_members = %d\n", ident->u.identifier, structs[structsSize].n_members);
            addVarToST(ident->u.identifier, types->u.type, structs[structsSize].members, &(structs[structsSize].n_members));
        }
    }

    strcpy(structs[structsSize].name, FIRSTCHILD(node)->u.identifier);

    structsSize++;

    return 0;
}

void printStructs(){
    int i;
    printf("\n******* STRUCTS ********\n");
    printf("--- Number of tables : %d ---\n", structsSize);

    for (i = 0; i < structsSize; i++)
    {
        printf("Struct %s :\n", structs[i].name);
        printSTSymbols(structs[i].members, structs[i].n_members);
    }
}