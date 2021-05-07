/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30705

/* Bison version string.  */
#define YYBISON_VERSION "3.7.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "test.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definicoes.h"
#include "tac.h"

extern FILE *yyin;
extern int linhaCount;
extern int colunaCount;
extern int yylex_destroy();

void yyerror(char const *s);
int yylex(void);


//#define ID 1

simbolo* tabelaSimbolos;
simbolo* topo;
no* raiz;
pilha* pilhaEscopo;
int errorCheck;

int escopoCounter;
int funcArgsCounter;


int VerificaTipoArgs(no* alvo, int* vector, int profund){
	int result;
	no* next;

	result = (vector[profund] != (*alvo).tipoVirtual);

	if(!strcmp((*alvo).filhos[1]->nome,"comma")){
		next = (*alvo).filhos[1]->filhos[1];
		return result && VerificaTipoArgs(next,vector,profund+1);
	}
	else {
		return result;
	}
	
}


void PreencheFuncArgs(no* alvo, int num, int* vector){
	if(num == 0) return;
	else if (!strcmp((*alvo).nome,"single")){
		vector[num] = (*alvo).filhos[0]->tipoVirtual;
		return;
	}
	else{
		vector[num] = (*alvo).filhos[0]->tipoVirtual;
		PreencheFuncArgs((*alvo).filhos[1],num + 1,vector);
	}
}

int ContaCallArgs(no* alvo){
	if(!strcmp((*alvo).nome,"mathop")){
		return 1 + ContaCallArgs((*alvo).filhos[1]);
	}
	else{
		if(!strcmp((*alvo).nome,"comma")){
			return ContaCallArgs((*alvo).filhos[1]);
		}
		else{
			return 0;
		}
	}
	return 0;
}

int ContaFuncArgs(no* alvo){
	if(!strcmp((*alvo).nome,"epsilon")) return 0;
	else if (!strcmp((*alvo).nome,"single")) return 1;
	else return 1 + ContaFuncArgs((*alvo).filhos[1]);
	return 0;
}

char* ConverteRetornoTipoString(int entrada){
	char* resultado;
	if(entrada == Int){
		resultado = strdup("Inteiro");
	}
	else if(entrada == Float){
		resultado = strdup("Float");
	}
	else if(entrada == Elem){
		resultado = strdup("Elem");
	}
	else if(entrada == Set){
		resultado = strdup("Set");
	}
	return resultado;
}


int ConverteRetornoTipo(no* entrada){
	char* nome = (*entrada).nome;

	if(!strcmp(nome,"set")){
		return Set;
	}
	else if(!strcmp(nome,"int")){
		return Int;
	}
	else if(!strcmp(nome,"elem")){
		return Elem;
	}
	else if(!strcmp(nome,"void")){
		return Void;
	}
	else{
		return Float;
	}
}


int ConverteTableTipo(int tipo){
	switch(tipo){
		case INT_TABLE:
			return Int;
		break;

		case FLOAT_TABLE:
			return Float;
		break;

		case ELEM_TABLE:
			return Elem;
		break;

		case SET_TABLE:
			return Set;
		break;
	}
	return 0;
}

int DecideConversao(int tipo1, int tipo2, int tipoAlvo){//Precisamos de uma função para decidir se vamos fazer uma conversão de tipos, dados 2 tipos de entrada.
	if((tipo1 == tipo2) && (tipo2 == tipoAlvo)){
		return None;
	}

	if(tipoAlvo == Int){
		if(tipo1 == Int && tipo2 == Float){
			return FloatToIntRight;
		}
		else if(tipo1 == Float && tipo2 == Int){
			return FloatToIntLeft;
		}
		else if(tipo1 == Float && tipo2 == Float){
			return FloatToIntBoth;
		}
		else if(tipo1 == Elem && tipo2 != Elem){
			return ElemToIntLeft;
		}
		else if(tipo1 != Elem && tipo2 == Elem){
			return ElemToIntRight;
		}
		else if (tipo1 == Elem && tipo2 == Elem){
			return ElemToIntBoth;
		}
	}
	else if(tipoAlvo == Float){
		if(tipo1 == Int && tipo2 == Float){
			return IntToFloatLeft;
		}
		else if(tipo1 == Float && tipo2 == Int){
			return IntToFloatRight;
		}
		else if(tipo1 == Int && tipo2 == Int){
			return IntToFloatBoth;
		}
		else if(tipo1 == Elem && tipo2 != Elem){
			return ElemToFloatLeft;
		}
		else if(tipo1 != Elem && tipo2 == Elem){
			return ElemToFloatRight;
		}
		else if((tipo1 == tipo2) && (tipo2 == Elem)){
			return ElemToFloatBoth;
		}
	}
	else if(tipoAlvo == Elem){
		return None;
	}
	
	printf("ERRO SEMANTICO! NAO CONSEGUI DETERMINAR COMO CONVERTER! Linha: %d, Coluna: %d\n",linhaCount,colunaCount);
	errorCheck = TRUE;
	return 99;

}


int DecideTipo(int tipo1, int tipo2){ //Função pra decidir qual deve ser o tipo de uma operação que use os dois tipos de entrada
	if(tipo1 == tipo2){ //Se já são do mesmo tipo, use esse tipo mesmo
		return tipo1;
	}
	else if((tipo1 == Float && tipo2 == Int) || (tipo1 == Int && tipo2 == Float)){ //Se um for Int e o outro Float, a operação deve retornar um Float
		return Float;
	}
	else if(tipo1 == Elem && tipo2 != Elem){//Se um deles for Elem, o resultado deve ser o tipo do outro
		return tipo2;
	}
	else {			
		return tipo1;
	}
}
	
simbolo* CriarSimbolo(char* nome, int tipo, char* valor, int escopo){
	simbolo *ancora = (simbolo*)malloc(sizeof(simbolo));
	if(tabelaSimbolos == NULL){
		tabelaSimbolos = topo = ancora;
		(*ancora).anterior = NULL;
	}
	else{
		(*topo).seguinte = ancora;
		(*ancora).anterior = topo;
	}
	(*ancora).seguinte = NULL;
	(*ancora).tipo = tipo;
	if(valor != NULL){
		(*ancora).valor = strdup(valor);
		(*ancora).tamanhoValor = strlen(valor);
	}
	else{
		(*ancora).valor = NULL;
		(*ancora).tamanhoValor = 0;
	} 
	(*ancora).nome = strdup(nome);
	(*ancora).escopo = escopo;
	(*ancora).returnType = Untyped;
	(*ancora).funcArgsTypes = NULL;
	(*ancora).numArgs = 0;
	topo = ancora;
	return ancora;
}

simbolo* CriarSimboloFuncao(char* nome, int tipo, char* valor, int escopo, int retorno){
	simbolo* ancora = CriarSimbolo(nome,tipo,valor,escopo);
	(*ancora).returnType = retorno;
	return ancora;
}
	
simbolo* RemoverSimbolo(simbolo* alvo){//retorna ponteiro para o próximo simbolo caso esteja removendo de algum lugar que não seja o fim da tabela
	simbolo *ancora = (*alvo).seguinte;
	free((*alvo).nome);
	free((*alvo).valor);
	free(alvo);
	return ancora;
}

void ApagarTabela(){
	simbolo *ancora = tabelaSimbolos;
	if(ancora == NULL) return;
	do{
		ancora = RemoverSimbolo(ancora);
	}
	while(ancora != NULL);
}

simbolo* ProcurarTabelaEscopo(char* alvo, int escopo){//Procura na tabela por um ID no escopo dado com o nome alvo. Usado pra verificar redeclaracao
	simbolo *ancora = tabelaSimbolos; 					//

	while(ancora != NULL){
		if(!strcmp((*ancora).nome,alvo) && (*ancora).escopo == escopo){
			return ancora;
		}
		ancora = (*ancora).seguinte;
	}
	return NULL;
}
	
simbolo* ProcurarTabela(char* alvo){
	simbolo *ancora = tabelaSimbolos;
//	printf("\n\nOI %s OI \n\n",alvo);
	while(ancora != NULL){
		if(!strcmp((*ancora).nome,alvo)){
			return ancora;
		}
		ancora = (*ancora).seguinte;
	}
	return NULL;
}

void EscreverTabela(){
	printf("-----------TABELA DE SIMBOLOS----------\n");
	printf("|-------------------------------------|\n");
	simbolo *ancora = tabelaSimbolos;
	char* ancoraString;
	while(ancora != NULL){
		printf("%s|",(*ancora).nome);
		if((*ancora).tipo == INT_TABLE){
			printf("variavel de numero inteiro|Escopo: %d\n",(*ancora).escopo);
		}
		else if((*ancora).tipo == FLOAT_TABLE){
			printf("variavel de numero float|Escopo: %d\n",(*ancora).escopo);
		}
		else if((*ancora).tipo == FUNC_TABLE){
			ancoraString = ConverteRetornoTipoString((*ancora).returnType);
			printf("funcao|Tipo de retorno: %s|Escopo: %d\n",ancoraString,(*ancora).escopo);
			free(ancoraString);
		}
		else if((*ancora).tipo == SET_TABLE){
			printf("variavel de set|Escopo: %d\n",(*ancora).escopo);
		}
		else if((*ancora).tipo == ELEM_TABLE){
			printf("variavel polimorfica|Escopo: %d\n",(*ancora).escopo);
		}
		else{
			printf("Constante\n");
		}

		ancora = (*ancora).seguinte;
	}
	printf("\n");
}

void ApagarNo(no* argumento){
	free((*argumento).nome);
	free((*argumento).valor);
	free(argumento);
}

void EscreverArvore(no* argumento,int profund){
	int i,j;
	if(argumento == NULL) return;
	for(j = 0;j < profund;j++){
		printf("-");
	}
	printf(">%s",(*argumento).nome);
	if((*argumento).conversion != None){
		switch((*argumento).conversion){
			case IntToFloatLeft:
				printf("|Typecast left arg from Int to Float");
			break;
			case IntToFloatRight:
				printf("|Typecast right arg from Int to Float");
			break;
			case IntToFloatBoth:
				printf("|Typecast both args from Int to Float");
			break;
			case FloatToIntLeft:
				printf("|Typecast left arg from Float to Int");
			break;
			case FloatToIntRight:
				printf("|Typecast right arg from Float to Int");
			break;
			case FloatToIntBoth:
				printf("|Typecast both args from Float to Int");
			break;
			case ElemToIntLeft:
				printf("|Typecast left arg from Elem to Int");
			break;
			case ElemToIntRight:
				printf("|Typecast right arg from Elem to Int");
			break;
			case ElemToFloatLeft:
				printf("|Typecast left arg from Elem to Float");
			break;
			case ElemToFloatRight:
				printf("|Typecast right arg from Elem to Float");
			break;
			case ElemToIntBoth:
				printf("|Typecast both args from Elem to Int");
			break;
			case ElemToFloatBoth:
				printf("|Typecast both args from Elem to Float");
			break;
		}
	}
	printf("\n");
	for(i = 0;i < (*argumento).numFilhos;i++){
		EscreverArvore((*argumento).filhos[i],profund+1);
	}
	
	ApagarNo(argumento);
}

no* ProcurarArvore(int tipo,char* nome, no* base){
	int i;
	no* ancora;
	if(!strcmp((*base).nome,nome) && (*base).tipo == tipo){
		return base;
	}
	else if((*base).numFilhos > 0){
		for(i = 0;i < (*base).numFilhos;i++){
			ancora = ProcurarArvore(tipo,nome,(*base).filhos[i]);
			if(ancora != NULL){
				return ancora;
			}
		}
	}
	return NULL;
}


pilha* Top(pilha* stack){
	pilha* ancora = stack;
	while((*ancora).seguinte != NULL){
		ancora = (*ancora).seguinte;
	}
	return ancora;
}

void Pop(pilha* stack){
	pilha* ancora = stack;
	pilha* ancora2;
	while((*ancora).seguinte != NULL){
		ancora = (*ancora).seguinte;
	}
	ancora2 = (*ancora).anterior;
	(*ancora2).seguinte = NULL;
	free(ancora);
}

void Push(pilha* stack, pilha* argumento){
	pilha* ancora = stack;
	while((*ancora).seguinte != NULL){
		ancora = (*ancora).seguinte;
	}
	(*argumento).anterior = ancora;
	(*ancora).seguinte = argumento;
}

pilha* CriarStack(int valor){
	pilha* ancora = (pilha*)malloc(sizeof(pilha));

	(*ancora).valor = valor;
	(*ancora).seguinte = NULL;
	(*ancora).anterior = NULL;
	return ancora;
}

void LimparStack(pilha* stack){
	pilha* ancora = stack;
	pilha* ancora2;

	while(ancora != NULL){
		ancora2 = ancora;
		ancora = (*ancora).seguinte;
		free(ancora2);
	}
}

simbolo* VerificarEscopo(char* alvo){		//Verifica se o simbolo 'alvo' é acessível dentro da pilha de escopo atual, olhando do topo até sua base, e retornando o ponteiro a tabela se sim.
											//Fazemos isto procurando na tabela de simbolos pela combinação do simbolo alvo e o escopo mais externo ainda não checado(podemos ter o mesmo simbolo em escopos diferentes!)
	pilha* ancora = Top(pilhaEscopo);
	simbolo* ancoraSimbolo;

	while(ancora != NULL){
		ancoraSimbolo = ProcurarTabelaEscopo(alvo,(*ancora).valor);
		if(ancoraSimbolo != NULL){
			return ancoraSimbolo;
		}
		ancora = (*ancora).anterior;
	}
	return NULL;
}


