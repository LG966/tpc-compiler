%{
  /* lexer.l */
  /* Compatible avec parser.y */
  #include "abstract-tree.h"
  #include "parser.tab.h"
  #include <string.h>
  #include "operator.h"
  #include "type.h"
  int lineno=1;
  int charno=0;
  int index_text = 0;
  char text_line[100];
  
  #define MAKE_BIN_OPE {yylval.node = makeNode(BinaryOperator); \
   yylval.node->u.operator = getOperatorFromString(yytext);} 

  #define MAKE_TYPE(X) {yylval.node = makeNode(Type); \
   yylval.node->u.type = X;}

%}


%option noinput nounput noyywrap
%x SHORT_COMMENT LONG_COMMENT
%%
[ \t\r]+		{charno += yyleng;}
\n			{charno=0; index_text = 0; lineno++;}
"/*"			{ charno += yyleng;BEGIN LONG_COMMENT; }
"//"			{ charno += yyleng;BEGIN SHORT_COMMENT; }
"&&"			{ charno += yyleng; MAKE_BIN_OPE; return AND; }
"||"			{ charno += yyleng; MAKE_BIN_OPE; return OR; }
"+"|-			{ charno += yyleng; MAKE_BIN_OPE; return ADDSUB;}
"*"|"/"|"%"			{ charno += yyleng; MAKE_BIN_OPE; return DIVSTAR; }
"<"|"<="|">"|">="		{ charno += yyleng; MAKE_BIN_OPE; return ORDER; }
==|!=			{ charno += yyleng;  MAKE_BIN_OPE; return EQ; }
int			{ charno += yyleng; MAKE_TYPE(tpc_int); return SIMPLETYPE; }
char		{ charno += yyleng; MAKE_TYPE(tpc_char); return SIMPLETYPE; }
print			{ charno += yyleng;return PRINT; }
readc			{ charno += yyleng;return READC; }
reade			{ charno += yyleng;return READE; }
void			{ charno += yyleng;return VOID; }
struct		{ charno += yyleng;return STRUCT; }
if			{ charno += yyleng; return IF; }
else			{ charno += yyleng;return ELSE; }
while			{ charno += yyleng;return WHILE; }
return			{ charno += yyleng;return RETURN; }
[a-zA-Z_][a-zA-Z0-9_]*	{ charno += yyleng; strcpy(yylval.string, yytext); return IDENT; }
[0-9]+			{ charno += yyleng; sscanf(yytext, "%d", &(yylval.num)); return NUM;}
'\\?.'			{ charno += yyleng; return CHARACTER; }
.			{ charno += yyleng; return yytext[0];}
<LONG_COMMENT>"*/"		{ BEGIN INITIAL; charno += yyleng; }
<LONG_COMMENT,SHORT_COMMENT>.		{charno += yyleng;}
<LONG_COMMENT>\n		{lineno++; index_text = 0; charno=0;}
<SHORT_COMMENT>\n		{BEGIN INITIAL; index_text = 0; lineno++;charno=0;}
%%
