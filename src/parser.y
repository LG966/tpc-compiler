%{
/* parser.y */
/* Syntaxe du TPC pour le projet d'analyse syntaxique de 2020-2021 */
  #include <stdio.h>
  #include "abstract-tree.h"
  #include <string.h>
  #include "symbolTable.h"
  #include "operator.h"
  extern int lineno;
  extern int charno;
  extern char text_line[100];
  int yylex();
  void yyerror(const char *);
  Node * makeIdentifier(char * ident_name);
  Node * makeSimpleType(char * type_name);
%}



%union{
    struct Node * node;
    int num;
    char character;
    char string[64];
}

%token <character> CHARACTER
%token <num> NUM
%token <string> IDENT
%token <string> SIMPLETYPE
%token <node> ORDER EQ
%token <node> ADDSUB
%token <node> DIVSTAR OR AND
%token <string> STRUCT IF WHILE RETURN VOID PRINT READC READE
%precedence ')'
%precedence ELSE

%type <node> DeclVars Declarateurs Type TypesVars EnTeteFonct
%type <node> DeclFoncts DeclFonct Parametres Corps ListTypVar Prog
%type <node> DeclChamps SuiteInstr Instr 
%type <node> F T E M TB FB Exp LValue ListExp Arguments

%%

Prog:  TypesVars DeclFoncts     {
                                    $$ = makeNode(Program);
                                    addChild($$, $1);
                                    addChild($$, $2);
                                    puts("Global Symbol Table");
                                    printglobalST();
                                    printTree($$);
                                }
    ;
TypesVars:
       TypesVars Type Declarateurs ';'      {
                                                $$ = $1;
                                                addChild($$, $2);
                                                addChild($2, $3);
                                                for(Node * ident = FIRSTCHILD($3); ident != NULL; ident = SIBLING(ident))
                                                {
                                                    /* printf("ident :%s - type :%s\n", ident->u.identifier, $2->u.type); */
                                                    addglobalVar(ident->u.identifier, $2->u.type);
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
                                    printf("func %s Symbol Table\n", SECONDCHILD($1)->u.identifier);
                                    printfuncST();
                                    emptyfuncST();
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
                                        addChild($$, $3);
                                       

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
                                                addfuncVar(ident->u.identifier, $2->u.type);
                                            }
                                        }
    |  %empty                           {
                                            $$ = makeNode(DeclVars);
                                        }
    ;
SuiteInstr:
       SuiteInstr Instr                   {
                                              $$ = $1;
                                              addChild($$, $2);
                                          }
    |  %empty                             {
                                              $$ = makeNode(SuiteInstr);
                                          }
    ;
Instr:
           LValue '=' Exp ';'             {
                                              $$ = makeNode(Assignement);
                                              $$->u.character = '=';
                                              addChild($$, $1);
                                              addSibling($1, $3);
                                          }
    |  READE '(' LValue ')' ';'           {
                                              $$ = makeNode(Func);
                                              strcpy($$->u.identifier, $1);
                                              addChild($$, $3);
                                          }

    |  READC '(' LValue ')' ';'           {
                                              $$ = makeNode(Func);
                                              strcpy($$->u.identifier, $1);
                                              addChild($$, $3);
                                          }
    |  PRINT '(' Exp ')' ';'              {
                                              $$ = makeNode(Func);
                                              strcpy($$->u.identifier, $1);
                                              addChild($$, $3);
                                          }
    |  IF '(' Exp ')' Instr               {
                                              $$ = makeNode(LoneIf);
                                              addChild($$, $3);
                                              addChild($$, $5);
                                          }
    |  IF '(' Exp ')' Instr ELSE Instr    {
                                              $$ = makeNode(IfElse);
                                              addChild($$, $3); //condition
                                              addChild($$, $5); // instr if true
                                              addChild($$, $7); // else instr
                                          }
    |  WHILE '(' Exp ')' Instr            {
                                              $$ = makeNode(While);
                                              addChild($$, $3);
                                              addChild($$, $5);
                                          }
    |  Exp ';'                            {
                                              $$ = $1;
                                          }
    |  RETURN Exp ';'                     {
                                              $$ = makeNode(Return);
                                              addChild($$, $2);
                                          }
    |  RETURN ';'                         {
                                              $$ = makeNode(Return);
                                              addChild($$, makeNode(Void));
                                          }
    |  '{' SuiteInstr '}'                 {
                                              $$ = $2;
                                          }
    |  ';'                                {
                                              $$ = makeNode(Void);
                                          }
    ;
