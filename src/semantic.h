#ifndef __SEMANTIC__
#define __SEMANTIC__

#include "abstract-tree.h"
#include "type.h"
#include "struct.h"
#include "operator.h"
#include "symbolTable.h"
#include "func.h"
#include <assert.h>


#define ERR_HEADER(NODE) (printf("%s:%d:%d: error: ", "tpcc", NODE->lineno, NODE->charno))
#define WAR_HEADER(NODE) (printf("%s:%d:%d: warning: ", "tpcc", NODE->lineno, NODE->charno))

#define SEMANTIC_ERROR 2
#define DEFAULT_ERROR 3 

void check_SuiteInstr(Node * si);












#endif