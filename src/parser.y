%{
/* parser.y */
/* Syntaxe du TPC pour le projet d'analyse syntaxique de 2020-2021 */
  #include <stdio.h>
  #include "abstract-tree.h"
  #include <string.h>
  #include "symbolTable.h"
  extern int lineno;
  extern int charno;
  int yylex();
  void yyerror(const char *);
  Node * makeIdentifier(char * ident_name);
  Node * makeSimpleType(char * type_name);
%}



%union{
    struct Node * node;
    char string[64];
}

%token CHARACTER
%token NUM
%token <string> IDENT
%token <string> SIMPLETYPE
%token ORDER EQ
%token ADDSUB
%token DIVSTAR
%token OR AND STRUCT IF WHILE RETURN VOID PRINT READC READE
%precedence ')'
%precedence ELSE

%type <node> DeclVars Declarateurs Type TypesVars EnTeteFonct 
%type <node> DeclFoncts DeclFonct Parametres Corps ListTypVar Prog
%type <node> DeclChamps //SuiteInstr Instr 

%%

Prog:  TypesVars DeclFoncts     {
                                    $$ = makeNode(Program);
                                    addChild($$, $1);
                                    addChild($$, $2);
                                    printTree($$);}
    ;
TypesVars:
       TypesVars Type Declarateurs ';'      {
                                                $$ = $1;
                                                addChild($$, $2);
                                                addChild($2, $3);
                                                for(Node * ident = FIRSTCHILD($3); ident != NULL; ident = SIBLING(ident))
                                                {
                                                    /* printf("ident :%s - type :%s\n", ident->u.identifier, $2->u.type); */
                                                    addVar(ident->u.identifier, $2->u.type);
                                                }
                                            }
    |  TypesVars STRUCT IDENT '{' DeclChamps '}' ';'    {
                                                            $$ = $1;
                                                            // Ajout d'un noeud "DeclStruct" pour différencier
                                                            // les déclarations de struct des déclarations
                                                            // de variable de type struct
                                                            // Peut-être pas nécessaire 
                                                            Node * declstruct = makeNode(DeclStruct); 
                                                            addChild($$, declstruct);
                                                            addChild(declstruct, makeIdentifier($3));
                                                            addChild(declstruct, $5);
                                                        }
    |  %empty                                           {$$ = makeNode(TypesVars);}
    ;
Type:
       SIMPLETYPE       {$$ = makeNode(Type); strcpy($$->u.type, $1);}
    |  STRUCT IDENT     {$$ = makeNode(StructType); strcpy($$->u.type, $2);}
    ;
Declarateurs:
       Declarateurs ',' IDENT {
                                $$ = $1;
                                Node * ident = makeNode(Identifier);
                                strcpy(ident->u.identifier, $3);
                                addChild($$, ident);
                              }
    |  IDENT                  {
                                $$ = makeNode(Declarateurs);
                                Node * ident = makeNode(Identifier);
                                strcpy(ident->u.identifier, $1);
                                addChild($$, ident);
                              }
    ;
DeclChamps :
       DeclChamps SIMPLETYPE Declarateurs ';'   {
                                                    $$ = $1;
                                                    Node * type = makeSimpleType($2);
                                                    addChild($$, type);
                                                    addChild(type, $3);
                                                }
    |  SIMPLETYPE Declarateurs ';'              {
                                                    $$ = makeNode(DeclChamps);
                                                    Node * type = makeSimpleType($1);
                                                    addChild($$, type);
                                                    addChild(type, $2);
                                                }
    ;
DeclFoncts:
       DeclFoncts DeclFonct     {
                                    $$ = $1;
                                    addChild($$, $2);                                    
                                }
    |  DeclFonct                {
                                    $$ = makeNode(DeclFoncts);
                                    addChild($$, $1);
                                }
    ;
DeclFonct:
       EnTeteFonct Corps        {
                                    $$ = makeNode(DeclFonct);
                                    addChild($$, $1);
                                    addChild($$, $2);
                                }
    ;
