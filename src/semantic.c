#include "semantic.h"

#define SET_VOID (nat = tpc_void);
#define SET_INT (nat = tpc_int);
#define SET_CHAR (nat = tpc_char);
#define MUST_NATIVE(X) if(check_Exp(X) != native){ERR_HEADER(X); printf("unexpected expression of type struct\n"); exit(SEMANTIC_ERROR);}
#define MUST_NOT_VOID(X) if(nat == tpc_void){ERR_HEADER(X); printf("unexpected expression of type void\n"); exit(SEMANTIC_ERROR);}

static native_t nat;            //native type to check after check_exp returns native
static unsigned char str;       //struct type to check after check_exp returns struc



static type_kind check_Exp(Node * exp){
    int tmp, i;
    fun_prototype * fun;
    type_kind tk;

    switch (exp->kind)
    {
    case IntLiteral:
        SET_INT
        return native;
    case CharLiteral:
        SET_CHAR
        return native;

    case UnaryOperator:
        MUST_NATIVE(FIRSTCHILD(exp));
        MUST_NOT_VOID(FIRSTCHILD(exp));
        SET_INT
        return native;

    case BinaryOperator:
        MUST_NATIVE(FIRSTCHILD(exp));
        MUST_NOT_VOID(FIRSTCHILD(exp));
        MUST_NATIVE(SIBLING(FIRSTCHILD(exp)));
        MUST_NOT_VOID(SIBLING(FIRSTCHILD(exp)));
        SET_INT
        return native;
    
    case Func: // Function call
        if((tmp = getIndexFromFunName(exp->u.identifier)) == -1){
            ERR_HEADER(exp); printf("unrecognized symbol '%s' in function call\n", exp->u.identifier);
            exit(SEMANTIC_ERROR);
        }
        fun = &(prototypes[tmp]);
        //Arguments
        i = 0;

        for (Node * arg = FIRSTCHILD(FIRSTCHILD(exp)); arg != NULL; i++, arg = SIBLING(arg))
        {
            if (i >= fun->param_len) // too many args
            {
                ERR_HEADER(exp); printf("too many arguments in function call '%s'\n", exp->u.identifier);
                exit(SEMANTIC_ERROR);
            }
            tk = check_Exp(arg);
            if (tk != fun->parameters[i].parameter_type_kind) // different type kind
            {
                ERR_HEADER(arg); printf("argument %d in function call '%s' has incorrect type\n", i, exp->u.identifier);
                exit(SEMANTIC_ERROR);
            }
            if (tk == native)
            {
                MUST_NOT_VOID(arg);
                if (nat == tpc_int && fun->parameters[i].parameter_type.native == tpc_char){
                    WAR_HEADER(arg);
                    printf("argument %d in function call '%s' has type int but an expression of type char was expected\n", i, exp->u.identifier);
                }
            }else{ //struct
                if (fun->parameters[i].parameter_type.struc != str){
                    ERR_HEADER(arg); printf("argument %d in function call '%s' has incorrect struct type\n", i, exp->u.identifier);
                    exit(SEMANTIC_ERROR);
                }
            }
        }
        // not enough arguments provided
        if (i < fun->param_len)
        {
            ERR_HEADER(exp); printf("function '%s' expected %d arguments, got %d instead\n", exp->u.identifier, fun->param_len, i);
            exit(SEMANTIC_ERROR);
        }
        
        //return type
        if (fun->return_type_kind == native)
        {
            nat = fun->return_type.native;
            return native;
        }else{
            str = fun->return_type.struc;
            return struc;
        }
        
        case Identifier:
            if ((tmp = getSymbolIndexFromVarNameLocal(exp->u.identifier)) == -1)
            {
                if ((tmp = getSymbolIndexFromVarNameGlobal(exp->u.identifier)) == -1)
                {
                    ERR_HEADER(exp); printf("Unrecognized symbol %s\n", exp->u.identifier);
                    exit(SEMANTIC_ERROR);
                }
                else{ //is a global variable
                    return (globalST[tmp].kind == native 
                        ? ({nat = globalST[tmp].type.native; native; })
                        : ({str = globalST[tmp].type.struc; struc; }));
                }
                
            }else{ //is a local variable
                return (funcST[tmp].kind == native 
                    ? ({nat = funcST[tmp].type.native; native; })
                    : ({str = funcST[tmp].type.struc; struc; }));
            }
        case StructAccess:
            if ((tmp = getSymbolIndexFromVarNameLocal(FIRSTCHILD(exp)->u.identifier)) == -1)
            {
                if ((tmp = getSymbolIndexFromVarNameGlobal(exp->u.identifier)) == -1)
                {
                    ERR_HEADER(exp); printf("unrecognized symbol %s\n", FIRSTCHILD(exp)->u.identifier);
                    exit(SEMANTIC_ERROR);
                }
                else{ //is a global variable
                    if (globalST[tmp].kind != struc) //trying to access something not a structure
                    {
                        ERR_HEADER(exp); printf("request for member '%s' in something not a structure\n", SIBLING(FIRSTCHILD(exp))->u.identifier);
                        exit(SEMANTIC_ERROR);
                    }
                    i = globalST[tmp].type.struc;
                    tk = getMemberType(&structs[i], SIBLING(FIRSTCHILD(exp))->u.identifier);
                    if (tk == -1)
                    {
                        ERR_HEADER(exp); printf("'struct %s' has no member '%s'\n", FIRSTCHILD(exp)->u.identifier, SIBLING(FIRSTCHILD(exp))->u.identifier);
                        exit(SEMANTIC_ERROR);
                    }else{
                        nat = tk;
                        return native;
                    }
                }
                
            }else{ //is a local variable
                if (funcST[tmp].kind != struc) //trying to access something not a structure
                {
                    ERR_HEADER(exp); printf("request for member '%s' in something not a structure\n", SIBLING(FIRSTCHILD(exp))->u.identifier);
                    exit(SEMANTIC_ERROR);
                }
                i = funcST[tmp].type.struc;
                tk = getMemberType(&structs[i], SIBLING(FIRSTCHILD(exp))->u.identifier);
                if (tk == -1)
                {
                    ERR_HEADER(exp); printf("'struct %s' has no member '%s'\n", FIRSTCHILD(exp)->u.identifier, SIBLING(FIRSTCHILD(exp))->u.identifier);
                    exit(SEMANTIC_ERROR);
                }else{
                    nat = tk;
                    return native;
                }
            }
        default: assert(!"WTF."); return native;
    }
}

