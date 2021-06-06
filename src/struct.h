#ifndef __STRUCT__
#define __STRUCT__

#include "type.h"
#include "string.h"
#include "abstract-tree.h"
#include <assert.h>

#define MAXSTRUCTS 64 // you can define 64 struct type  
#define MAXMEMBERS 32 // struct types have a maximum of 32 members

typedef struct {
    char name[MAXNAME];
    int n_members;
    struct members{
        char name[MAXNAME];
        native_t type;        
    } members[MAXMEMBERS];
}StructDecl;

extern StructDecl structs[MAXSTRUCTS];
extern int structsSize;

void initStructDecl(StructDecl * s);
/* returns 1 if redeclaration, 2 if table overflow, 0 if okay */
int addStructDecl(Node * node);

int getStructIndex(char * name);
const char * getStructNameFromIndex(unsigned char i);
void printStructs();

native_t getMemberType(StructDecl * decl, char * member);

#endif
