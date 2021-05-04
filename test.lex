%option nounput noinput
%option noyywrap
%{
	/*
	#define LT 1
	#define LE 2
	#define EQ 3
	#define NE 4
	#define GT 5
	#define GE 6
	#define IF 7
	#define ELSE 9
	#define ID 10
	#define NUMBER 11
	#define RELOP 12
	#define INT 14
	#define FLOAT 15
	#define ELEM 16
	#define SET 17
	#define OPAR 18
	#define CPAR 19
	#define OBRA 20
	#define CBRA 21
	#define MAIN 22
	*/
	#include "test.tab.h"
	int linhaCount = 1;
	int colunaCount = 1;
	int chamaComment;
	
	
	
%}

%x comentario


delim			[ \t]
ws			{delim}+
letter			[a-zA-Z]
digit			[0-9]
id			{letter}({letter}|{digit}|"_")*
integer		-?{digit}+
floating		-?{digit}+\.{digit}+
string			\"(\\.|[^"\\])*\"
char			'[^\r\n]'
negacao 		!
enderecamento 		&
commentsingle		"//".*
linebreak		(\r\n?|\n)



%%

"/*"		{BEGIN(comentario);}
<comentario>"*/" {BEGIN(INITIAL);}
<comentario>[^*\n]+
<comentario>"*"
<comentario>"\n" {linhaCount++;colunaCount = 0;}
		
{linebreak}		{linhaCount++;colunaCount = 0;}
{commentsingle}	{}
{ws}		{colunaCount += yyleng;}
int		{printf("Achei um int na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return INT;}
float		{printf("Achei um float na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return FLOAT;}
elem		{printf("Achei um float na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return ELEM;}
set		{printf("Achei um set na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return SET;}
if		{printf("Achei um if na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return IF;}
else		{printf("Achei um else na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return ELSE;}
for		{printf("Achei um for na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return FOR;}
read		{printf("Achei um read na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return READ;}
write		{printf("Achei um write na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return WRITE;}
writeln	{printf("Achei um writeln na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return WRITELN;}
in		{printf("Achei um in na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return IN;}
is_set		{printf("Achei um is_set na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return IS_SET;}
add		{printf("Achei um add na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return ADD;}
remove		{printf("Achei um remove na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return REMOVE;}
exists		{printf("Achei um exists na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return EXISTS;}
forall		{printf("Achei um forall na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return FORALL;}
return		{printf("Achei um return na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return RETURN;}
void		{printf("Achei um void na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return VOID;}
EMPTY		{printf("Achei um EMPTY na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng; return EMPTY;}
{id}		{printf("Achei o id %s na linha %d, coluna %d!\n",yytext,linhaCount,colunaCount);colunaCount += yyleng; yylval.text = strdup(yytext); return ID;}
{integer}	{printf("Achei o inteiro %s na linha %d, coluna %d!\n",yytext,linhaCount,colunaCount);colunaCount += yyleng; yylval.text = strdup(yytext); return INTEGER;}
{floating}	{printf("Achei o float %s na linha %d, coluna %d!\n",yytext,linhaCount,colunaCount);colunaCount += yyleng; yylval.text = strdup(yytext); return FLOATING;}
{string}	{printf("Achei a string %s na linha %d, coluna %d!\n",yytext,linhaCount,colunaCount);colunaCount += yyleng;  yylval.text = strdup(yytext); return STRING;}
{char}		{printf("Achei o char %s na linha %d, coluna %d!\n",yytext,linhaCount,colunaCount);colunaCount += yyleng; yylval.text = strdup(yytext); return CHAR;}
"("		{printf("Achei um ( na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return OPENPAR;}
")"		{printf("Achei um ) na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return CLOSEPAR;}
"{"		{printf("Achei um { na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return OPENCURLY;}
"}"		{printf("Achei um } na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return CLOSECURLY;}
"["		{printf("Achei um [ na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return OPENBRAC;}
"]"		{printf("Achei um ] na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return CLOSEBRAC;}
"<"		{printf("Achei um < na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return LESS;}
"<="		{printf("Achei um <= na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return LE;}
"=="		{printf("Achei um == na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return EQ;}
"!="		{printf("Achei um != na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return NEQ;}
">"		{printf("Achei um > na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return GREATER;}
">="		{printf("Achei um >= na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return GE;}
"+"		{printf("Achei um + na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return PLUS;}
"-"		{printf("Achei um - na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return MINUS;}
"*"		{printf("Achei um * na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return AST;}
"/"		{printf("Achei um / na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return BS;}
"="		{printf("Achei um = na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return ASSIGN;}
","		{printf("Achei um , na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return COMMA;}
";"		{printf("Achei um ; na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return SEMICOLON;}
"||"		{printf("Achei um || na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return OR;}
"&&"		{printf("Achei um && na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return AND;}
{negacao}	{printf("Achei uma negacao na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return NOT;}
"&"		{printf("Achei um & na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return AMP;}
"%"		{printf("Achei um %% na linha %d, coluna %d!\n",linhaCount,colunaCount);colunaCount += yyleng;return PCENT;}
.		{printf("Achei um token ->%s<- que não pertence a linguagem, na linha %d, coluna %d!\n",yytext,linhaCount,colunaCount);colunaCount += yyleng;}

%%