#line 534 "test.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "test.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INTEGER = 3,                    /* INTEGER  */
  YYSYMBOL_FLOATING = 4,                   /* FLOATING  */
  YYSYMBOL_STRING = 5,                     /* STRING  */
  YYSYMBOL_EMPTY = 6,                      /* EMPTY  */
  YYSYMBOL_CHAR = 7,                       /* CHAR  */
  YYSYMBOL_ID = 8,                         /* ID  */
  YYSYMBOL_INT = 9,                        /* INT  */
  YYSYMBOL_FLOAT = 10,                     /* FLOAT  */
  YYSYMBOL_ELEM = 11,                      /* ELEM  */
  YYSYMBOL_SET = 12,                       /* SET  */
  YYSYMBOL_IF = 13,                        /* IF  */
  YYSYMBOL_ELSE = 14,                      /* ELSE  */
  YYSYMBOL_FOR = 15,                       /* FOR  */
  YYSYMBOL_READ = 16,                      /* READ  */
  YYSYMBOL_WRITE = 17,                     /* WRITE  */
  YYSYMBOL_WRITELN = 18,                   /* WRITELN  */
  YYSYMBOL_IN = 19,                        /* IN  */
  YYSYMBOL_IS_SET = 20,                    /* IS_SET  */
  YYSYMBOL_ADD = 21,                       /* ADD  */
  YYSYMBOL_REMOVE = 22,                    /* REMOVE  */
  YYSYMBOL_EXISTS = 23,                    /* EXISTS  */
  YYSYMBOL_FORALL = 24,                    /* FORALL  */
  YYSYMBOL_RETURN = 25,                    /* RETURN  */
  YYSYMBOL_OPENPAR = 26,                   /* OPENPAR  */
  YYSYMBOL_CLOSEPAR = 27,                  /* CLOSEPAR  */
  YYSYMBOL_OPENCURLY = 28,                 /* OPENCURLY  */
  YYSYMBOL_CLOSECURLY = 29,                /* CLOSECURLY  */
  YYSYMBOL_OPENBRAC = 30,                  /* OPENBRAC  */
  YYSYMBOL_CLOSEBRAC = 31,                 /* CLOSEBRAC  */
  YYSYMBOL_LESS = 32,                      /* LESS  */
  YYSYMBOL_LE = 33,                        /* LE  */
  YYSYMBOL_EQ = 34,                        /* EQ  */
  YYSYMBOL_NEQ = 35,                       /* NEQ  */
  YYSYMBOL_GREATER = 36,                   /* GREATER  */
  YYSYMBOL_GE = 37,                        /* GE  */
  YYSYMBOL_PLUS = 38,                      /* PLUS  */
  YYSYMBOL_MINUS = 39,                     /* MINUS  */
  YYSYMBOL_DBS = 40,                       /* DBS  */
  YYSYMBOL_AST = 41,                       /* AST  */
  YYSYMBOL_BS = 42,                        /* BS  */
  YYSYMBOL_ASSIGN = 43,                    /* ASSIGN  */
  YYSYMBOL_COMMA = 44,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 45,                 /* SEMICOLON  */
  YYSYMBOL_OR = 46,                        /* OR  */
  YYSYMBOL_AND = 47,                       /* AND  */
  YYSYMBOL_NOT = 48,                       /* NOT  */
  YYSYMBOL_AMP = 49,                       /* AMP  */
  YYSYMBOL_PCENT = 50,                     /* PCENT  */
  YYSYMBOL_VOID = 51,                      /* VOID  */
  YYSYMBOL_YYACCEPT = 52,                  /* $accept  */
  YYSYMBOL_inicio = 53,                    /* inicio  */
  YYSYMBOL_statement = 54,                 /* statement  */
  YYSYMBOL_55_1 = 55,                      /* $@1  */
  YYSYMBOL_single_line_statement = 56,     /* single_line_statement  */
  YYSYMBOL_comparg = 57,                   /* comparg  */
  YYSYMBOL_comparison = 58,                /* comparison  */
  YYSYMBOL_read = 59,                      /* read  */
  YYSYMBOL_write = 60,                     /* write  */
  YYSYMBOL_writeln = 61,                   /* writeln  */
  YYSYMBOL_return = 62,                    /* return  */
  YYSYMBOL_for = 63,                       /* for  */
  YYSYMBOL_64_2 = 64,                      /* $@2  */
  YYSYMBOL_if = 65,                        /* if  */
  YYSYMBOL_66_3 = 66,                      /* $@3  */
  YYSYMBOL_67_4 = 67,                      /* $@4  */
  YYSYMBOL_else = 68,                      /* else  */
  YYSYMBOL_69_5 = 69,                      /* $@5  */
  YYSYMBOL_conjuntoop = 70,                /* conjuntoop  */
  YYSYMBOL_conjuntoop1 = 71,               /* conjuntoop1  */
  YYSYMBOL_pertinencia = 72,               /* pertinencia  */
  YYSYMBOL_tipagem = 73,                   /* tipagem  */
  YYSYMBOL_inclusao = 74,                  /* inclusao  */
  YYSYMBOL_remocao = 75,                   /* remocao  */
  YYSYMBOL_selecao = 76,                   /* selecao  */
  YYSYMBOL_iteracao = 77,                  /* iteracao  */
  YYSYMBOL_78_6 = 78,                      /* $@6  */
  YYSYMBOL_79_7 = 79,                      /* $@7  */
  YYSYMBOL_function_call = 80,             /* function_call  */
  YYSYMBOL_args = 81,                      /* args  */
  YYSYMBOL_args1 = 82,                     /* args1  */
  YYSYMBOL_funcargs = 83,                  /* funcargs  */
  YYSYMBOL_function_declaration = 84,      /* function_declaration  */
  YYSYMBOL_85_8 = 85,                      /* $@8  */
  YYSYMBOL_86_9 = 86,                      /* $@9  */
  YYSYMBOL_assignment = 87,                /* assignment  */
  YYSYMBOL_variable_declaration = 88,      /* variable_declaration  */
  YYSYMBOL_mathop = 89,                    /* mathop  */
  YYSYMBOL_mathop1 = 90,                   /* mathop1  */
  YYSYMBOL_mathop2 = 91,                   /* mathop2  */
  YYSYMBOL_matharg = 92,                   /* matharg  */
  YYSYMBOL_type = 93,                      /* type  */
  YYSYMBOL_num = 94                        /* num  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
# define YYCOPY_NEEDED 1
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  72
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   448

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  110
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  232

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   306


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   583,   583,   598,   615,   614,   639,   656,   672,   688,
     704,   722,   739,   755,   771,   787,   803,   831,   847,   893,
     908,   939,   965,   980,   995,  1010,  1025,  1040,  1055,  1070,
    1088,  1116,  1133,  1149,  1170,  1187,  1204,  1229,  1244,  1259,
    1276,  1276,  1312,  1311,  1338,  1357,  1356,  1384,  1406,  1420,
    1434,  1434,  1455,  1470,  1483,  1496,  1509,  1522,  1538,  1551,
    1564,  1577,  1590,  1603,  1627,  1642,  1660,  1679,  1698,  1718,
    1738,  1737,  1763,  1781,  1780,  1807,  1828,  1862,  1900,  1941,
    1958,  1972,  1987,  2003,  2022,  2034,  2053,  2052,  2099,  2098,
    2146,  2191,  2212,  2234,  2249,  2264,  2280,  2295,  2310,  2327,
    2340,  2358,  2380,  2393,  2407,  2424,  2439,  2454,  2469,  2487,
    2501
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  static const char *const yy_sname[] =
  {
  "end of file", "error", "invalid token", "INTEGER", "FLOATING",
  "STRING", "EMPTY", "CHAR", "ID", "INT", "FLOAT", "ELEM", "SET", "IF",
  "ELSE", "FOR", "READ", "WRITE", "WRITELN", "IN", "IS_SET", "ADD",
  "REMOVE", "EXISTS", "FORALL", "RETURN", "OPENPAR", "CLOSEPAR",
  "OPENCURLY", "CLOSECURLY", "OPENBRAC", "CLOSEBRAC", "LESS", "LE", "EQ",
  "NEQ", "GREATER", "GE", "PLUS", "MINUS", "DBS", "AST", "BS", "ASSIGN",
  "COMMA", "SEMICOLON", "OR", "AND", "NOT", "AMP", "PCENT", "VOID",
  "$accept", "inicio", "statement", "$@1", "single_line_statement",
  "comparg", "comparison", "read", "write", "writeln", "return", "for",
  "$@2", "if", "$@3", "$@4", "else", "$@5", "conjuntoop", "conjuntoop1",
  "pertinencia", "tipagem", "inclusao", "remocao", "selecao", "iteracao",
  "$@6", "$@7", "function_call", "args", "args1", "funcargs",
  "function_declaration", "$@8", "$@9", "assignment",
  "variable_declaration", "mathop", "mathop1", "mathop2", "matharg",
  "type", "num", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306
};
#endif

#define YYPACT_NINF (-169)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-92)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     136,   -34,  -169,  -169,  -169,    19,  -169,  -169,  -169,  -169,
      -3,     1,     3,     5,    23,    31,    34,    37,    52,    61,
       6,   416,  -169,   108,   128,  -169,    73,    93,    96,   124,
     130,    73,    73,   140,  -169,  -169,  -169,  -169,   113,    73,
    -169,    73,   144,   146,     2,   125,  -169,  -169,    99,  -169,
    -169,   231,   416,    16,   175,   186,   281,   307,   389,   395,
     395,   395,   358,   182,     6,   422,   399,  -169,    84,    65,
     189,  -169,  -169,  -169,  -169,  -169,  -169,  -169,  -169,  -169,
    -169,   389,  -169,  -169,  -169,    73,   389,   416,   416,   416,
     416,  -169,   171,   180,  -169,   206,   138,    87,   209,   211,
      87,   198,   199,   216,   218,   219,    79,   223,   228,    92,
      10,   232,  -169,  -169,  -169,  -169,   113,   233,   113,   235,
     236,   240,   245,   252,    65,  -169,   422,   422,   422,   422,
     422,   422,   422,   422,  -169,   254,   255,  -169,  -169,  -169,
     125,   125,  -169,  -169,   267,   382,  -169,  -169,  -169,   274,
     300,     6,  -169,  -169,  -169,  -169,  -169,  -169,  -169,  -169,
    -169,  -169,  -169,   326,   352,  -169,  -169,  -169,  -169,  -169,
    -169,  -169,  -169,  -169,  -169,   169,   169,  -169,  -169,   284,
    -169,   284,   260,  -169,  -169,  -169,  -169,  -169,   282,   311,
     297,   189,   248,  -169,   189,  -169,   175,   189,   189,   303,
     291,   308,   309,  -169,  -169,  -169,   310,   313,   312,   316,
     189,   169,   189,   284,   189,   284,  -169,  -169,  -169,   321,
    -169,   328,  -169,   334,  -169,   337,  -169,  -169,  -169,   189,
     338,  -169
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,   109,   110,   104,   101,   106,   108,   107,   105,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      39,     0,     4,     0,     0,     2,     0,     0,     0,     0,
       0,     0,     0,     0,    53,    54,    55,    56,    57,     0,
     103,     0,     0,     0,     0,    95,    98,    99,     0,   102,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   101,     0,     0,     0,    37,    20,     0,
       0,    88,     1,     3,    16,    14,    15,    12,     7,     8,
      17,     0,     9,     6,    13,     0,     0,     0,     0,     0,
       0,    92,    86,    81,    77,     0,    81,    90,     0,     0,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     101,     0,    58,    59,    60,    61,    62,     0,     0,     0,
       0,     0,     0,     0,    20,    21,     0,     0,     0,     0,
       0,     0,     0,     0,   100,     0,     0,    65,    10,    64,
      93,    94,    96,    97,     0,     0,    79,    76,    78,     0,
       0,     0,    30,    32,    33,    31,    35,    36,    34,    66,
      67,    68,    69,     0,     0,    19,    26,    27,    28,    29,
      24,    25,    23,    22,     5,     0,     0,    80,    45,    52,
      42,    52,     0,    73,    75,    70,    72,    85,     0,     0,
       0,     0,     0,    47,     0,    44,     0,     0,     0,     0,
      82,     0,     0,    50,    49,    48,     0,     0,     0,     0,
       0,     0,     0,    52,     0,    52,    40,    74,    71,     0,
      83,     0,    46,     0,    43,     0,    89,    87,    51,     0,
       0,    41
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -169,  -169,   -26,  -169,   -29,   -61,   -46,  -169,  -169,  -169,
    -169,  -169,  -169,   145,  -169,  -169,  -165,  -169,  -169,    69,
     161,   -30,   -28,    -6,   167,  -169,  -169,  -169,  -169,   226,
     280,  -168,  -169,  -169,  -169,   -53,  -169,   -18,    86,   141,
    -169,  -150,  -169
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    24,    25,    70,    26,    66,    67,    27,    28,    29,
      30,    31,   225,    32,   194,   191,   193,   214,    33,   111,
      34,    35,    36,    37,    38,    39,   198,   197,    40,    95,
     146,   188,    41,   144,   136,    42,    43,    44,    45,    46,
      47,    48,    49
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      73,   102,    68,    69,   125,    78,    79,    99,   190,     2,
       3,    50,     4,    82,    63,    83,   195,    98,   123,     2,
       3,    86,     4,    53,    63,   189,   189,    54,   113,    55,
     114,    56,    64,    96,    97,   100,    51,   -63,   106,   109,
      87,    88,    64,   220,   135,    51,   124,   100,   222,    57,
     224,   113,   115,   114,    65,   -63,   113,    58,   114,   138,
      59,   189,    52,    60,    65,   166,   167,   168,   169,   170,
     171,   172,   173,   -11,     1,   115,     2,     3,    61,     4,
     115,     5,     6,     7,     8,     9,    10,    62,    11,    12,
      13,    14,   134,    15,    16,    17,    18,    19,    20,    21,
      91,    22,   -11,    87,    88,   182,   155,    92,   100,   100,
     100,   100,   100,   100,   100,   100,    71,    87,    88,   158,
     179,   181,    87,    88,    23,    87,    88,    96,    72,   -38,
      87,    88,    81,   100,   184,   186,   -11,     1,    74,     2,
       3,    75,     4,   207,     5,     6,     7,     8,     9,    10,
     137,    11,    12,    13,    14,   139,    15,    16,    17,    18,
      19,    20,    21,   204,    22,   202,    89,    90,   206,    76,
     187,   208,   209,   140,   141,    77,    87,    88,     6,     7,
       8,     9,   145,   101,   219,    80,   221,    23,   223,    84,
       1,    85,     2,     3,   103,     4,   -84,     5,     6,     7,
       8,     9,    10,   230,    11,    12,    13,    14,    51,    15,
      16,    17,    18,    19,    20,    21,   -91,    22,   -11,   112,
     117,   119,   120,   122,   145,   116,   118,   118,   118,   118,
     142,   143,    93,   147,     2,     3,   149,     4,   150,    63,
      23,    52,   112,   152,   151,   153,   154,   112,   116,     1,
     156,     2,     3,   116,     4,   157,     5,    21,    94,   159,
     160,    10,   161,   162,    12,    13,    14,   163,    15,    16,
      17,    18,   164,    20,    21,     1,   203,     2,     3,   165,
       4,   175,     5,   174,     2,     3,   104,     4,   105,    63,
      12,    13,    14,   176,    15,    16,    17,    18,   192,    20,
      21,     1,   178,     2,     3,   196,     4,    21,     5,   199,
       2,     3,   107,     4,   108,    63,    12,    13,    14,   200,
      15,    16,    17,    18,   201,    20,    21,     1,   180,     2,
       3,   210,     4,    21,     5,   211,   212,   205,   213,   215,
     216,   217,    12,    13,    14,   218,    15,    16,    17,    18,
     226,    20,    21,     1,   183,     2,     3,   227,     4,   121,
       5,     2,     3,   228,     4,   229,    63,   231,    12,    13,
      14,   177,    15,    16,    17,    18,   148,    20,    21,     0,
     185,    18,     0,    93,    21,     2,     3,     0,     4,     0,
      63,     0,     2,     3,     0,     4,     0,   110,     2,     3,
       0,     4,     0,    63,     0,     0,     0,     0,    21,    15,
      16,    17,    18,     0,     0,    21,     0,     0,    18,     2,
       3,    21,     4,     0,    63,     2,     3,     0,     4,     0,
      63,   126,   127,   128,   129,   130,   131,     0,     0,     0,
       0,     0,    21,     0,     0,   132,   133,     0,    64
};

static const yytype_int16 yycheck[] =
{
      26,    54,    20,    21,    65,    31,    32,    53,   176,     3,
       4,    45,     6,    39,     8,    41,   181,     1,    64,     3,
       4,    19,     6,    26,     8,   175,   176,    26,    58,    26,
      58,    26,    26,    51,    52,    53,    26,    27,    56,    57,
      38,    39,    26,   211,    70,    26,    64,    65,   213,    26,
     215,    81,    58,    81,    48,    45,    86,    26,    86,    85,
      26,   211,    43,    26,    48,   126,   127,   128,   129,   130,
     131,   132,   133,     0,     1,    81,     3,     4,    26,     6,
      86,     8,     9,    10,    11,    12,    13,    26,    15,    16,
      17,    18,    27,    20,    21,    22,    23,    24,    25,    26,
       1,    28,    29,    38,    39,   151,    27,     8,   126,   127,
     128,   129,   130,   131,   132,   133,     8,    38,    39,    27,
     149,   150,    38,    39,    51,    38,    39,   145,     0,    45,
      38,    39,    19,   151,   163,   164,     0,     1,    45,     3,
       4,    45,     6,   196,     8,     9,    10,    11,    12,    13,
      81,    15,    16,    17,    18,    86,    20,    21,    22,    23,
      24,    25,    26,   192,    28,   191,    41,    42,   194,    45,
       1,   197,   198,    87,    88,    45,    38,    39,     9,    10,
      11,    12,    44,     8,   210,    45,   212,    51,   214,    45,
       1,    45,     3,     4,     8,     6,    27,     8,     9,    10,
      11,    12,    13,   229,    15,    16,    17,    18,    26,    20,
      21,    22,    23,    24,    25,    26,    45,    28,    29,    58,
      59,    60,    61,    62,    44,    58,    59,    60,    61,    62,
      89,    90,     1,    27,     3,     4,    27,     6,    27,     8,
      51,    43,    81,    27,    45,    27,    27,    86,    81,     1,
      27,     3,     4,    86,     6,    27,     8,    26,    27,    27,
      27,    13,    27,    27,    16,    17,    18,    27,    20,    21,
      22,    23,    27,    25,    26,     1,    28,     3,     4,    27,
       6,    26,     8,    29,     3,     4,     5,     6,     7,     8,
      16,    17,    18,    26,    20,    21,    22,    23,    14,    25,
      26,     1,    28,     3,     4,    45,     6,    26,     8,    27,
       3,     4,     5,     6,     7,     8,    16,    17,    18,     8,
      20,    21,    22,    23,    27,    25,    26,     1,    28,     3,
       4,    28,     6,    26,     8,    44,    28,   192,    29,    29,
      27,    29,    16,    17,    18,    29,    20,    21,    22,    23,
      29,    25,    26,     1,    28,     3,     4,    29,     6,     1,
       8,     3,     4,    29,     6,    28,     8,    29,    16,    17,
      18,   145,    20,    21,    22,    23,    96,    25,    26,    -1,
      28,    23,    -1,     1,    26,     3,     4,    -1,     6,    -1,
       8,    -1,     3,     4,    -1,     6,    -1,     8,     3,     4,
      -1,     6,    -1,     8,    -1,    -1,    -1,    -1,    26,    20,
      21,    22,    23,    -1,    -1,    26,    -1,    -1,    23,     3,
       4,    26,     6,    -1,     8,     3,     4,    -1,     6,    -1,
       8,    32,    33,    34,    35,    36,    37,    -1,    -1,    -1,
      -1,    -1,    26,    -1,    -1,    46,    47,    -1,    26
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     4,     6,     8,     9,    10,    11,    12,
      13,    15,    16,    17,    18,    20,    21,    22,    23,    24,
      25,    26,    28,    51,    53,    54,    56,    59,    60,    61,
      62,    63,    65,    70,    72,    73,    74,    75,    76,    77,
      80,    84,    87,    88,    89,    90,    91,    92,    93,    94,
      45,    26,    43,    26,    26,    26,    26,    26,    26,    26,
      26,    26,    26,     8,    26,    48,    57,    58,    89,    89,
      55,     8,     0,    54,    45,    45,    45,    45,    54,    54,
      45,    19,    54,    54,    45,    45,    19,    38,    39,    41,
      42,     1,     8,     1,    27,    81,    89,    89,     1,    58,
      89,     8,    87,     8,     5,     7,    89,     5,     7,    89,
       8,    71,    72,    73,    74,    75,    76,    72,    76,    72,
      72,     1,    72,    58,    89,    57,    32,    33,    34,    35,
      36,    37,    46,    47,    27,    54,    86,    71,    54,    71,
      90,    90,    91,    91,    85,    44,    82,    27,    82,    27,
      27,    45,    27,    27,    27,    27,    27,    27,    27,    27,
      27,    27,    27,    27,    27,    27,    57,    57,    57,    57,
      57,    57,    57,    57,    29,    26,    26,    81,    28,    56,
      28,    56,    58,    28,    56,    28,    56,     1,    83,    93,
      83,    67,    14,    68,    66,    68,    45,    79,    78,    27,
       8,    27,    54,    28,    56,    65,    54,    87,    54,    54,
      28,    44,    28,    29,    69,    29,    27,    29,    29,    54,
      83,    54,    68,    54,    68,    64,    29,    29,    29,    28,
      54,    29
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    52,    53,    54,    55,    54,    54,    54,    54,    54,
      54,    54,    56,    56,    56,    56,    56,    56,    56,    57,
      57,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      59,    60,    60,    60,    61,    61,    61,    62,    62,    62,
      64,    63,    66,    65,    65,    67,    65,    65,    68,    68,
      69,    68,    68,    70,    70,    70,    70,    70,    71,    71,
      71,    71,    71,    71,    72,    72,    73,    74,    75,    76,
      78,    77,    77,    79,    77,    77,    80,    80,    81,    81,
      82,    82,    83,    83,    83,    83,    85,    84,    86,    84,
      87,    88,    88,    89,    89,    89,    90,    90,    90,    91,
      91,    92,    92,    92,    92,    93,    93,    93,    93,    94,
      94
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     4,     2,     2,     2,     2,
       3,     0,     2,     2,     2,     2,     2,     2,     2,     3,
       1,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       4,     4,     4,     4,     4,     4,     4,     2,     2,     1,
       0,    12,     0,     9,     6,     0,     9,     6,     2,     2,
       0,     5,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     4,     4,     4,     4,
       0,     8,     5,     0,     8,     5,     4,     3,     2,     2,
       2,     0,     2,     4,     0,     1,     0,     9,     0,     9,
       3,     2,     2,     3,     3,     1,     3,     3,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        YY_LAC_DISCARD ("YYBACKUP");                              \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Given a state stack such that *YYBOTTOM is its bottom, such that
   *YYTOP is either its top or is YYTOP_EMPTY to indicate an empty
   stack, and such that *YYCAPACITY is the maximum number of elements it
   can hold without a reallocation, make sure there is enough room to
   store YYADD more elements.  If not, allocate a new stack using
   YYSTACK_ALLOC, copy the existing elements, and adjust *YYBOTTOM,
   *YYTOP, and *YYCAPACITY to reflect the new capacity and memory
   location.  If *YYBOTTOM != YYBOTTOM_NO_FREE, then free the old stack
   using YYSTACK_FREE.  Return 0 if successful or if no reallocation is
   required.  Return YYENOMEM if memory is exhausted.  */
