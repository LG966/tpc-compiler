%{
  /* lexer.l */
  /* Compatible avec parser.y */
  #define MAXNAME 30
  #include "abstract-tree.h"
  #include "parser.tab.h"
  #include <string.h>
  #include "operator.h"
  int lineno=1;
  int charno=0;
  int index_text = 0;
  char text_line[100];
  #define YY_INPUT(buf, result, max_size){ \
		char c = fgetc(yyin); \
		result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
		/* Vérifie si la ligne a déjà été enregistré */ \
		if(index_text == 0){ \
			while(c != EOF && c != '\n'){ \
				text_line[index_text] = c; \
				index_text++; \
				c = fgetc(yyin); \
			} \
			text_line[index_text] = '\0'; \
			fseek(yyin, -index_text, SEEK_CUR); \
		} \
	}
%}


%option noinput nounput noyywrap
%x SHORT_COMMENT LONG_COMMENT
%%
[ \t\r]+		{charno += yyleng;}
\n			{charno=0; index_text = 0; lineno++;}
"/*"			{ charno += yyleng;BEGIN LONG_COMMENT; }
"//"			{ charno += yyleng;BEGIN SHORT_COMMENT; }
"&&"			{ charno += yyleng; strcpy(yylval.string, yytext); return AND; }
"||"			{ charno += yyleng; strcpy(yylval.string, yytext); return OR; }
"+"|-			{ yylval.character=yytext[0]; charno += yyleng; return ADDSUB; }
"*"|"/"|"%"			{ yylval.character=yytext[0]; charno += yyleng;return DIVSTAR; }
"<"|"<="|">"|">="		{ charno += yyleng; strcpy(yylval.string, yytext); return ORDER; }
==|!=			{ charno += yyleng; strcpy(yylval.string, yytext); return EQ; }
int			{ charno += yyleng; strcpy(yylval.string, yytext); return SIMPLETYPE;}
char		{ charno += yyleng; strcpy(yylval.string, yytext); return SIMPLETYPE;}
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
