#include "operator.h"

static const char *StringFromOperator[] = {
  "+",
  "-",
  "/",
  "*",
  "%",
  "&&",
  "||",
  "<",
  "<=",
  ">",
  ">=",
  "==",
  "!=",
  "!",
  "&"
};

static int n_operator = (sizeof(StringFromOperator) / sizeof(char *));

const char * getStringFromOperator(Operator op){
    return StringFromOperator[op];
}

Operator getOperatorFromString(char * s){
    int i;

    for (i = 0; i < n_operator; i++)
    {
        if (strcmp(StringFromOperator[i], s) == 0){
            return i;
        }
    }
    return -1;
}