static int
yy_lac_stack_realloc (YYPTRDIFF_T *yycapacity, YYPTRDIFF_T yyadd,
#if YYDEBUG
                      char const *yydebug_prefix,
                      char const *yydebug_suffix,
#endif
                      yy_state_t **yybottom,
                      yy_state_t *yybottom_no_free,
                      yy_state_t **yytop, yy_state_t *yytop_empty)
{
  YYPTRDIFF_T yysize_old =
    *yytop == yytop_empty ? 0 : *yytop - *yybottom + 1;
  YYPTRDIFF_T yysize_new = yysize_old + yyadd;
  if (*yycapacity < yysize_new)
    {
      YYPTRDIFF_T yyalloc = 2 * yysize_new;
      yy_state_t *yybottom_new;
      /* Use YYMAXDEPTH for maximum stack size given that the stack
         should never need to grow larger than the main state stack
         needs to grow without LAC.  */
      if (YYMAXDEPTH < yysize_new)
        {
          YYDPRINTF ((stderr, "%smax size exceeded%s", yydebug_prefix,
                      yydebug_suffix));
          return YYENOMEM;
        }
      if (YYMAXDEPTH < yyalloc)
        yyalloc = YYMAXDEPTH;
      yybottom_new =
        YY_CAST (yy_state_t *,
                 YYSTACK_ALLOC (YY_CAST (YYSIZE_T,
                                         yyalloc * YYSIZEOF (*yybottom_new))));
      if (!yybottom_new)
        {
          YYDPRINTF ((stderr, "%srealloc failed%s", yydebug_prefix,
                      yydebug_suffix));
          return YYENOMEM;
        }
      if (*yytop != yytop_empty)
        {
          YYCOPY (yybottom_new, *yybottom, yysize_old);
          *yytop = yybottom_new + (yysize_old - 1);
        }
      if (*yybottom != yybottom_no_free)
        YYSTACK_FREE (*yybottom);
      *yybottom = yybottom_new;
      *yycapacity = yyalloc;
    }
  return 0;
}

/* Establish the initial context for the current lookahead if no initial
   context is currently established.

   We define a context as a snapshot of the parser stacks.  We define
   the initial context for a lookahead as the context in which the
   parser initially examines that lookahead in order to select a
   syntactic action.  Thus, if the lookahead eventually proves
   syntactically unacceptable (possibly in a later context reached via a
   series of reductions), the initial context can be used to determine
   the exact set of tokens that would be syntactically acceptable in the
   lookahead's place.  Moreover, it is the context after which any
   further semantic actions would be erroneous because they would be
   determined by a syntactically unacceptable token.

   YY_LAC_ESTABLISH should be invoked when a reduction is about to be
   performed in an inconsistent state (which, for the purposes of LAC,
   includes consistent states that don't know they're consistent because
   their default reductions have been disabled).  Iff there is a
   lookahead token, it should also be invoked before reporting a syntax
   error.  This latter case is for the sake of the debugging output.

   For parse.lac=full, the implementation of YY_LAC_ESTABLISH is as
   follows.  If no initial context is currently established for the
   current lookahead, then check if that lookahead can eventually be
   shifted if syntactic actions continue from the current context.
   Report a syntax error if it cannot.  */
#define YY_LAC_ESTABLISH                                                \
do {                                                                    \
  if (!yy_lac_established)                                              \
    {                                                                   \
      YYDPRINTF ((stderr,                                               \
                  "LAC: initial context established for %s\n",          \
                  yysymbol_name (yytoken)));                            \
      yy_lac_established = 1;                                           \
      switch (yy_lac (yyesa, &yyes, &yyes_capacity, yyssp, yytoken))    \
        {                                                               \
        case YYENOMEM:                                                  \
          goto yyexhaustedlab;                                          \
        case 1:                                                         \
          goto yyerrlab;                                                \
        }                                                               \
    }                                                                   \
} while (0)

/* Discard any previous initial lookahead context because of Event,
   which may be a lookahead change or an invalidation of the currently
   established initial context for the current lookahead.

   The most common example of a lookahead change is a shift.  An example
   of both cases is syntax error recovery.  That is, a syntax error
   occurs when the lookahead is syntactically erroneous for the
   currently established initial context, so error recovery manipulates
   the parser stacks to try to find a new initial context in which the
   current lookahead is syntactically acceptable.  If it fails to find
   such a context, it discards the lookahead.  */