static void check_assignement(Node * ass){
    assert(ass->kind == Assignement);
    native_t t1;
    unsigned char s1;

    if (check_Exp(FIRSTCHILD(ass)) == native) //leftvalue has native type
    {
        MUST_NOT_VOID(FIRSTCHILD(ass));
        t1 = nat;

        if (check_Exp(SIBLING(FIRSTCHILD(ass))) != native)
        {
            ERR_HEADER(ass); printf("incompatible types when assigning to type '%s' from type 'struct %s' \n", getCharFromNativeType(t1), getStructNameFromIndex(str));
            exit(SEMANTIC_ERROR);
        }else{ 
            MUST_NOT_VOID(SIBLING(FIRSTCHILD(ass)));
            if (nat == tpc_int && t1 == tpc_char)
            {
                WAR_HEADER(ass); printf("Assigning 'int' value to 'char' variable\n");
            }
        }
        
    }else{ //leftvalue has struct type
        s1 = str;
        if (check_Exp(SIBLING(FIRSTCHILD(ass))) != struc)
        {
            ERR_HEADER(ass); printf("incompatible types when assigning to type 'struct %s' from type '%s' \n", getStructNameFromIndex(s1), getCharFromNativeType(nat));
            exit(SEMANTIC_ERROR);
        }
        if (s1 != str)
        {
            ERR_HEADER(ass);
            printf("incompatible types when assigning to type 'struct %s' from type 'struct %s' \n", getStructNameFromIndex(s1), getStructNameFromIndex(str));
            exit(SEMANTIC_ERROR);
        }
    }
}

static void check_Reade(Node * re){
    assert(re->kind == Reade);

    if(check_Exp(FIRSTCHILD(re)) == struc)
    {
        ERR_HEADER(FIRSTCHILD(re));
        printf("reade expects variable of type 'int' but argument is of type 'struct %s'\n", getStructNameFromIndex(str));
        exit(SEMANTIC_ERROR);
    }
    MUST_NOT_VOID(FIRSTCHILD(re));
    if (nat == tpc_char)
    {
        WAR_HEADER(FIRSTCHILD(re)); printf("reade expects variable of type 'int' but argument is of type 'char'\n");
    }
}

