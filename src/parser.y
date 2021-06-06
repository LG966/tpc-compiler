%{
    /* parser.y */
    #include <stdio.h>
    #include "abstract-tree.h"
    #include <string.h>
    #include "symbolTable.h"
    #include "operator.h"
    #include "type.h"
    #include "struct.h"
    #include "gen_code_asm.h"
    #include "func.h"
    #include "option.h"
    #include "semantic.h"

    #define STRUCT_TYPE(NODE) ({int __val = getStructIndex(NODE->u.identifier); \
                            (__val == -1 ? ({ERR_HEADER(NODE); printf("unrecognized struct type '%s'\n", NODE->u.identifier); \
                            exit(EXIT_FAILURE); -1;}) : __val); })

    extern int lineno;
    extern int charno;
    extern char text_line[100];

    int yylex();
    void yyerror(const char *);
    Node * makeIdentifier(char * ident_name);
    Node *racine;
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
%token <node> SIMPLETYPE
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
                                    
                                    racine = $$;
                                    if(OPTION_TREE()){ printTree(racine); }
                                    if(OPTION_SYMTABS()){
                                        printglobalST();
                                        printStructs();
                                        printPrototypes();
                                    }
                                }
    ;
TypesVars:
       TypesVars Type Declarateurs ';'      {
                                                $$ = $1;
                                                addChild($$, $2);
                                                addChild($2, $3);

                                                for(Node * ident = FIRSTCHILD($3); ident != NULL; ident = SIBLING(ident))
                                                {
                                                    if($2->kind == Type)
                                                    {
                                                        switch(addglobalVar_native(ident->u.identifier, $2->u.type)){
                                                            case 1: ERR_HEADER(ident); printf("redifinition of variable '%s'\n", ident->u.identifier); exit(SEMANTIC_ERROR);
                                                            case 2: ERR_HEADER(ident); printf("too many global variables defined (LIMIT : %d)\n", MAXSYMBOLS); exit(DEFAULT_ERROR);
                                                            default: break;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        switch(addglobalVar_struct(ident->u.identifier, STRUCT_TYPE($2))){
                                                            case 1: ERR_HEADER(ident); printf("%d:%d: error: redifinition of variable '%s'\n", lineno, charno, ident->u.identifier); exit(SEMANTIC_ERROR);
                                                            case 2: ERR_HEADER(ident); printf("%d:%d: error: too many global variables defined (LIMIT : %d)\n", lineno, charno, MAXSYMBOLS); exit(DEFAULT_ERROR);
                                                            default: break;
                                                        }
                                                    }
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

                                                            switch(addStructDecl(declstruct)){
                                                                case 1: ERR_HEADER($5); printf("redifinition of struct '%s'\n", $3); exit(SEMANTIC_ERROR);
                                                                case 2: ERR_HEADER($5); printf("too many struct types defined (LIMIT : %d)\n", MAXSTRUCTS); exit(DEFAULT_ERROR);
                                                                case 3: ERR_HEADER($5); printf("redifinition of a member in struct '%s'\n", $3); exit(SEMANTIC_ERROR);
                                                                default: break;
                                                            }

                                                        }
    |  %empty                                           {$$ = makeNode(TypesVars);}
    ;
Type:
       SIMPLETYPE
    |  STRUCT IDENT     {$$ = makeNode(StructType); strcpy($$->u.identifier, $2);}
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
                                                    addChild($$, $2);
                                                    addChild($2, $3);
                                                }
    |  SIMPLETYPE Declarateurs ';'              {
                                                    $$ = makeNode(DeclChamps);
                                                    addChild($$, $1);
                                                    addChild($1, $2);
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

                                    // Table des symboles locales
                                    if(OPTION_SYMTABS()){
                                        printf("\nLocal variables of function '%s' -> ", SECONDCHILD($1)->u.identifier);
                                        printfuncST();
                                    }

                                    //Ajout dans la table des prototypes
                                    switch(addFuncPrototype($$))
                                    {
                                        case 1: ERR_HEADER(SIBLING(FIRSTCHILD($1))); printf("redeclaration of function '%s'\n",SIBLING(FIRSTCHILD($1))->u.identifier); exit(SEMANTIC_ERROR);
                                        case 2: ERR_HEADER(SIBLING(FIRSTCHILD($1))); printf("too many functions declared (LIMIT : %d)\n", MAXFUNC); exit(DEFAULT_ERROR);
                                        case 3: ERR_HEADER(SIBLING(FIRSTCHILD($1))); printf("too many parameters in function '%s' (LIMIT : %d)\n", SIBLING(FIRSTCHILD($1))->u.identifier, MAXPARAMATERS); exit(DEFAULT_ERROR);
                                        default: break;
                                    }

                                    check_SuiteInstr(SIBLING(FIRSTCHILD($2)));
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
    |  ListTypVar   {
                        $$ = $1;
                        for(Node * ident = FIRSTCHILD($$); ident != NULL; ident = SIBLING(ident))
                        {
                            if(ident->kind == Type)
                            {
                                switch(addfuncVar_native(FIRSTCHILD(ident)->u.identifier, ident->u.type)){
                                    case 1: ERR_HEADER(FIRSTCHILD(ident)); printf("redifinition of variable '%s'\n", FIRSTCHILD(ident)->u.identifier); exit(SEMANTIC_ERROR);
                                    case 2: ERR_HEADER(FIRSTCHILD(ident)); printf("too many local variables defined (LIMIT : %d)\n", MAXSYMBOLS); exit(DEFAULT_ERROR);
                                    /* case 3: ERR_HEADER(FIRSTCHILD(ident)); printf("redifinition of global variable '%s'\n", FIRSTCHILD(ident)->u.identifier); exit(SEMANTIC_ERROR); */
                                    default: break;
                                }
                            }
                            else
                            {
                                switch(addfuncVar_struct(FIRSTCHILD(ident)->u.identifier, STRUCT_TYPE(ident))){
                                    case 1: ERR_HEADER(FIRSTCHILD(ident)); printf("redifinition of variable '%s'\n", FIRSTCHILD(ident)->u.identifier); exit(SEMANTIC_ERROR);
                                    case 2: ERR_HEADER(FIRSTCHILD(ident)); printf("too many local variables defined (LIMIT : %d)\n", MAXSYMBOLS); exit(DEFAULT_ERROR);
                                    default: break;
                                }
                            }

                        }
                    }
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
                                                if($2->kind == Type)
                                                {
                                                    switch(addfuncVar_native(ident->u.identifier, $2->u.type)){
                                                        case 1: ERR_HEADER(ident); printf("redifinition of variable '%s'\n", ident->u.identifier); exit(SEMANTIC_ERROR);
                                                        case 2: ERR_HEADER(ident); printf("too many local variables defined (LIMIT : %d)\n", MAXSYMBOLS); exit(DEFAULT_ERROR);
                                                        case 3: ERR_HEADER(ident); printf("redifinition of global variable '%s'\n", ident->u.identifier); exit(SEMANTIC_ERROR);
                                                        default: break;
                                                    }
                                                }
                                                else
                                                {
                                                    switch(addfuncVar_struct(ident->u.identifier, STRUCT_TYPE($2))){
                                                        case 1: ERR_HEADER(ident); printf("redifinition of variable '%s'\n", ident->u.identifier); exit(SEMANTIC_ERROR);
                                                        case 2: ERR_HEADER(ident); printf("too many local variables defined (LIMIT : %d)\n", MAXSYMBOLS); exit(DEFAULT_ERROR);
                                                        default: break;
                                                    }
                                                }

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
                                              $$ = makeNode(Reade);
                                              strcpy($$->u.identifier, $1);
                                              addChild($$, $3);
                                          }

    |  READC '(' LValue ')' ';'           {
                                              $$ = makeNode(Readc);
                                              strcpy($$->u.identifier, $1);
                                              addChild($$, $3);
                                          }
    |  PRINT '(' Exp ')' ';'              {
                                              $$ = makeNode(Print);
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
                                              addSibling(ident1, ident2);
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
    FILE *file;

    if(!parse_options(argc, argv))
    {
        return 0;
    }

    rtr = yyparse();

    if(NULL == (file = fopen("./_anonymous.asm", "w")))
    {
        printf("%s: error: unable to create .asm file\n", __FILE__);
        return DEFAULT_ERROR;        
    }

    generate(file, racine);

    fclose(file);
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