#if YYDEBUG
# define YY_LAC_DISCARD(Event)                                           \
do {                                                                     \
  if (yy_lac_established)                                                \
    {                                                                    \
      YYDPRINTF ((stderr, "LAC: initial context discarded due to "       \
                  Event "\n"));                                          \
      yy_lac_established = 0;                                            \
    }                                                                    \
} while (0)
#else
# define YY_LAC_DISCARD(Event) yy_lac_established = 0
#endif

/* Given the stack whose top is *YYSSP, return 0 iff YYTOKEN can
   eventually (after perhaps some reductions) be shifted, return 1 if
   not, or return YYENOMEM if memory is exhausted.  As preconditions and
   postconditions: *YYES_CAPACITY is the allocated size of the array to
   which *YYES points, and either *YYES = YYESA or *YYES points to an
   array allocated with YYSTACK_ALLOC.  yy_lac may overwrite the
   contents of either array, alter *YYES and *YYES_CAPACITY, and free
   any old *YYES other than YYESA.  */
static int
yy_lac (yy_state_t *yyesa, yy_state_t **yyes,
        YYPTRDIFF_T *yyes_capacity, yy_state_t *yyssp, yysymbol_kind_t yytoken)
{
  yy_state_t *yyes_prev = yyssp;
  yy_state_t *yyesp = yyes_prev;
  /* Reduce until we encounter a shift and thereby accept the token.  */
  YYDPRINTF ((stderr, "LAC: checking lookahead %s:", yysymbol_name (yytoken)));
  if (yytoken == YYSYMBOL_YYUNDEF)
    {
      YYDPRINTF ((stderr, " Always Err\n"));
      return 1;
    }
  while (1)
    {
      int yyrule = yypact[+*yyesp];
      if (yypact_value_is_default (yyrule)
          || (yyrule += yytoken) < 0 || YYLAST < yyrule
          || yycheck[yyrule] != yytoken)
        {
          /* Use the default action.  */
          yyrule = yydefact[+*yyesp];
          if (yyrule == 0)
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
        }
      else
        {
          /* Use the action from yytable.  */
          yyrule = yytable[yyrule];
          if (yytable_value_is_error (yyrule))
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
          if (0 < yyrule)
            {
              YYDPRINTF ((stderr, " S%d\n", yyrule));
              return 0;
            }
          yyrule = -yyrule;
        }
      /* By now we know we have to simulate a reduce.  */
      YYDPRINTF ((stderr, " R%d", yyrule - 1));
      {
        /* Pop the corresponding number of values from the stack.  */
        YYPTRDIFF_T yylen = yyr2[yyrule];
        /* First pop from the LAC stack as many tokens as possible.  */
        if (yyesp != yyes_prev)
          {
            YYPTRDIFF_T yysize = yyesp - *yyes + 1;
            if (yylen < yysize)
              {
                yyesp -= yylen;
                yylen = 0;
              }
            else
              {
                yyesp = yyes_prev;
                yylen -= yysize;
              }
          }
        /* Only afterwards look at the main stack.  */
        if (yylen)
          yyesp = yyes_prev -= yylen;
      }
      /* Push the resulting state of the reduction.  */
      {
        yy_state_fast_t yystate;
        {
          const int yylhs = yyr1[yyrule] - YYNTOKENS;
          const int yyi = yypgoto[yylhs] + *yyesp;
          yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyesp
                     ? yytable[yyi]
                     : yydefgoto[yylhs]);
        }
        if (yyesp == yyes_prev)
          {
            yyesp = *yyes;
            YY_IGNORE_USELESS_CAST_BEGIN
            *yyesp = YY_CAST (yy_state_t, yystate);
            YY_IGNORE_USELESS_CAST_END
          }
        else
          {
            if (yy_lac_stack_realloc (yyes_capacity, 1,
#if YYDEBUG
                                      " (", ")",
#endif
                                      yyes, yyesa, &yyesp, yyes_prev))
              {
                YYDPRINTF ((stderr, "\n"));
                return YYENOMEM;
              }
            YY_IGNORE_USELESS_CAST_BEGIN
            *++yyesp = YY_CAST (yy_state_t, yystate);
            YY_IGNORE_USELESS_CAST_END
          }
        YYDPRINTF ((stderr, " G%d", yystate));
      }
    }
}

/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yy_state_t *yyesa;
  yy_state_t **yyes;
  YYPTRDIFF_T *yyes_capacity;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;

  int yyx;
  for (yyx = 0; yyx < YYNTOKENS; ++yyx)
    {
      yysymbol_kind_t yysym = YY_CAST (yysymbol_kind_t, yyx);
      if (yysym != YYSYMBOL_YYerror && yysym != YYSYMBOL_YYUNDEF)
        switch (yy_lac (yyctx->yyesa, yyctx->yyes, yyctx->yyes_capacity, yyctx->yyssp, yysym))
          {
          case YYENOMEM:
            return YYENOMEM;
          case 1:
            continue;
          default:
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = yysym;
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif



static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
       In the first two cases, it might appear that the current syntax
       error should have been detected in the previous state when yy_lac
       was invoked.  However, at that time, there might have been a
       different syntax error that discarded a different initial context
       during error recovery, leaving behind the current lookahead.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      YYDPRINTF ((stderr, "Constructing syntax error message\n"));
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else if (yyn == 0)
        YYDPRINTF ((stderr, "No expected tokens.\n"));
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.  In order to see if a particular token T is a
   valid looakhead, invoke yy_lac (YYESA, YYES, YYES_CAPACITY, YYSSP, T).

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store or if
   yy_lac returned YYENOMEM.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yystrlen (yysymbol_name (yyarg[yyi]));
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp = yystpcpy (yyp, yysymbol_name (yyarg[yyi++]));
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_INTEGER: /* INTEGER  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2010 "test.tab.c"
        break;

    case YYSYMBOL_FLOATING: /* FLOATING  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2016 "test.tab.c"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2022 "test.tab.c"
        break;

    case YYSYMBOL_EMPTY: /* EMPTY  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2028 "test.tab.c"
        break;

    case YYSYMBOL_CHAR: /* CHAR  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2034 "test.tab.c"
        break;

    case YYSYMBOL_ID: /* ID  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2040 "test.tab.c"
        break;

    case YYSYMBOL_INT: /* INT  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2046 "test.tab.c"
        break;

    case YYSYMBOL_FLOAT: /* FLOAT  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2052 "test.tab.c"
        break;

    case YYSYMBOL_ELEM: /* ELEM  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2058 "test.tab.c"
        break;

    case YYSYMBOL_SET: /* SET  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2064 "test.tab.c"
        break;

    case YYSYMBOL_IF: /* IF  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2070 "test.tab.c"
        break;

    case YYSYMBOL_ELSE: /* ELSE  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2076 "test.tab.c"
        break;

    case YYSYMBOL_FOR: /* FOR  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2082 "test.tab.c"
        break;

    case YYSYMBOL_READ: /* READ  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2088 "test.tab.c"
        break;

    case YYSYMBOL_WRITE: /* WRITE  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2094 "test.tab.c"
        break;

    case YYSYMBOL_WRITELN: /* WRITELN  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2100 "test.tab.c"
        break;

    case YYSYMBOL_IN: /* IN  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2106 "test.tab.c"
        break;

    case YYSYMBOL_IS_SET: /* IS_SET  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2112 "test.tab.c"
        break;

    case YYSYMBOL_ADD: /* ADD  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2118 "test.tab.c"
        break;

    case YYSYMBOL_REMOVE: /* REMOVE  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2124 "test.tab.c"
        break;

    case YYSYMBOL_EXISTS: /* EXISTS  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2130 "test.tab.c"
        break;

    case YYSYMBOL_FORALL: /* FORALL  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2136 "test.tab.c"
        break;

    case YYSYMBOL_RETURN: /* RETURN  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2142 "test.tab.c"
        break;

    case YYSYMBOL_OPENPAR: /* OPENPAR  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2148 "test.tab.c"
        break;

    case YYSYMBOL_CLOSEPAR: /* CLOSEPAR  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2154 "test.tab.c"
        break;

    case YYSYMBOL_OPENCURLY: /* OPENCURLY  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2160 "test.tab.c"
        break;

    case YYSYMBOL_CLOSECURLY: /* CLOSECURLY  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2166 "test.tab.c"
        break;

    case YYSYMBOL_OPENBRAC: /* OPENBRAC  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2172 "test.tab.c"
        break;

    case YYSYMBOL_CLOSEBRAC: /* CLOSEBRAC  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2178 "test.tab.c"
        break;

    case YYSYMBOL_LESS: /* LESS  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2184 "test.tab.c"
        break;

    case YYSYMBOL_LE: /* LE  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2190 "test.tab.c"
        break;

    case YYSYMBOL_EQ: /* EQ  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2196 "test.tab.c"
        break;

    case YYSYMBOL_NEQ: /* NEQ  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2202 "test.tab.c"
        break;

    case YYSYMBOL_GREATER: /* GREATER  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2208 "test.tab.c"
        break;

    case YYSYMBOL_GE: /* GE  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2214 "test.tab.c"
        break;

    case YYSYMBOL_PLUS: /* PLUS  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2220 "test.tab.c"
        break;

    case YYSYMBOL_MINUS: /* MINUS  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2226 "test.tab.c"
        break;

    case YYSYMBOL_DBS: /* DBS  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2232 "test.tab.c"
        break;

    case YYSYMBOL_AST: /* AST  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2238 "test.tab.c"
        break;

    case YYSYMBOL_BS: /* BS  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2244 "test.tab.c"
        break;

    case YYSYMBOL_ASSIGN: /* ASSIGN  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2250 "test.tab.c"
        break;

    case YYSYMBOL_COMMA: /* COMMA  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2256 "test.tab.c"
        break;

    case YYSYMBOL_SEMICOLON: /* SEMICOLON  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2262 "test.tab.c"
        break;

    case YYSYMBOL_OR: /* OR  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2268 "test.tab.c"
        break;

    case YYSYMBOL_AND: /* AND  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2274 "test.tab.c"
        break;

    case YYSYMBOL_NOT: /* NOT  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2280 "test.tab.c"
        break;

    case YYSYMBOL_AMP: /* AMP  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2286 "test.tab.c"
        break;

    case YYSYMBOL_PCENT: /* PCENT  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2292 "test.tab.c"
        break;

    case YYSYMBOL_VOID: /* VOID  */
#line 560 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2298 "test.tab.c"
        break;

    case YYSYMBOL_statement: /* statement  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2304 "test.tab.c"
        break;

    case YYSYMBOL_single_line_statement: /* single_line_statement  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2310 "test.tab.c"
        break;

    case YYSYMBOL_comparg: /* comparg  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2316 "test.tab.c"
        break;

    case YYSYMBOL_comparison: /* comparison  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2322 "test.tab.c"
        break;

    case YYSYMBOL_read: /* read  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2328 "test.tab.c"
        break;

    case YYSYMBOL_write: /* write  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2334 "test.tab.c"
        break;

    case YYSYMBOL_writeln: /* writeln  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2340 "test.tab.c"
        break;

    case YYSYMBOL_return: /* return  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2346 "test.tab.c"
        break;

    case YYSYMBOL_for: /* for  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2352 "test.tab.c"
        break;

    case YYSYMBOL_if: /* if  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2358 "test.tab.c"
        break;

    case YYSYMBOL_else: /* else  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2364 "test.tab.c"
        break;

    case YYSYMBOL_conjuntoop: /* conjuntoop  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2370 "test.tab.c"
        break;

    case YYSYMBOL_conjuntoop1: /* conjuntoop1  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2376 "test.tab.c"
        break;

    case YYSYMBOL_pertinencia: /* pertinencia  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2382 "test.tab.c"
        break;

    case YYSYMBOL_tipagem: /* tipagem  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2388 "test.tab.c"
        break;

    case YYSYMBOL_inclusao: /* inclusao  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2394 "test.tab.c"
        break;

    case YYSYMBOL_remocao: /* remocao  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2400 "test.tab.c"
        break;

    case YYSYMBOL_selecao: /* selecao  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2406 "test.tab.c"
        break;

    case YYSYMBOL_iteracao: /* iteracao  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2412 "test.tab.c"
        break;

    case YYSYMBOL_function_call: /* function_call  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2418 "test.tab.c"
        break;

    case YYSYMBOL_args: /* args  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2424 "test.tab.c"
        break;

    case YYSYMBOL_args1: /* args1  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2430 "test.tab.c"
        break;

    case YYSYMBOL_funcargs: /* funcargs  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2436 "test.tab.c"
        break;

    case YYSYMBOL_function_declaration: /* function_declaration  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2442 "test.tab.c"
        break;

    case YYSYMBOL_assignment: /* assignment  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2448 "test.tab.c"
        break;

    case YYSYMBOL_variable_declaration: /* variable_declaration  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2454 "test.tab.c"
        break;

    case YYSYMBOL_mathop: /* mathop  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2460 "test.tab.c"
        break;

    case YYSYMBOL_mathop1: /* mathop1  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2466 "test.tab.c"
        break;

    case YYSYMBOL_mathop2: /* mathop2  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2472 "test.tab.c"
        break;

    case YYSYMBOL_matharg: /* matharg  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2478 "test.tab.c"
        break;

    case YYSYMBOL_type: /* type  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2484 "test.tab.c"
        break;

    case YYSYMBOL_num: /* num  */
#line 560 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2490 "test.tab.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    yy_state_t yyesa[20];
    yy_state_t *yyes = yyesa;
    YYPTRDIFF_T yyes_capacity = 20 < YYMAXDEPTH ? 20 : YYMAXDEPTH;

  /* Whether LAC context is established.  A Boolean.  */
  int yy_lac_established = 0;
  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    {
      YY_LAC_ESTABLISH;
      goto yydefault;
    }
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      YY_LAC_ESTABLISH;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  YY_LAC_DISCARD ("shift");
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  {
    int yychar_backup = yychar;
    switch (yyn)
      {
  case 2: /* inicio: statement  */
#line 583 "test.y"
                                                                                        {
													raiz = (yyvsp[0].node);
													//(*raiz).escopo = 1;
													if(ProcurarTabela("main") == NULL){
														printf("ERRO SEMANTICO! NAO FOI DECLARADA UMA FUNCAO MAIN!\n");
														errorCheck = TRUE;
													}
												}
#line 2781 "test.tab.c"
    break;

  case 3: /* statement: single_line_statement statement  */
#line 598 "test.y"
                                                                                        {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).filhos[0] = (yyvsp[-1].node);
																(*ancora).filhos[1] = (yyvsp[0].node);
																//(*(no*)$1).escopo = (*ancora).escopo;
																//(*(no*)$2).escopo = (*ancora).escopo;
																(*ancora).numFilhos = 2;
																char ancora2[] = "single_line_statement";
																(*ancora).nome = strdup(ancora2);
																(*ancora).tipo = YYSYMBOL_statement;
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
																(yyval.node) = ancora;
															}
#line 2802 "test.tab.c"
    break;

  case 4: /* $@1: %empty  */
#line 615 "test.y"
                                                                                                                        {
																escopoCounter++;
																Push(pilhaEscopo,CriarStack(escopoCounter));
																(yyvsp[0].text) = NULL;
															}
#line 2812 "test.tab.c"
    break;

  case 5: /* statement: OPENCURLY $@1 statement CLOSECURLY  */
#line 620 "test.y"
                                                                                                {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).filhos[0] = (yyvsp[-1].node);
																//(*(no*)$1).escopo = (*ancora).escopo;
																//(*(no*)$2).escopo = (*ancora).escopo;
																(*ancora).numFilhos = 1;
																char ancora2[] = "curly";
																(*ancora).nome = strdup(ancora2);
																(*ancora).tipo = YYSYMBOL_statement;
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(yyval.node) = ancora;
																// $ 3 = NULL;
																(yyvsp[0].text) = NULL;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
																Pop(pilhaEscopo);
															}
#line 2835 "test.tab.c"
    break;

  case 6: /* statement: function_declaration statement  */
#line 639 "test.y"
                                                                                        {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).filhos[0] = (yyvsp[-1].node);
																(*ancora).filhos[1] = (yyvsp[0].node);
																//(*(no*)$1).escopo = (*ancora).escopo;
																//(*(no*)$2).escopo = (*ancora).escopo;
																(*ancora).numFilhos = 2;
																char ancora2[] = "function_declaration";
																(*ancora).nome = strdup(ancora2);
																(*ancora).tipo = YYSYMBOL_statement;
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
																(yyval.node) = ancora;
															}
#line 2856 "test.tab.c"
    break;

  case 7: /* statement: for statement  */
#line 656 "test.y"
                                                                                                        {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).filhos[0] = (yyvsp[-1].node);
																(*ancora).filhos[1] = (yyvsp[0].node);
																//(*(no*)$1).escopo = (*ancora).escopo;
																//(*(no*)$2).escopo = (*ancora).escopo;
																(*ancora).numFilhos = 2;
																char ancora2[] = "for";
																(*ancora).nome = strdup(ancora2);
																(*ancora).tipo = YYSYMBOL_statement;
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
																(yyval.node) = ancora;
															}
