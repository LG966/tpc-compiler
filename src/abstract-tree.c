/* abstract-tree.c */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "abstract-tree.h"
extern int lineno;       /* from lexer */

static const char *StringFromKind[] = {
  "Program",
  "VarDeclList",
  "Identifier",
  "CharLiteral",
  "IntLiteral",
  "DeclVars",
  "Type",
  "StructType",
  "Declarateurs",
  "TypesVars",
  "DeclFoncts",
  "DeclFonct",
  "ListTypVar",
  "Void",
  "EnTeteFonct",
  "Corps",
  "SuiteInstr",
  "Instr",
  "DeclChamps",
  "DeclStruct",
  "UnaryOperator",
  "Operator",
  "Assignement",
  "Func",
  "Arguments",
  "StructAccess"
  /* and all other node labels */
  /* The list must coincide with the enum in abstract-tree.h */
  /* To avoid listing them twice, see https://stackoverflow.com/a/10966395 */
};


Node *makeNode(Kind kind) {
  Node *node = malloc(sizeof(Node));
  if (!node) {
    printf("Run out of memory\n");
    exit(1);
  }
  node->kind = kind;
  node-> firstChild = (node->nextSibling = NULL);
  node->lineno=lineno;
  return node;
}

void addSibling(Node *node, Node *sibling) {
  Node *curr = node;
  while (curr->nextSibling != NULL) {
    curr = curr->nextSibling;
  }
  curr->nextSibling = sibling;
}

void addChild(Node *parent, Node *child) {
  if (parent->firstChild == NULL) {
    parent->firstChild = child;
  }
  else {
    addSibling(parent->firstChild, child);
  }
}

void deleteTree(Node *node) {
  if (node->firstChild) {
    deleteTree(node->firstChild);
  }
  if (node->nextSibling) {
    deleteTree(node->nextSibling);
  }
  free(node);
}

void printTree(Node *node) {
  //fprintf(stderr, "pointer = %p, stack = %p\n", child, __builtin_return_address(0));
  static bool rightmost[128]; // current node is rightmost sibling
  static int depth = 0;       // depth of current node
  for (int i = 1; i < depth; i++) { // 2502 = vertical line
    printf(rightmost[i] ? "    " : "\u2502   ");
  }
  if (depth > 0) { // 2514 = up and right; 2500 = horiz; 251c = vertical and right
    printf(rightmost[depth] ? "\u2514\u2500\u2500 " : "\u251c\u2500\u2500 ");
  }
  printf("%s", StringFromKind[node->kind]);
  switch (node->kind) {
    case IntLiteral: printf(": %d", node->u.integer); break;
    case CharLiteral: printf(": '%c'", node->u.character); break;
    case Identifier: printf(": %s", node->u.identifier); break;
    case StructType:
    case Type : printf(": %s", node->u.type); break;
    case UnaryOperator : printf(": '%c'", node->u.character); break;
    case Operator : printf(": %s", node->u.identifier); break;
    case Func : printf(": %s", node->u.identifier); break;
    default: break;
  }
  printf("\n");
  depth++;

  /* fprintf(stderr, "@ %s \n", StringFromKind[node->kind]);
  for(Node *child = node->firstChild; child != NULL; child = child->nextSibling);
  fprintf(stderr, "Done\n"); */


  for ( Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
    rightmost[depth] = (child->nextSibling) ? false : true;
    printTree(child);
  }
  depth--;
}