Exp :  Exp OR TB                          {
                                              $$ = $2;
                                              addChild($$, $1);
                                              addSibling($1, $3);
                                          }
    |  TB                                 {
                                              $$ = $1;
                                          }
    ;
TB  :  TB AND FB                          {
                                              $$ = $2;
                                              addChild($$, $1);
                                              addSibling($1, $3);
                                          }
    |  FB                                 {
                                              $$ = $1;
                                          }
    ;
FB  :  FB EQ M                            {
                                              $$ = $2;
                                              addChild($$, $1);
                                              addSibling($1, $3);
                                          }
    |  M                                  {
                                              $$ = $1;
                                          }
    ;
M   :  M ORDER E                          {
                                              $$ = $2;
                                              addChild($$, $1);
                                              addSibling($1, $3);
                                          }
    |  E                                  {
                                              $$ = $1;
                                          }
    ;
E   :  E ADDSUB T                         {
                                              $$ = $2;
                                              addChild($$, $1);
                                              addSibling($1, $3);
                                         }
    |  T                                 {
                                              $$ = $1;
                                         }
    ;
T   :  T DIVSTAR F                       {
                                              $$ = $2;
                                              addChild($$, $1);
                                              addSibling($1, $3);
                                         }
    |  F                                 {
                                              $$ = $1;
                                         }
    ;
F   :  ADDSUB F                          {
                                              $$ = $1;
                                              $$->kind = UnaryOperator; // Binary par défaut, on doit changer
                                              addChild($$, $2);
                                          }
    |  '!' F                              {
                                              $$ = makeNode(UnaryOperator);
                                              $$->u.operator = Not;
                                              addChild($$, $2);
                                          }
    |  '&' IDENT                          {
                                              $$ = makeNode(UnaryOperator);
                                              $$->u.operator = Addressof;
                                              Node *ident = makeIdentifier($2);
                                              addChild($$, ident);
                                          }
    |  '(' Exp ')'                        {
                                              $$ = $2;
                                          }
    |  NUM                                {
                                              $$ = makeNode(IntLiteral);
                                              $$->u.integer = $1;
                                          }
    |  CHARACTER                          {
                                              $$ = makeNode(CharLiteral);
                                              $$->u.character = $1;
                                          }
    |  LValue                             {
                                              $$ = $1;
                                          }
    |  IDENT '(' Arguments  ')'           {
                                              $$ = makeNode(Func);
                                              strcpy($$->u.identifier, $1);
                                              addChild($$, $3);
                                          }
    ;
LValue:
       IDENT                              {
                                              $$ = makeIdentifier($1);
                                          }
    |  IDENT '.' IDENT                    {
                                              $$ = makeNode(StructAccess);
                                              $$->u.character = '.';
                                              Node *ident1 = makeIdentifier($1);
                                              Node *ident2 = makeIdentifier($3);
                                              addChild($$, ident1);
                                              addSibling($$, ident2);
                                          }
    ;
Arguments:
       ListExp                            {
                                              $$ = makeNode(Arguments);
                                              addChild($$, $1);
                                          }
    |  %empty                             {
                                              $$ = makeNode(Void);
                                          }
    ;
ListExp:
       ListExp ',' Exp                    {
                                              $$ = $1;
                                              addSibling($$, $3);
                                          }
    |  Exp                                {
                                              $$ = $1;
                                          }
    ;

%%

int main(int argc, char** argv) {
    int rtr;
    rtr = yyparse();
	return rtr;
}

void display_error(){
	int index;
	for(index = 0; index < charno - 1; index++){
		if(text_line[index] == '\t')
			printf("\t");
		else
			printf(" ");
	}
	printf("^\n");
}

void yyerror(const char *s) {
	printf("Erreur à la ligne %d colonne %d!\n", lineno, charno);
	printf("%s\n", text_line);
	display_error();
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