#line 2877 "test.tab.c"
    break;

  case 8: /* statement: if statement  */
#line 672 "test.y"
                                                                                                        {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).filhos[0] = (yyvsp[-1].node);
																(*ancora).filhos[1] = (yyvsp[0].node);
																//(*(no*)$1).escopo = (*ancora).escopo;
																//(*(no*)$2).escopo = (*ancora).escopo;
																(*ancora).numFilhos = 2;
																char ancora2[] = "if";
																(*ancora).nome = strdup(ancora2);
																(*ancora).tipo = YYSYMBOL_statement;
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
																(yyval.node) = ancora;
															}
#line 2898 "test.tab.c"
    break;

  case 9: /* statement: iteracao statement  */
#line 688 "test.y"
                                                                                                        {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).numFilhos = 2;
																(*ancora).filhos[0] = (yyvsp[-1].node);
																(*ancora).filhos[1] = (yyvsp[0].node);
																//(*(no*)$1).escopo = (*ancora).escopo;
																//(*(no*)$2).escopo = (*ancora).escopo;
																(*ancora).tipo = YYSYMBOL_statement;
																char ancora2[] = "iteracao";
																(*ancora).nome = strdup(ancora2);
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
																(yyval.node) = ancora;
															}
#line 2919 "test.tab.c"
    break;

  case 10: /* statement: variable_declaration SEMICOLON statement  */
#line 704 "test.y"
                                                                                {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).filhos[0] = (yyvsp[-2].node);
																(*ancora).filhos[1] = (yyvsp[0].node);
																(*ancora).numFilhos = 2;
																//(*(no*)$1).escopo = (*ancora).escopo;
																//(*(no*)$3).escopo = (*ancora).escopo;
																char ancora2[] = "variable_declaration";
																(*ancora).nome = strdup(ancora2);
																(*ancora).tipo = YYSYMBOL_statement;
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(yyval.node) = ancora;
																(yyvsp[-1].text) = NULL;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
															}
#line 2941 "test.tab.c"
    break;

  case 11: /* statement: %empty  */
#line 722 "test.y"
                                                                                                                {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).numFilhos = 0;
																char ancora2[] = "epsilon";
																(*ancora).nome = strdup(ancora2);
																(*ancora).tipo = YYSYMBOL_statement;
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(yyval.node) = ancora;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
															}
#line 2958 "test.tab.c"
    break;

  case 12: /* single_line_statement: return SEMICOLON  */
#line 739 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "return";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(yyval.node) = ancora;
													(yyvsp[0].text) = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
#line 2978 "test.tab.c"
    break;

  case 13: /* single_line_statement: assignment SEMICOLON  */
#line 755 "test.y"
                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "assignment";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(yyval.node) = ancora;
													(yyvsp[0].text) = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
#line 2998 "test.tab.c"
    break;

  case 14: /* single_line_statement: write SEMICOLON  */
#line 771 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "write";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(yyval.node) = ancora;
													(yyvsp[0].text) = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
#line 3018 "test.tab.c"
    break;

  case 15: /* single_line_statement: writeln SEMICOLON  */
#line 787 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "writeln";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(yyval.node) = ancora;
													(yyvsp[0].text) = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
#line 3038 "test.tab.c"
    break;

  case 16: /* single_line_statement: read SEMICOLON  */
#line 803 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "read";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(yyval.node) = ancora;
													(yyvsp[0].text) = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
#line 3058 "test.tab.c"
    break;

  case 17: /* single_line_statement: conjuntoop SEMICOLON  */
#line 831 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "conjuntoop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(yyval.node) = ancora;
													(yyvsp[0].text) = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
#line 3078 "test.tab.c"
    break;

  case 18: /* single_line_statement: error SEMICOLON  */
#line 847 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													char ancora2[] = "ERROR";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[0].text) = NULL;
													errorCheck = TRUE;
													yyerrok;
												}
#line 3098 "test.tab.c"
    break;

  case 19: /* comparg: OPENPAR comparison CLOSEPAR  */
