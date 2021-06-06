#ifndef __OPTION__
#define __OPTION__

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


char OPTION_TREE();
char OPTION_SYMTABS();


int parse_options(int argc, char** argv);

#endif