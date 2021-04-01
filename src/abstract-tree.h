/* abstract-tree.h */

typedef enum {
  Program = 0,
  VarDeclList = 1,
  Identifier = 2,
  CharLiteral = 3,
  IntLiteral = 4,
  DeclVars = 5,
  Type = 6,
  StructType = 7,
  Declarateurs = 8,
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

  /* and allother node labels */
  /* The list must coincide with the strings in abstract-tree.c */
  /* To avoid listing them twice, see https://stackoverflow.com/a/10966395 */
} Kind;

typedef struct Node {
  Kind kind;
  union {
    int integer;
    char character;
    char identifier[64];
    char type[64];
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