#line 893 "test.y"
                                                                        {	
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_comparg;
													char ancora2[] = "comparison";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ((yyvsp[-1].node))->tipoVirtual;
													(yyval.node) = ancora;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3118 "test.tab.c"
    break;

  case 20: /* comparg: mathop  */
#line 908 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_comparg;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).valor = NULL;
													(*ancora).refereTabela = NULL;
													(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
													(*ancora).conversion = None;
													(yyval.node) = ancora;
												}
#line 3136 "test.tab.c"
    break;

  case 21: /* comparison: NOT comparg  */
#line 939 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "not";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													switch(((yyvsp[0].node))->tipoVirtual){
														case Elem:
															(*ancora).conversion = ElemToIntLeft;
															(*ancora).tipoVirtual = Int;
															break;
														case Set:
															printf("ERRO SEMANTICO! EXPRESSAO RESULTANTE EM SET USADA EM OPERACAO INVALIDA! Linha: %d, Coluna: %d\n",linhaCount,colunaCount);
															errorCheck = TRUE;
															break;
														default:
															(*ancora).conversion = None;
															(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
															break;
													}
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3167 "test.tab.c"
    break;

  case 22: /* comparison: comparg AND comparg  */
#line 965 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "and";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
													(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3187 "test.tab.c"
    break;

  case 23: /* comparison: comparg OR comparg  */
#line 980 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "or";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
													(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3207 "test.tab.c"
    break;

  case 24: /* comparison: comparg GREATER comparg  */
#line 995 "test.y"
                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "greater";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
													(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3227 "test.tab.c"
    break;

  case 25: /* comparison: comparg GE comparg  */
#line 1010 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "greater_or_equal";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
													(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3247 "test.tab.c"
    break;

  case 26: /* comparison: comparg LESS comparg  */
#line 1025 "test.y"
                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "less";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
													(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3267 "test.tab.c"
    break;

  case 27: /* comparison: comparg LE comparg  */
#line 1040 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "less_or_equal";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
													(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3287 "test.tab.c"
    break;

  case 28: /* comparison: comparg EQ comparg  */
#line 1055 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "equal";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
													(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3307 "test.tab.c"
    break;

  case 29: /* comparison: comparg NEQ comparg  */
#line 1070 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "not_equal";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
													(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 3327 "test.tab.c"
    break;

  case 30: /* read: READ OPENPAR ID CLOSEPAR  */
#line 1088 "test.y"
                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_read;
													char ancora2[] = "read";
													(*ancora).nome = strdup(ancora2);
													simbolo *ancoraSimb = VerificarEscopo((yyvsp[-1].text));
													if(ancoraSimb != NULL){ 
														(*ancora).refereTabela = ancoraSimb;
													}
													else{
														printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[-1].text),linhaCount,colunaCount);
														(*ancora).refereTabela = NULL;
														errorCheck = TRUE;
													}
													(yyval.node) = ancora;
													(*ancora).valor = strdup((yyvsp[-1].text));
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													free((yyvsp[-1].text));
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3356 "test.tab.c"
    break;

  case 31: /* write: WRITE OPENPAR mathop CLOSEPAR  */
#line 1116 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_write;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3377 "test.tab.c"
    break;

  case 32: /* write: WRITE OPENPAR STRING CLOSEPAR  */
#line 1133 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_write;
													char ancora2[] = "string";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = strdup((yyvsp[-1].text));
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													free((yyvsp[-1].text));
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3398 "test.tab.c"
    break;

  case 33: /* write: WRITE OPENPAR CHAR CLOSEPAR  */
#line 1149 "test.y"
                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_write;
													char ancora2[] = "char";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = strdup((yyvsp[-1].text));
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													free((yyvsp[-1].text));
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3419 "test.tab.c"
    break;

  case 34: /* writeln: WRITELN OPENPAR mathop CLOSEPAR  */
#line 1170 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[-1].node);
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_writeln;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3440 "test.tab.c"
    break;

  case 35: /* writeln: WRITELN OPENPAR STRING CLOSEPAR  */
#line 1187 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_writeln;
													char ancora2[] = "string";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = strdup((yyvsp[-1].text));
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													free((yyvsp[-1].text));
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3461 "test.tab.c"
    break;

  case 36: /* writeln: WRITELN OPENPAR CHAR CLOSEPAR  */
#line 1204 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_writeln;
													char ancora2[] = "char";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = strdup((yyvsp[-1].text));
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													free((yyvsp[-1].text));
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3482 "test.tab.c"
    break;

  case 37: /* return: RETURN comparison  */
#line 1229 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_return;
													char ancora2[] = "comparison";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
													//free($3);
												}
#line 3502 "test.tab.c"
    break;

  case 38: /* return: RETURN mathop  */
#line 1244 "test.y"
                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_return;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
													//free($3);
												}
#line 3522 "test.tab.c"
    break;

  case 39: /* return: RETURN  */
#line 1259 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_return;
													char ancora2[] = "null";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[0].text) = NULL;
													//free($2);
												}
#line 3541 "test.tab.c"
    break;

  case 40: /* $@2: %empty  */
#line 1276 "test.y"
                                                                                                                                                        {
																													escopoCounter++;
																													Push(pilhaEscopo,CriarStack(escopoCounter));
																													(yyvsp[-7].text) = NULL;
																													(yyvsp[-6].text) = NULL;
																													(yyvsp[-4].text) = NULL;
																													(yyvsp[-2].text) = NULL;
																													(yyvsp[0].text) = NULL;
																													
																													
																												}
#line 3557 "test.tab.c"
    break;

  case 41: /* for: FOR OPENPAR assignment SEMICOLON comparison SEMICOLON assignment CLOSEPAR $@2 OPENCURLY statement CLOSECURLY  */
#line 1287 "test.y"
                                                                                                                                                                                                {
																													no* ancora = (no*)malloc(sizeof(no));
																													(*ancora).filhos[0] = (yyvsp[-9].node);
																													(*ancora).filhos[1] = (yyvsp[-7].node);
																													(*ancora).filhos[2] = (yyvsp[-5].node);
																													//$ $ = ancora;
																													(*ancora).numFilhos = 4;
																													(*ancora).tipo = YYSYMBOL_for;
																													char ancora2[] = "for";
																													(*ancora).nome = strdup(ancora2);
																													(*ancora).refereTabela = NULL;
																													(*ancora).valor = NULL;
																													(*ancora).filhos[3] = (yyvsp[-1].node);
																													(*ancora).conversion = None;
																													(*ancora).tipoVirtual = 0;
																													(yyvsp[0].text) = NULL;
																													(yyvsp[-2].text) = NULL;
																													(yyval.node) = ancora;
																													Pop(pilhaEscopo);
																												}
#line 3582 "test.tab.c"
    break;

  case 42: /* $@3: %empty  */
#line 1312 "test.y"
                                                                                                                                                                                                                        {
																												escopoCounter++;
																												Push(pilhaEscopo,CriarStack(escopoCounter));
																												
																											}
#line 3592 "test.tab.c"
    break;

  case 43: /* if: IF OPENPAR comparison CLOSEPAR OPENCURLY $@3 statement CLOSECURLY else  */
#line 1317 "test.y"
                                                                                                                                                                                                {
																												no* ancora = (no*)malloc(sizeof(no));
																												(*ancora).filhos[0] = (yyvsp[-6].node);
																												(yyvsp[-8].text) = NULL;
																												(yyvsp[-7].text) = NULL;
																												(yyvsp[-5].text) = NULL;
																												(yyvsp[-4].text) = NULL;
																												char ancora2[] = "if";
																												(*ancora).nome = strdup(ancora2);
																												(*ancora).refereTabela = NULL;
																												(*ancora).valor = NULL;
																												(*ancora).numFilhos = 3;
																												(*ancora).tipo = YYSYMBOL_if;
																												(*ancora).filhos[1] = (yyvsp[-2].node);
																												(*ancora).filhos[2] = (yyvsp[0].node);
																												(*ancora).conversion = None;
																												(*ancora).tipoVirtual = 0;
																												(yyvsp[-1].text) = NULL;
																												Pop(pilhaEscopo);
																												(yyval.node) = ancora;
																											}
#line 3618 "test.tab.c"
    break;

  case 44: /* if: IF OPENPAR comparison CLOSEPAR single_line_statement else  */
#line 1338 "test.y"
                                                                                                                                                        {
																												no* ancora = (no*)malloc(sizeof(no));
																												(*ancora).filhos[0] = (yyvsp[-3].node);
																												(*ancora).filhos[1] = (yyvsp[-1].node);
																												(*ancora).filhos[2] = (yyvsp[0].node);
																												(*ancora).numFilhos = 3;
																												(*ancora).tipo = YYSYMBOL_if;
																												char ancora2[] = "single_line_statement";
																												(*ancora).nome = strdup(ancora2);
																												(*ancora).refereTabela = NULL;
																												(*ancora).valor = NULL;
																												(*ancora).conversion = None;
																												(*ancora).tipoVirtual = 0;
																												(yyval.node) = ancora;
																												(yyvsp[-5].text) = NULL;
																												(yyvsp[-4].text) = NULL;
																												(yyvsp[-2].text) = NULL;
																											}
#line 3641 "test.tab.c"
    break;

  case 45: /* $@4: %empty  */
#line 1357 "test.y"
                                                                                                                                                                                                                        {
																												escopoCounter++;
																												Push(pilhaEscopo,CriarStack(escopoCounter));
																												
																											}
#line 3651 "test.tab.c"
    break;

  case 46: /* if: IF OPENPAR error CLOSEPAR OPENCURLY $@4 statement CLOSECURLY else  */
#line 1362 "test.y"
                                                                                                                                                                                                {
																												no* ancora = (no*)malloc(sizeof(no));
																												(yyvsp[-8].text) = NULL;
																												(yyvsp[-7].text) = NULL;
																												(yyvsp[-5].text) = NULL;
																												(yyvsp[-4].text) = NULL;
																												char ancora2[] = "ERROR";
																												(*ancora).nome = strdup(ancora2);
																												(*ancora).refereTabela = NULL;
																												(*ancora).valor = NULL;
																												(*ancora).numFilhos = 2;
																												(*ancora).tipo = YYSYMBOL_if;
																												(*ancora).filhos[0] = (yyvsp[-2].node);
																												(*ancora).filhos[1] = (yyvsp[0].node);
																												(*ancora).conversion = None;
																												(*ancora).tipoVirtual = 0;
																												(yyvsp[-1].text) = NULL;
																												Pop(pilhaEscopo);
																												(yyval.node) = ancora;
																												errorCheck = TRUE;
																												yyerrok;
																											}
#line 3678 "test.tab.c"
    break;

  case 47: /* if: IF OPENPAR error CLOSEPAR single_line_statement else  */
#line 1384 "test.y"
                                                                                                                                                                {
																												no* ancora = (no*)malloc(sizeof(no));
																												(*ancora).filhos[0] = (yyvsp[-1].node);
																												(*ancora).filhos[1] = (yyvsp[0].node);
																												(*ancora).numFilhos = 2;
																												(*ancora).tipo = YYSYMBOL_if;
																												char ancora2[] = "ERROR";
																												(*ancora).nome = strdup(ancora2);
																												(*ancora).refereTabela = NULL;
																												(*ancora).valor = NULL;
																												(*ancora).conversion = None;
																												(*ancora).tipoVirtual = 0;
																												(yyval.node) = ancora;
																												(yyvsp[-5].text) = NULL;
																												(yyvsp[-4].text) = NULL;
																												(yyvsp[-2].text) = NULL;
																												errorCheck = TRUE;
																												yyerrok;
																											}
#line 3702 "test.tab.c"
    break;

  case 48: /* else: ELSE if  */
#line 1406 "test.y"
                                                                                                {
														no* ancora = (no*)malloc(sizeof(no));
														(*ancora).filhos[0] = (yyvsp[0].node);
														(*ancora).numFilhos = 1;
														char ancora2[] = "if";
														(*ancora).nome = strdup(ancora2);
														(*ancora).tipo = YYSYMBOL_else;
														(*ancora).refereTabela = NULL;
														(*ancora).valor = NULL;
														(*ancora).conversion = None;
														(*ancora).tipoVirtual = 0;
														(yyval.node) = ancora;
														free((yyvsp[-1].text));
													}
#line 3721 "test.tab.c"
    break;

  case 49: /* else: ELSE single_line_statement  */
#line 1420 "test.y"
                                                                                {
														no* ancora = (no*)malloc(sizeof(no));
														(*ancora).filhos[0] = (yyvsp[0].node);
														(*ancora).numFilhos = 1;
														char ancora2[] = "single_line_statement";
														(*ancora).nome = strdup(ancora2);
														(*ancora).tipo = YYSYMBOL_else;
														(*ancora).refereTabela = NULL;
														(*ancora).valor = NULL;
														(*ancora).conversion = None;
														(*ancora).tipoVirtual = 0;
														(yyval.node) = ancora;
														(yyvsp[-1].text) = NULL;
													}
#line 3740 "test.tab.c"
    break;

  case 50: /* $@5: %empty  */
#line 1434 "test.y"
                                                                                        {
														escopoCounter++;
														Push(pilhaEscopo,CriarStack(escopoCounter));
														(yyvsp[-1].text) = NULL;
														(yyvsp[0].text) = NULL;
													}
#line 3751 "test.tab.c"
    break;

  case 51: /* else: ELSE OPENCURLY $@5 statement CLOSECURLY  */
#line 1440 "test.y"
                                                                                {
														no* ancora = (no*)malloc(sizeof(no));
														(*ancora).filhos[0] = (yyvsp[-1].node);
														(*ancora).numFilhos = 1;
														char ancora2[] = "curly";
														(*ancora).nome = strdup(ancora2);
														(*ancora).tipo = YYSYMBOL_else;
														(*ancora).refereTabela = NULL;
														(*ancora).valor = NULL;
														(*ancora).conversion = None;
														(*ancora).tipoVirtual = 0;
														(yyval.node) = ancora;
														(yyvsp[0].text) = NULL;
														Pop(pilhaEscopo);
													}
#line 3771 "test.tab.c"
    break;

  case 52: /* else: %empty  */
#line 1455 "test.y"
                                                                                                {
														no* ancora = (no*)malloc(sizeof(no));
														(*ancora).numFilhos = 0;
														char ancora2[] = "epsilon";
														(*ancora).nome = strdup(ancora2);
														(*ancora).tipo = YYSYMBOL_else;
														(*ancora).refereTabela = NULL;
														(*ancora).valor = NULL;
														(*ancora).conversion = None;
														(*ancora).tipoVirtual = 0;
														(yyval.node) = ancora;
													}
#line 3788 "test.tab.c"
    break;

  case 53: /* conjuntoop: pertinencia  */
#line 1470 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "pertinencia";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
													(yyval.node) = ancora;
												}
#line 3806 "test.tab.c"
    break;

  case 54: /* conjuntoop: tipagem  */
#line 1483 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "tipagem";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
													(yyval.node) = ancora;
												}
#line 3824 "test.tab.c"
    break;

  case 55: /* conjuntoop: inclusao  */
#line 1496 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "inclusao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													(yyval.node) = ancora;
												}
#line 3842 "test.tab.c"
    break;

  case 56: /* conjuntoop: remocao  */
#line 1509 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "remocao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													(yyval.node) = ancora;
												}
#line 3860 "test.tab.c"
    break;

  case 57: /* conjuntoop: selecao  */
#line 1522 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "selecao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													(yyval.node) = ancora;
												}
#line 3878 "test.tab.c"
    break;

  case 58: /* conjuntoop1: pertinencia  */
#line 1538 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "pertinencia";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
													(yyval.node) = ancora;
												}
#line 3896 "test.tab.c"
    break;

  case 59: /* conjuntoop1: tipagem  */
#line 1551 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "tipagem";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
													(yyval.node) = ancora;
												}
#line 3914 "test.tab.c"
    break;

  case 60: /* conjuntoop1: inclusao  */
#line 1564 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "inclusao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													(yyval.node) = ancora;
												}
#line 3932 "test.tab.c"
    break;

  case 61: /* conjuntoop1: remocao  */
#line 1577 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "remocao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													(yyval.node) = ancora;
												}
#line 3950 "test.tab.c"
    break;

  case 62: /* conjuntoop1: selecao  */
#line 1590 "test.y"
                                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "selecao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													(yyval.node) = ancora;
												}
#line 3968 "test.tab.c"
    break;

  case 63: /* conjuntoop1: ID  */
#line 1603 "test.y"
                                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "ID";
													(*ancora).nome = strdup(ancora2);
													simbolo *ancoraSimb = VerificarEscopo((yyvsp[0].text));
													if(ancoraSimb != NULL){ 
														(*ancora).refereTabela = ancoraSimb;
													}
													else{
														printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[0].text),linhaCount,colunaCount);
														errorCheck = TRUE;
														(*ancora).refereTabela = NULL;
													}
													(*ancora).valor = strdup((yyvsp[0].text));
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = (*ancoraSimb).tipo;
													free((yyvsp[0].text));
													(yyval.node) = ancora;
												}
#line 3994 "test.tab.c"
    break;

  case 64: /* pertinencia: mathop IN conjuntoop1  */
#line 1627 "test.y"
                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 2;
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_pertinencia;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 4014 "test.tab.c"
    break;

  case 65: /* pertinencia: selecao IN conjuntoop1  */
#line 1642 "test.y"
                                                                        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 2;
													(*ancora).filhos[0] = (yyvsp[-2].node);
													(*ancora).filhos[1] = (yyvsp[0].node);
													(*ancora).tipo = YYSYMBOL_pertinencia;
													char ancora2[] = "selecao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-1].text) = NULL;
												}
#line 4034 "test.tab.c"
    break;

  case 66: /* tipagem: IS_SET OPENPAR conjuntoop1 CLOSEPAR  */
#line 1660 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[-1].node);
													(*ancora).tipo = YYSYMBOL_tipagem;
													char ancora2[] = "tipagem";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 4055 "test.tab.c"
    break;

  case 67: /* inclusao: ADD OPENPAR pertinencia CLOSEPAR  */
#line 1679 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[-1].node);
													(*ancora).tipo = YYSYMBOL_inclusao;
													char ancora2[] = "inclusao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 4076 "test.tab.c"
    break;

  case 68: /* remocao: REMOVE OPENPAR pertinencia CLOSEPAR  */
#line 1698 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[-1].node);
													(*ancora).tipo = YYSYMBOL_remocao;
													char ancora2[] = "remocao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 4097 "test.tab.c"
    break;

  case 69: /* selecao: EXISTS OPENPAR pertinencia CLOSEPAR  */
#line 1718 "test.y"
                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = (yyvsp[-1].node);
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "iteracao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													(yyval.node) = ancora;
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 4118 "test.tab.c"
    break;

  case 70: /* $@6: %empty  */
#line 1738 "test.y"
                                                                                                                                                        {
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				
																			}
#line 4128 "test.tab.c"
    break;

  case 71: /* iteracao: FORALL OPENPAR pertinencia CLOSEPAR OPENCURLY $@6 statement CLOSECURLY  */
#line 1743 "test.y"
                                                                                                                                {
		 																		(yyvsp[-7].text) = NULL;
																				(yyvsp[-6].text) = NULL;
																				(yyvsp[-4].text) = NULL;
																				(yyvsp[-3].text) = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 2;
																				(*ancora).filhos[0] = (yyvsp[-5].node);
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "bracket";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).refereTabela = NULL;
																				(*ancora).valor = NULL;
																				(*ancora).filhos[1] = (yyvsp[-1].node);
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				(yyvsp[0].text) = NULL;
																				Pop(pilhaEscopo);
																				(yyval.node) = ancora;
																			}
#line 4153 "test.tab.c"
    break;

  case 72: /* iteracao: FORALL OPENPAR pertinencia CLOSEPAR single_line_statement  */
#line 1763 "test.y"
                                                                                                {
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 2;
																				(*ancora).filhos[0] = (yyvsp[-2].node);
																				(*ancora).filhos[1] = (yyvsp[0].node);
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "semicolon";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).refereTabela = NULL;
																				(*ancora).valor = NULL;
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				(yyval.node) = ancora;
																				(yyvsp[-4].text) = NULL;
																				(yyvsp[-3].text) = NULL;
																				(yyvsp[-1].text) = NULL;
																			}
#line 4175 "test.tab.c"
    break;

  case 73: /* $@7: %empty  */
#line 1781 "test.y"
                                                                                                                                                        {
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				
																			}
#line 4185 "test.tab.c"
    break;

  case 74: /* iteracao: FORALL OPENPAR error CLOSEPAR OPENCURLY $@7 statement CLOSECURLY  */
