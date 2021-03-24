%{
  /* lexer.l */
  /* Compatible avec parser.y */
  #define MAXNAME 30
  #include "abstract-tree.h"
  #include "parser.tab.h"
  #include <string.h>
  int lineno=1;
  int charno=0;
%}


%option noinput nounput noyywrap
%x SHORT_COMMENT LONG_COMMENT
%%
[ \t\r]+		{charno += yyleng;}
\n			{charno=0; lineno++;}
"/*"			{ charno += yyleng;BEGIN LONG_COMMENT; }
"//"			{ charno += yyleng;BEGIN SHORT_COMMENT; }
&&			{ charno += yyleng;return AND; }
"||"			{ charno += yyleng;return OR; }
"+"|-			{ yylval.character=yytext[0]; charno += yyleng; return ADDSUB; }
"*"|"/"|"%"			{ yylval.character=yytext[0]; charno += yyleng;return DIVSTAR; }
"<"|"<="|">"|>=		{ charno += yyleng;return ORDER; }
==|!=			{ charno += yyleng;return EQ; }
int			{ charno += yyleng; strcpy(yylval.string, yytext); return SIMPLETYPE;}
char		{ charno += yyleng; strcpy(yylval.string, yytext); return SIMPLETYPE;}
print			{ charno += yyleng;return PRINT; }
readc			{ charno += yyleng;return READC; }
reade			{ charno += yyleng;return READE; }
void			{ charno += yyleng;return VOID; }
struct		{ charno += yyleng;return STRUCT; }
if			{ charno += yyleng;return IF; }
else			{ charno += yyleng;return ELSE; }
while			{ charno += yyleng;return WHILE; }
return			{ charno += yyleng;return RETURN; }
[a-zA-Z_][a-zA-Z0-9_]*	{ charno += yyleng; strcpy(yylval.string, yytext); return IDENT; }
[0-9]+			{ charno += yyleng; sscanf(yytext, "%d", &(yylval.num)); return NUM;}
'\\?.'			{ charno += yyleng; return CHARACTER; }
.			{ charno += yyleng; return yytext[0];}
<LONG_COMMENT>"*/"		{ BEGIN INITIAL; charno += yyleng; }
<LONG_COMMENT,SHORT_COMMENT>.		{charno += yyleng;}
<LONG_COMMENT>\n		{lineno++;charno=0;}
<SHORT_COMMENT>\n		{BEGIN INITIAL; lineno++;charno=0;}
%%