EnTeteFonct:
       Type IDENT '(' Parametres ')'    {
                                            $$ = makeNode(EnTeteFonct);
                                            addChild($$, $1);
                                            Node * ident = makeNode(Identifier);
                                            strcpy(ident->u.identifier, $2);
                                            addChild($$, ident);
                                            addChild($$, $4);
                                        }
    |  VOID IDENT '(' Parametres ')'    {
                                            $$ = makeNode(EnTeteFonct);
                                            addChild($$, makeNode(Void));
                                            Node * ident = makeNode(Identifier);
                                            strcpy(ident->u.identifier, $2);
                                            addChild($$, ident);
                                            addChild($$, $4);
                                        }
    ;
Parametres:
       VOID         {$$ = makeNode(Void);}
    |  ListTypVar   {$$ = $1;}
    ;
ListTypVar:
       ListTypVar ',' Type IDENT    {
                                        $$ = $1;
                                        addChild($$, $3);
                                        addChild($3, makeIdentifier($4));
                                    }
    |  Type IDENT                   {
                                        $$ = makeNode(ListTypVar);
                                        addChild($$, $1);
                                        addChild($1, makeIdentifier($2));
                                    }
    ;
Corps: '{' DeclVars SuiteInstr '}'  {
                                        $$ = makeNode(Corps);
                                        addChild($$, $2);
                                        //addChild($$, $3);    
                                    }
    ;
DeclVars:
       DeclVars Type Declarateurs ';'   {
                                            $$ = $1;
                                            addChild($$, $2);
                                            addChild($2, $3);

                                            for(Node * ident = FIRSTCHILD($3); ident != NULL; ident = SIBLING(ident))
                                            {
                                                /* printf("ident :%s - type :%s\n", ident->u.identifier, $2->u.type); */
                                                addVar(ident->u.identifier, $2->u.type);
                                            }
                                        }
    |  %empty                           {
                                            $$ = makeNode(DeclVars);
                                        }
    ;
SuiteInstr:
       SuiteInstr Instr     {
                                //$$ = $1;
                                //addChild($$, $2);
                            }
    |  %empty               {
                                //$$ = makeNode(SuiteInstr);
                            }
    ;
Instr:
       LValue '=' Exp ';'
    |  READE '(' LValue ')' ';'
    |  READC '(' LValue ')' ';'
    |  PRINT '(' Exp ')' ';'
    |  IF '(' Exp ')' Instr
    |  IF '(' Exp ')' Instr ELSE Instr
    |  WHILE '(' Exp ')' Instr
    |  Exp ';'
    |  RETURN Exp ';'
    |  RETURN ';'
    |  '{' SuiteInstr '}'
    |  ';'
    ;
Exp :  Exp OR TB
    |  TB
    ;
TB  :  TB AND FB
    |  FB
    ;
FB  :  FB EQ M
    |  M
    ;
M   :  M ORDER E
    |  E
    ;
E   :  E ADDSUB T
    |  T
    ;
T   :  T DIVSTAR F
    |  F
    ;
F   :  ADDSUB F
    |  '!' F
    |  '&' IDENT
    |  '(' Exp ')'
    |  NUM
    |  CHARACTER
    |  LValue
    |  IDENT '(' Arguments  ')'
    ;
LValue:
       IDENT
    |  IDENT '.' IDENT
    ;
Arguments:
       ListExp
    |  %empty
    ;
ListExp:
       ListExp ',' Exp
    |  Exp
    ;

%%

int main(int argc, char** argv) {
    int rtr;
    initSymbolTable();
    rtr = yyparse();
    printSymbols();
	return rtr;
}

void yyerror(const char *s){
	fprintf(stderr, "%s near line %d character %d\n", s, lineno,charno);
    
}

Node * makeIdentifier(char * ident_name){
    Node * ident = makeNode(Identifier);
    strcpy(ident->u.identifier, ident_name);
    return ident;
} 

Node * makeSimpleType(char * type_name){
    Node * type = makeNode(Type); 
    strcpy(type->u.type, type_name);
    return type;
}