#line 1786 "test.y"
                                                                                                                                {
		 																		(yyvsp[-7].text) = NULL;
																				(yyvsp[-6].text) = NULL;
																				(yyvsp[-4].text) = NULL;
																				(yyvsp[-3].text) = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).filhos[0] = (yyvsp[-1].node);
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "ERROR";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).refereTabela = NULL;
																				(*ancora).valor = NULL;
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				(yyvsp[0].text) = NULL;
																				Pop(pilhaEscopo);
																				(yyval.node) = ancora;
																				errorCheck = TRUE;
																				yyerrok;
																			}
#line 4211 "test.tab.c"
    break;

  case 75: /* iteracao: FORALL OPENPAR error CLOSEPAR single_line_statement  */
#line 1807 "test.y"
                                                                                                {
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).filhos[0] = (yyvsp[0].node);
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "ERROR SINGLE";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).refereTabela = NULL;
																				(*ancora).valor = NULL;
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				(yyval.node) = ancora;
																				(yyvsp[-4].text) = NULL;
																				(yyvsp[-3].text) = NULL;
																				(yyvsp[-1].text) = NULL;
																				errorCheck = TRUE;
																				yyerrok;
																			}
#line 4234 "test.tab.c"
    break;

  case 76: /* function_call: ID OPENPAR args CLOSEPAR  */
#line 1828 "test.y"
                                                                                                                                {
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).filhos[0] = (yyvsp[-1].node);
																				(*ancora).tipo = YYSYMBOL_function_call;
																				char ancora2[] = "function_call";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).valor = strdup((yyvsp[-3].text));
																				simbolo *ancoraSimb = VerificarEscopo((yyvsp[-3].text));
																				if(ancoraSimb != NULL){ 
																					(*ancora).refereTabela = ancoraSimb;
																					(*ancora).tipoVirtual = (*ancoraSimb).returnType;
																				}
																				else{
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[-3].text),linhaCount,colunaCount);
																					(*ancora).refereTabela = NULL;
																					(*ancora).tipoVirtual = 0;
																					errorCheck = TRUE;
																				}
																				int calledArgs = ContaCallArgs((yyvsp[-1].node));
																				if((*ancora).refereTabela->numArgs != calledArgs){
																					printf("ERRO SEMANTICO! CHAMADA DE FUNCAO %s USA QUANTIDADE ERRADA DE ARGUMENTOS! Linha: %d, Coluna %d\n",(yyvsp[-3].text),linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				if(!VerificaTipoArgs((yyvsp[-1].node),(*ancoraSimb).funcArgsTypes,0)){
																					printf("ERRO SEMANTICO! CHAMADA DE FUNCAO %s USA ARGUMENTOS DE TIPOS ERRADOS! Linha: %d, Coluna %d\n",(yyvsp[-3].text),linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				(*ancora).conversion = None;
																				(yyval.node) = ancora;
																				(yyvsp[-3].text) = NULL;
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[0].text) = NULL;
																			}
#line 4273 "test.tab.c"
    break;

  case 77: /* function_call: ID OPENPAR CLOSEPAR  */
#line 1862 "test.y"
                                                                                                                                        {
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 0;
																				(*ancora).tipo = YYSYMBOL_function_call;
																				char ancora2[] = "function_call";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).valor = strdup((yyvsp[-2].text));
																				simbolo *ancoraSimb = VerificarEscopo((yyvsp[-2].text));
																				if(ancoraSimb != NULL){ 
																					(*ancora).refereTabela = ancoraSimb;
																					(*ancora).tipoVirtual = (*ancoraSimb).returnType;
																				}
																				else{
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[-2].text),linhaCount,colunaCount);
																					(*ancora).refereTabela = NULL;
																					(*ancora).tipoVirtual = 0;
																					errorCheck = TRUE;
																				}
																				if((*ancoraSimb).numArgs != 0){
																					printf("ERRO SEMANTICO! CHAMADA DE FUNCAO %s USA QUANTIDADE ERRADA DE ARGUMENTOS! Linha: %d, Coluna %d\n",(yyvsp[-2].text),linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				(*ancora).conversion = None;
																				(yyval.node) = ancora;
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[-1].text) = NULL;
																				(yyvsp[0].text) = NULL;
																			}
#line 4306 "test.tab.c"
    break;

  case 78: /* args: mathop args1  */
#line 1900 "test.y"
                                        {
								no* ancora = (no*)malloc(sizeof(no));
								(*ancora).numFilhos = 2;
								(*ancora).filhos[0] = (yyvsp[-1].node);
								(*ancora).filhos[1] = (yyvsp[0].node);
								(*ancora).tipo = YYSYMBOL_args;
								char ancora2[] = "mathop";
								(*ancora).nome = strdup(ancora2);
								(*ancora).refereTabela = NULL;
								(*ancora).valor = strdup(((yyvsp[-1].node))->valor);
								(*ancora).conversion = None;
								(*ancora).tipoVirtual = ((yyvsp[-1].node))->tipoVirtual;
								(yyval.node) = ancora;
							}
#line 4325 "test.tab.c"
    break;

  case 79: /* args: error args1  */
#line 1941 "test.y"
                                        {	
								no* ancora = (no*)malloc(sizeof(no));
								(*ancora).numFilhos = 1;
								(*ancora).filhos[0] = (yyvsp[0].node);
								(*ancora).tipo = YYSYMBOL_args;
								char ancora2[] = "ERROR";
								(*ancora).nome = strdup(ancora2);
								(*ancora).refereTabela = NULL;
								(*ancora).conversion = None;
								(*ancora).tipoVirtual = 0;
								(yyval.node) = ancora;
								errorCheck = TRUE;
								yyerrok;
							}
#line 4344 "test.tab.c"
    break;

  case 80: /* args1: COMMA args  */
#line 1958 "test.y"
                                        {
							no* ancora = (no*)malloc(sizeof(no));
							(*ancora).numFilhos = 1;
							(*ancora).filhos[0] = (yyvsp[0].node);
							(*ancora).tipo = YYSYMBOL_args1;
							char ancora2[] = "comma";
							(*ancora).nome = strdup(ancora2);
							(*ancora).refereTabela = NULL;
							(*ancora).valor = NULL;
							(*ancora).conversion = None;
							(*ancora).tipoVirtual = 0;
							(yyval.node) = ancora;
							(yyvsp[-1].text) = NULL;
						}
#line 4363 "test.tab.c"
    break;

  case 81: /* args1: %empty  */
#line 1972 "test.y"
                                        {
							no* ancora = (no*)malloc(sizeof(no));
							(*ancora).numFilhos = 0;
							char ancora2[] = "epsilon";
							(*ancora).nome = strdup(ancora2);
							(*ancora).tipo = YYSYMBOL_args1;
							(*ancora).refereTabela = NULL;
							(*ancora).valor = NULL;
							(*ancora).conversion = None;
							(*ancora).tipoVirtual = 0;
							(yyval.node) = ancora;
						}
#line 4380 "test.tab.c"
    break;

  case 82: /* funcargs: type ID  */
#line 1987 "test.y"
                                                        {
									no* ancora = (no*)malloc(sizeof(no));
									(*ancora).numFilhos = 1;
									(*ancora).filhos[0] = (yyvsp[-1].node);
									(*ancora).tipo = YYSYMBOL_funcargs;
									char ancora2[] = "single";
									(*ancora).nome = strdup(ancora2);
									//simbolo *ancoraSimb = VerificarEscopo($2);
									(*ancora).valor = strdup((yyvsp[0].text));
									(*ancora).refereTabela = CriarSimbolo((yyvsp[0].text),atoi(((no*)(yyvsp[-1].node))->valor),NULL,escopoCounter);
									(*ancora).refereTabela->ordemFuncArgs = funcArgsCounter;
									(*ancora).conversion = None;
									(*ancora).tipoVirtual = 0;
									free((yyvsp[0].text));
									(yyval.node) = ancora;
								}
#line 4401 "test.tab.c"
    break;

  case 83: /* funcargs: type ID COMMA funcargs  */
#line 2003 "test.y"
                                        {
									no* ancora = (no*)malloc(sizeof(no));
									(*ancora).numFilhos = 2;
									(*ancora).filhos[0] = (yyvsp[-3].node);
									(*ancora).filhos[1] = (yyvsp[0].node);
									(*ancora).tipo = YYSYMBOL_funcargs;
									char ancora2[] = "comma";
									(*ancora).nome = strdup(ancora2);
									//printf("\n\nOI %s OI\n\n",$2);
									(*ancora).valor = strdup((yyvsp[-2].text));
									(*ancora).refereTabela = CriarSimbolo((yyvsp[-2].text),atoi(((no*)(yyvsp[-3].node))->valor),NULL,escopoCounter);
									(*ancora).refereTabela->ordemFuncArgs = funcArgsCounter;
									funcArgsCounter++;
									(*ancora).conversion = None;
									(*ancora).tipoVirtual = 0;
									free((yyvsp[-2].text));
									(yyvsp[-1].text) = NULL;
									(yyval.node) = ancora;
								}
#line 4425 "test.tab.c"
    break;

  case 84: /* funcargs: %empty  */
#line 2022 "test.y"
                                                        {
									no* ancora = (no*)malloc(sizeof(no));
									(*ancora).numFilhos = 0;
									char ancora2[] = "epsilon";
									(*ancora).nome = strdup(ancora2);
									(*ancora).tipo = YYSYMBOL_funcargs;
									(*ancora).refereTabela = NULL;
									(*ancora).valor = NULL;
									(*ancora).conversion = None;
									(*ancora).tipoVirtual = 0;
									(yyval.node) = ancora;
								}
#line 4442 "test.tab.c"
    break;

  case 85: /* funcargs: error  */
#line 2034 "test.y"
                                                        {
									no* ancora = (no*)malloc(sizeof(no));
									(*ancora).numFilhos = 0;
									char ancora2[] = "ERROR";
									(*ancora).nome = strdup(ancora2);
									(*ancora).tipo = YYSYMBOL_funcargs;
									(*ancora).refereTabela = NULL;
									(*ancora).valor = NULL;
									(*ancora).conversion = None;
									(*ancora).tipoVirtual = 0;
									(yyval.node) = ancora;
									errorCheck = TRUE;
									yyerrok;
								}
#line 4461 "test.tab.c"
    break;

  case 86: /* $@8: %empty  */
#line 2053 "test.y"
                                                                                                                                                        { //Declaração de função é um pouco estranha, pq o escopo da função é diferente dos argumentos
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				funcArgsCounter = 0;
																			}
#line 4471 "test.tab.c"
    break;

  case 87: /* function_declaration: type ID $@8 OPENPAR funcargs CLOSEPAR OPENCURLY statement CLOSECURLY  */
