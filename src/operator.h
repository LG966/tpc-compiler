#ifndef __OPERATOR__
#define __OPERATOR__

#include <string.h>
#include <stdio.h>


typedef enum{
    Add,
    Sub,
    Div,
    Mul,
    Mod,
    And,
    Or,
    Inf,
    Infeg,
    Sup,
    Supeg,
    Equal,
    Unequal,
    Not,
    Addressof
}Operator;


const char * getStringFromOperator(Operator op);
Operator getOperatorFromString(char * s);

#endif