static void check_Readc(Node * rc){
    assert(rc->kind == Readc);

    if(check_Exp(FIRSTCHILD(rc)) == struc)
    {
        ERR_HEADER(FIRSTCHILD(rc));
        printf("readc expects variable of type 'char' but argument is of type 'struct %s'\n", getStructNameFromIndex(str));
        exit(SEMANTIC_ERROR);
    }
    MUST_NOT_VOID(FIRSTCHILD(rc));
}

static void check_Print(Node * p){
    assert(p->kind == Print);

    if(check_Exp(FIRSTCHILD(p)) == struc)
    {
        ERR_HEADER(FIRSTCHILD(p));
        printf("print expects expression of type 'char' or 'int' but argument is of type 'struct %s'\n", getStructNameFromIndex(str));
        exit(SEMANTIC_ERROR);
    }
    MUST_NOT_VOID(FIRSTCHILD(p));
}

static void check_IfWhile(Node * iw){
    assert(iw->kind == LoneIf || iw->kind == While || iw->kind == IfElse);
    if(check_Exp(FIRSTCHILD(iw)) == struc)
    {
        ERR_HEADER(FIRSTCHILD(iw));
        printf("If/While condition expects expression of type 'char' or 'int' but expression is of type 'struct %s'\n", getStructNameFromIndex(str));
        exit(SEMANTIC_ERROR);
    }
    MUST_NOT_VOID(FIRSTCHILD(iw));
}

static void check_Return(Node * r){
    assert(r->kind == Return);
    fun_prototype * pro = &(prototypes[prototypesSize-1]); //get current prototype function

    if (FIRSTCHILD(r)->kind == Void) // 'return;' case
    {
        if (pro->return_type_kind == native && pro->return_type.native != tpc_void)
        {
            ERR_HEADER(r);
            printf("'return' with no value, in function returning non-void : '%s'\n", getCharFromNativeType(pro->return_type.native));
            exit(SEMANTIC_ERROR);
        }
        
    }else{
        if (check_Exp(FIRSTCHILD(r)) == native) // return has native type
        {
            if (pro->return_type_kind != native)
            {
                ERR_HEADER(r);
                printf("function has return type 'struct %s' but return expression has type '%s'\n", getStructNameFromIndex(pro->return_type.struc), getCharFromNativeType(nat));
                exit(SEMANTIC_ERROR);
            }
            if (pro->return_type.native == tpc_char && nat == tpc_int)
            {
                WAR_HEADER(r);
                printf("function has return type char but return expression has type char\n");
            } 
            else if (pro->return_type.native == tpc_void && nat != tpc_void)
            {
                ERR_HEADER(r);
                printf("function has return type 'void' but return expression has type '%s'\n", getCharFromNativeType(nat));
                exit(SEMANTIC_ERROR);
            }
        }else{ // return has struct type
        printf("ok98749687498674986748\n");
            if (pro->return_type_kind != struc)
            {
                ERR_HEADER(r);
                printf("function has return type '%s' but return expression has type 'struct %s'\n", getCharFromNativeType(pro->return_type.native), getStructNameFromIndex(nat));
                exit(SEMANTIC_ERROR);
            }
            if (pro->return_type.struc != str)
            {
                ERR_HEADER(r);
                printf("function has return type 'struct %s' but return expression has type 'struct %s'\n", getStructNameFromIndex(pro->return_type.struc), getStructNameFromIndex(str));
                exit(SEMANTIC_ERROR);
            }
        }
    }
}

void check_SuiteInstr(Node * si){
    assert(si->kind == SuiteInstr);


    for (Node * i = FIRSTCHILD(si); si != NULL; si = SIBLING(si))
    {
        switch (i->kind)
        {
        case Assignement: check_assignement(i); return;
        case Reade: check_Reade(i); return;
        case Readc: check_Readc(i); return;
        case Print: check_Print(i); return;
        case While:
        case LoneIf:
        case IfElse: check_IfWhile(i); return;
        case Return: check_Return(i); return;
        default: break;
        }
    }
    
}