#line 2058 "test.y"
                                                                                                {
																				int numArgumentos;
																				numArgumentos = ContaFuncArgs((yyvsp[-4].node));
																				(yyvsp[-5].text) = NULL;
																				(yyvsp[-3].text) = NULL;
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[0].text) = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				int tipoRetorno = ConverteRetornoTipo((yyvsp[-8].node));
																				int realEscopo;
																				(*ancora).numFilhos = 3;
																				(*ancora).filhos[0] = (yyvsp[-8].node);
																				(*ancora).filhos[1] = (yyvsp[-4].node);
																				(*ancora).filhos[2] = (yyvsp[-1].node);
																				(*ancora).tipo = YYSYMBOL_function_declaration;
																				char ancora2[] = "type";
																				(*ancora).nome = strdup(ancora2);
																				simbolo *ancoraSimb = ProcurarTabela((yyvsp[-7].text));
																				Pop(pilhaEscopo);
																				realEscopo = Top(pilhaEscopo)->valor;
																				if(ancoraSimb != NULL){
																					printf("ERRO SEMANTICO! ID %s REDECLARADO COMO FUNCAO! LINHA: %d, COLUNA: %d \n",(yyvsp[-7].text),linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				else{
																					(*ancora).refereTabela = CriarSimboloFuncao((yyvsp[-7].text),FUNC_TABLE,NULL,realEscopo,tipoRetorno);
																				}
																				if(numArgumentos > 0){
																					ancoraSimb = (*ancora).refereTabela;
																					(*ancoraSimb).funcArgsTypes = (int*)malloc(numArgumentos * sizeof(int));
																					PreencheFuncArgs((yyvsp[-4].node),0,(*ancoraSimb).funcArgsTypes);
																				}
																				(*ancora).refereTabela->numArgs = numArgumentos;
																				(*ancora).valor = strdup((yyvsp[-7].text));
																				free((yyvsp[-7].text));
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				(yyval.node) = ancora;
																			}
#line 4515 "test.tab.c"
    break;

  case 88: /* $@9: %empty  */
#line 2099 "test.y"
                                                                                                                                                        { //Declaração de função é um pouco estranha, pq o escopo da função é diferente dos argumentos
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				funcArgsCounter = 0;
																			}
#line 4525 "test.tab.c"
    break;

  case 89: /* function_declaration: VOID ID $@9 OPENPAR funcargs CLOSEPAR OPENCURLY statement CLOSECURLY  */
#line 2104 "test.y"
                                                                                                {
																				int numArgumentos;
																				numArgumentos = ContaFuncArgs((yyvsp[-4].node));
																				(yyvsp[-8].text) = NULL;
																				(yyvsp[-5].text) = NULL;
																				(yyvsp[-3].text) = NULL;
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[0].text) = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				int realEscopo;
																				(*ancora).numFilhos = 2;
																				(*ancora).filhos[0] = (yyvsp[-4].node);
																				(*ancora).filhos[1] = (yyvsp[-1].node);
																				(*ancora).tipo = YYSYMBOL_function_declaration;
																				char ancora2[] = "void";
																				(*ancora).nome = strdup(ancora2);
																				simbolo *ancoraSimb = ProcurarTabela((yyvsp[-7].text));
																				Pop(pilhaEscopo);
																				realEscopo = Top(pilhaEscopo)->valor;
																				if(ancoraSimb != NULL){
																					printf("ERRO SEMANTICO! ID %s REDECLARADO COMO FUNCAO! LINHA: %d, COLUNA: %d \n",(yyvsp[-7].text),linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				else{
																					(*ancora).refereTabela = CriarSimboloFuncao((yyvsp[-7].text),FUNC_TABLE,NULL,realEscopo,Void);
																				}
																				if(numArgumentos > 0){
																					ancoraSimb = (*ancora).refereTabela;
																					(*ancoraSimb).funcArgsTypes = (int*)malloc(numArgumentos * sizeof(int));
																					PreencheFuncArgs((yyvsp[-4].node),0,(*ancoraSimb).funcArgsTypes);
																				}
																				(*ancora).refereTabela->numArgs = numArgumentos;
																				(*ancora).valor = strdup((yyvsp[-7].text));
																				free((yyvsp[-7].text));
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				(yyval.node) = ancora;
																			}
#line 4568 "test.tab.c"
    break;

  case 90: /* assignment: ID ASSIGN mathop  */
#line 2146 "test.y"
                                                                                                                                        {
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).tipo = YYSYMBOL_assignment;
																				(*ancora).filhos[0] = (yyvsp[0].node);
																				char ancora2[] = "ID";
																				(*ancora).nome = strdup(ancora2);
																				simbolo *ancoraSimb = VerificarEscopo((yyvsp[-2].text));
																				if(ancoraSimb != NULL){ 
																					(*ancora).refereTabela = ancoraSimb;
																					(*ancora).tipoVirtual = ConverteTableTipo((*ancoraSimb).tipo);
																					if(((yyvsp[0].node))->tipoVirtual == (*ancora).tipoVirtual){
																						(*ancora).conversion = None;
																					}
																					else if(((yyvsp[0].node))->tipoVirtual == Float && (*ancora).tipoVirtual == Int){
																						(*ancora).conversion = FloatToIntLeft;
																					}
																					else if(((yyvsp[0].node))->tipoVirtual == Int && (*ancora).tipoVirtual == Float){
																						(*ancora).conversion = IntToFloatLeft;
																					}
																					else if((*ancora).tipoVirtual == Elem){
																						(*ancora).conversion = DecideConversao(Elem,((yyvsp[0].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
																					}
																					else{
																						printf("ERRO SEMANTICO! NAO HA COMO CONVERTER ESTA EXPRESSAO PARA O ID %s! Linha: %d, Coluna: %d\n",(yyvsp[-2].text),linhaCount,colunaCount);
																						(*ancora).conversion = None;
																						(*ancora).tipoVirtual = 0;
																						errorCheck = TRUE;
																					}
																				}
																				else{
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO!\n",(yyvsp[-2].text));
																					(*ancora).refereTabela = NULL;
																					(*ancora).conversion = None;
																					(*ancora).tipoVirtual = 0;
																					errorCheck = TRUE;
																				}
																				(*ancora).valor = strdup((yyvsp[-2].text));
																				free((yyvsp[-2].text));
																				(yyvsp[-1].text) = NULL;
																				(yyval.node) = ancora;
																			}
#line 4615 "test.tab.c"
    break;

  case 91: /* variable_declaration: type ID  */
#line 2191 "test.y"
                                                                                                                                        {
																			no* ancora = (no*)malloc(sizeof(no));
																			(*ancora).numFilhos = 1;
																			(*ancora).filhos[0] = (yyvsp[-1].node);
																			(*ancora).tipo = YYSYMBOL_variable_declaration;
																			char ancora2[] = "variable_declaration";
																			(*ancora).nome = strdup(ancora2);
																			simbolo *ancoraSimb = ProcurarTabelaEscopo((yyvsp[0].text),(Top(pilhaEscopo)->valor));
																			if(ancoraSimb != NULL){
																				printf("ERRO SEMANTICO! VARIAVEL %s REDECLARADA! LINHA: %d, COLUNA: %d\n",(yyvsp[0].text),linhaCount,colunaCount);
																				errorCheck = TRUE;
																			}
																			else{
																				(*ancora).refereTabela = CriarSimbolo((yyvsp[0].text),atoi(((no*)(yyvsp[-1].node))->valor),NULL,escopoCounter);
																			}
																			(*ancora).valor = strdup((yyvsp[0].text));
																			(*ancora).conversion = None;
																			(*ancora).tipoVirtual = 0;
																			free((yyvsp[0].text));
																			(yyval.node) = ancora;
																		}
#line 4641 "test.tab.c"
    break;

  case 92: /* variable_declaration: type error  */
#line 2212 "test.y"
                                                                                                                                                {
																			no* ancora = (no*)malloc(sizeof(no));
																			(*ancora).numFilhos = 1;
																			(*ancora).filhos[0] = (yyvsp[-1].node);
																			char ancora2[] = "ERROR";
																			(*ancora).nome = strdup(ancora2);
																			(*ancora).tipo = YYSYMBOL_variable_declaration;
																			(*ancora).refereTabela = NULL;
																			(*ancora).valor = NULL;
																			(*ancora).conversion = None;
																			(*ancora).tipoVirtual = 0;
																			(yyval.node) = ancora;
																			errorCheck = TRUE;
																			yyerrok;
																		}
#line 4661 "test.tab.c"
    break;

  case 93: /* mathop: mathop PLUS mathop1  */
#line 2234 "test.y"
                                                        {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 2;
										(*ancora).filhos[0] = (yyvsp[-2].node);
										(*ancora).filhos[1] = (yyvsp[0].node);
										char ancora2[] = "plus";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
										(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
										(yyval.node) = ancora;
										(yyvsp[-1].text) = NULL;
									}
#line 4681 "test.tab.c"
    break;

  case 94: /* mathop: mathop MINUS mathop1  */
#line 2249 "test.y"
                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 2;
										(*ancora).filhos[0] = (yyvsp[-2].node);
										(*ancora).filhos[1] = (yyvsp[0].node);
										char ancora2[] = "minus";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
										(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
										(yyval.node) = ancora;
										(yyvsp[-1].text) = NULL;
									}
#line 4701 "test.tab.c"
    break;

  case 95: /* mathop: mathop1  */
#line 2264 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = (yyvsp[0].node);
										char ancora2[] = "mathop1";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
										(yyval.node) = ancora;
									}
#line 4719 "test.tab.c"
    break;

  case 96: /* mathop1: mathop1 AST mathop2  */
#line 2280 "test.y"
                                                        {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 2;
										(*ancora).filhos[0] = (yyvsp[-2].node);
										(*ancora).filhos[1] = (yyvsp[0].node);
										char ancora2[] = "ast";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop1;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).tipoVirtual = DecideTipo(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual);
										(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
										(yyval.node) = ancora;
										(yyvsp[-1].text) = NULL;
									}
#line 4739 "test.tab.c"
    break;

  case 97: /* mathop1: mathop1 BS mathop2  */
#line 2295 "test.y"
                                                        {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 2;
										(*ancora).filhos[0] = (yyvsp[-2].node);
										(*ancora).filhos[1] = (yyvsp[0].node);
										char ancora2[] = "bs";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop1;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).tipoVirtual = Float; //Melhor que toda divisão resulte em um float
										(*ancora).conversion = DecideConversao(((yyvsp[-2].node))->tipoVirtual,((yyvsp[0].node))->tipoVirtual,(*ancora).tipoVirtual);
										(yyval.node) = ancora;
										(yyvsp[-1].text) = NULL;
									}
#line 4759 "test.tab.c"
    break;

  case 98: /* mathop1: mathop2  */
#line 2310 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = (yyvsp[0].node);
										char ancora2[] = "mathop2";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop1;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
										(yyval.node) = ancora;
									}
#line 4777 "test.tab.c"
    break;

  case 99: /* mathop2: matharg  */
#line 2327 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = (yyvsp[0].node);
										char ancora2[] = "matharg";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop2;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
										(yyval.node) = ancora;
									}
#line 4795 "test.tab.c"
    break;

  case 100: /* mathop2: OPENPAR mathop CLOSEPAR  */
#line 2340 "test.y"
                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = (yyvsp[-1].node);
										char ancora2[] = "pars";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop2;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ((yyvsp[-1].node))->tipoVirtual;
										(yyval.node) = ancora;
										(yyvsp[-2].text) = NULL;
										(yyvsp[0].text) = NULL;
									}
#line 4815 "test.tab.c"
    break;

  case 101: /* matharg: ID  */
#line 2358 "test.y"
                                                                        {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "ID";
										(*ancora).nome = strdup(ancora2);
										simbolo *ancoraSimb = VerificarEscopo((yyvsp[0].text));
										if(ancoraSimb != NULL){ 
											(*ancora).refereTabela = ancoraSimb;
										}
										else{
											printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO!\n",(yyvsp[0].text));
											(*ancora).refereTabela = NULL;
											errorCheck = TRUE;
										}
										(*ancora).valor = strdup((yyvsp[0].text));
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ConverteTableTipo((*ancoraSimb).tipo);
										free((yyvsp[0].text));
										(yyval.node) = ancora;																
									}
#line 4841 "test.tab.c"
    break;

  case 102: /* matharg: num  */
#line 2380 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = (yyvsp[0].node);
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "num";
										(*ancora).nome = strdup(ancora2);
										(*ancora).valor = NULL;
										(*ancora).refereTabela = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
										(yyval.node) = ancora;																
									}
#line 4859 "test.tab.c"
    break;

  case 103: /* matharg: function_call  */
#line 2393 "test.y"
                                                        {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = (yyvsp[0].node);
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "function_call";
										(*ancora).nome = strdup(ancora2);
										(*ancora).valor = NULL;
										(*ancora).refereTabela = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ((yyvsp[0].node))->tipoVirtual;
										(yyval.node) = ancora;																
									}
#line 4877 "test.tab.c"
    break;

  case 104: /* matharg: EMPTY  */
#line 2407 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "empty";
										(*ancora).nome = strdup(ancora2);
										(*ancora).valor = strdup((yyvsp[0].text));
										(*ancora).refereTabela = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Set;
										(yyval.node) = ancora;																
									}
#line 4894 "test.tab.c"
    break;

  case 105: /* type: SET  */
#line 2424 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "set";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_type;
										(*ancora).refereTabela = NULL;
										char ancora3[2];
										sprintf(ancora3,"%d",SET_TABLE);
										(*ancora).valor = strdup(ancora3);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Set;
										(yyval.node) = ancora;
										(yyvsp[0].text) = NULL;
									}
#line 4914 "test.tab.c"
    break;

  case 106: /* type: INT  */
#line 2439 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "int";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_type;
										(*ancora).refereTabela = NULL;
										char ancora3[2];
										sprintf(ancora3,"%d",INT_TABLE);
										(*ancora).valor = strdup(ancora3);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Int;
										(yyval.node) = ancora;
										(yyvsp[0].text) = NULL;
									}
#line 4934 "test.tab.c"
    break;

  case 107: /* type: ELEM  */
#line 2454 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "elem";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_type;
										(*ancora).refereTabela = NULL;
										char ancora3[2];
										sprintf(ancora3,"%d",ELEM_TABLE);
										(*ancora).valor = strdup(ancora3);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Elem;
										(yyval.node) = ancora;
										(yyvsp[0].text) = NULL;
									}
#line 4954 "test.tab.c"
    break;

  case 108: /* type: FLOAT  */
#line 2469 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "float";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_type;
										(*ancora).refereTabela = NULL;
										char ancora3[2];
										sprintf(ancora3,"%d",FLOAT_TABLE);
										(*ancora).valor = strdup(ancora3);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Float;
										(yyval.node) = ancora;
										(yyvsp[0].text) = NULL;
									}
#line 4974 "test.tab.c"
    break;

  case 109: /* num: INTEGER  */
#line 2487 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "integer";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_num;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = strdup((yyvsp[0].text));
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Int;
										(yyval.node) = ancora;
										free((yyvsp[0].text));
									}
#line 4992 "test.tab.c"
    break;

  case 110: /* num: FLOATING  */
#line 2501 "test.y"
                                                                {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "floating";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_num;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = strdup((yyvsp[0].text));
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Float;
										(yyval.node) = ancora;
										free((yyvsp[0].text));
									}
#line 5010 "test.tab.c"
    break;


#line 5014 "test.tab.c"

        default: break;
      }
    if (yychar_backup != yychar)
      YY_LAC_DISCARD ("yychar change");
  }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yyesa, &yyes, &yyes_capacity, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        if (yychar != YYEMPTY)
          YY_LAC_ESTABLISH;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          goto yyexhaustedlab;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  /* If the stack popping above didn't lose the initial context for the
     current lookahead token, the shift below will for sure.  */
  YY_LAC_DISCARD ("error recovery");

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if 1
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yyes != yyesa)
    YYSTACK_FREE (yyes);
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 2517 "test.y"

/*
void yyerror(char *s){
	extern int yylineno;
	extern char *yytext;

	printf("ERRO %s NO SIMBOLO <%s> na linha <%d>\n",s,yytext,yylineno);
	exit(1);
}
*/

void yyerror(char const *s){
	printf("Linha: %d, Coluna: %d -> %s\n",linhaCount,colunaCount,s);

}


/*int yyerror(char *s){
	return yyerror(s);
}
*/
int main(int argc, char **argv){
	topo = tabelaSimbolos;
	//raiz = (no*)malloc(sizeof(no));
	//variavelRaiz = TRUE;
	++argv;
	--argc;//pula o nome do programa
	escopoCounter = 0;
	funcArgsCounter = 0;
	errorCheck = FALSE;
	pilhaEscopo = CriarStack(0);
	if (argc > 0){
		yyin = fopen(argv[0],"r");
	}
	else{
		yyin = stdin;
	}
	yyparse();
	fclose(yyin);
	EscreverTabela();
	printf("--------------------ARVORE SINTATICA-------------------\n");
	EscreverArvore(raiz,1);
	//yylex_destroy();
	if(errorCheck == TRUE){
		printf("Houve erro na compilacao! Nao sera gerado o arquivo TAC!\n");
	}
	else{
		ConverterTac();
	}
	ApagarTabela();
	LimparStack(pilhaEscopo);
	
	yylex_destroy();
	return 0;
}

