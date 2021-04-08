/* abstract-tree.h */
#ifndef __ABSTRACT_TREE__
#define __ABSTRACT_TREE__

#include "operator.h"

typedef enum {
  Program,
  VarDeclList,
  Identifier,
  CharLiteral,
  IntLiteral,
  DeclVars,
  Type,
  StructType,
  Declarateurs,
  TypesVars,
  DeclFoncts,
  DeclFonct,
  ListTypVar,
  Void,
  EnTeteFonct,
  Corps,
  SuiteInstr,
  Instr,
  DeclChamps,
  DeclStruct,
  UnaryOperator,
  BinaryOperator,
  Assignement,
  Func,
  Arguments,
  StructAccess,
  LoneIf,
  IfElse,
  While,
  BoolOperator,
  Return

} Kind;

typedef struct Node {
  Kind kind;
  union {
    int integer;
    char character;
    char identifier[64];
    char type[64];
    Operator operator;
  } u;
  struct Node *firstChild, *nextSibling;
  int lineno;
} Node;

Node *makeNode(Kind kind);
void addSibling(Node *node, Node *sibling);
void addChild(Node *parent, Node *child);
void deleteTree(Node*node);
void printTree(Node *node);

#define FIRSTCHILD(node) node->firstChild
#define SECONDCHILD(node) node->firstChild->nextSibling
#define THIRDCHILD(node) node->firstChild->nextSibling->nextSibling
#define SIBLING(node) node->nextSibling

#endif
