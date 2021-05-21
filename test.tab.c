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
no* ancoraGlobalNo;

int escopoCounter;
int funcArgsCounter;
int dentroDeclaraFunc;
int textCounter;


int VerificaTipoArgs(no* alvo, int* vector, int profund){
	int result;
	no* next;

	result = (vector[profund] == (*alvo).tipoVirtual);

	if(!strcmp((*alvo).filhos[1]->nome,"comma")){
		next = (*alvo).filhos[1]->filhos[1];
		return result && VerificaTipoArgs(next,vector,profund+1);
	}
	else {
		return result;
	}
	
}


void PreencheFuncArgs(no* alvo, int num, int* vector, char** names){
	if(!strcmp((*alvo).nome,"epsilon")) return;
	else{
		names[num] = strdup((*alvo).valor);
		vector[num] = (*alvo).filhos[0]->tipoVirtual;
		if (strcmp((*alvo).nome,"single")){
			PreencheFuncArgs((*alvo).filhos[1],num + 1,vector,names);
		}
	
	} 
	
}

int ContaCallArgs(no* alvo){
	if(!strcmp((*alvo).nome,"mathop")){
		return 1 + ContaCallArgs((*alvo).filhos[1]);
	}
	else{
		if(!strcmp((*alvo).nome,"comma")){
			return ContaCallArgs((*alvo).filhos[0]);
		}
		else{
			return 0;
		}
	}
	return 0;
}

int ContaFuncArgs(no* alvo){
	if(!strcmp((*alvo).nome,"single")){
		return 1;
	} 
	else if (!strcmp((*alvo).nome,"comma")){
		return 1 + ContaFuncArgs((*alvo).filhos[1]);
	} 
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

simbolo* CriarSimboloFuncao(char* nome, int tipo, char* valor, int escopo, int escopoArgs, int retorno){
	simbolo* ancora = CriarSimbolo(nome,tipo,valor,escopo);
	(*ancora).returnType = retorno;
	(*ancora).escopoArgs = escopoArgs;
	return ancora;
}
	
simbolo* RemoverSimbolo(simbolo* alvo){//retorna ponteiro para o próximo simbolo caso esteja removendo de algum lugar que não seja o fim da tabela
	int i;
	simbolo *ancora = (*alvo).seguinte;
	free((*alvo).nome);
	free((*alvo).valor);
	if((*alvo).numArgs > 0){
		free((*alvo).funcArgsTypes);
		for(i = 0;i < (*alvo).numArgs;i++){
			free((*alvo).funcArgs[i]);
		}
		free((*alvo).funcArgs);
	} 
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

void PrintLines(int profund){
	int j;
	for(j = 0;j < profund;j++){
		printf("-");
	}	
}

void EscreveConversao(no* argumento){
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
	pilha* ancora = Top(stack);
	pilha* ancora2;
	ancora2 = (*ancora).anterior;
	(*ancora2).seguinte = NULL;
	free(ancora);
}

void Push(pilha* stack, pilha* argumento){
	pilha* ancora = Top(stack);
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


#line 540 "test.tab.c"

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
  YYSYMBOL_87_10 = 87,                     /* $@10  */
  YYSYMBOL_88_11 = 88,                     /* $@11  */
  YYSYMBOL_assignment = 89,                /* assignment  */
  YYSYMBOL_variable_declaration = 90,      /* variable_declaration  */
  YYSYMBOL_mathop = 91,                    /* mathop  */
  YYSYMBOL_mathop1 = 92,                   /* mathop1  */
  YYSYMBOL_mathop2 = 93,                   /* mathop2  */
  YYSYMBOL_matharg = 94,                   /* matharg  */
  YYSYMBOL_type = 95,                      /* type  */
  YYSYMBOL_num = 96                        /* num  */
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
#define YYFINAL  75
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   498

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  114
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  237

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
       0,   589,   589,   604,   622,   621,   647,   665,   682,   699,
     716,   735,   754,   772,   788,   804,   820,   848,   864,   910,
     925,   956,   982,   997,  1012,  1027,  1042,  1057,  1072,  1087,
    1105,  1133,  1150,  1168,  1191,  1208,  1225,  1250,  1265,  1280,
    1297,  1297,  1333,  1332,  1359,  1378,  1377,  1405,  1427,  1441,
    1455,  1455,  1476,  1491,  1504,  1517,  1530,  1543,  1559,  1572,
    1585,  1598,  1611,  1624,  1649,  1664,  1682,  1701,  1720,  1740,
    1760,  1759,  1785,  1803,  1802,  1829,  1850,  1890,  1930,  1971,
    1988,  2002,  2017,  2033,  2052,  2064,  2083,  2095,  2082,  2146,
    2158,  2145,  2211,  2256,  2277,  2299,  2314,  2329,  2345,  2360,
    2375,  2392,  2405,  2423,  2446,  2459,  2473,  2490,  2505,  2520,
    2535,  2553,  2567,  2580,  2597
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
  "function_declaration", "$@8", "$@9", "$@10", "$@11", "assignment",
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

#define YYPACT_NINF (-172)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-94)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     138,   -34,  -172,  -172,  -172,    -5,  -172,  -172,  -172,  -172,
      -3,    11,    24,    59,    61,    84,    94,    97,   109,   111,
       6,   425,  -172,    75,   132,   143,  -172,    80,   120,   126,
     131,   149,    80,    80,   151,  -172,  -172,  -172,  -172,   166,
      80,  -172,    80,   155,   158,    37,    86,  -172,  -172,    52,
    -172,  -172,   340,   425,    16,   180,   185,    23,   411,   382,
     419,   419,   419,   368,   186,     6,   435,   451,  -172,    87,
     148,   198,  -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,   382,  -172,  -172,  -172,    80,   382,
     425,   425,   425,   425,  -172,   187,   181,  -172,   208,   140,
     130,   217,   218,   130,   211,   216,   236,   238,   239,   190,
     240,   241,   203,   107,   243,  -172,  -172,  -172,  -172,   166,
     244,   166,   248,   253,   260,   262,   264,   148,  -172,   435,
     435,   435,   435,   435,   435,   435,   435,  -172,   263,   267,
    -172,  -172,  -172,    86,    86,  -172,  -172,   268,   374,  -172,
    -172,  -172,   256,   282,     6,  -172,  -172,  -172,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,   308,   334,  -172,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,  -172,   171,   171,
    -172,  -172,   283,  -172,   283,   251,  -172,  -172,  -172,  -172,
    -172,   274,   298,   286,   198,   230,  -172,   198,  -172,   180,
     198,   198,  -172,   271,  -172,   288,  -172,  -172,  -172,   289,
     292,   291,   293,   295,   171,   299,   283,   198,   283,  -172,
    -172,  -172,   198,  -172,   198,  -172,   303,  -172,   311,   316,
     320,  -172,   198,  -172,  -172,   324,  -172
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,   111,   112,   106,   103,   108,   110,   109,   107,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      39,     0,     4,     0,     0,     0,     2,     0,     0,     0,
       0,     0,     0,     0,     0,    53,    54,    55,    56,    57,
       0,   105,     0,     0,     0,     0,    97,   100,   101,     0,
     104,    18,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,     0,     0,    37,    20,
       0,     0,   113,   114,    89,     1,     3,    16,    14,    15,
      12,     7,     8,    17,     0,     9,     6,    13,     0,     0,
       0,     0,     0,     0,    94,    86,    81,    77,     0,    81,
      92,     0,     0,    20,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   103,     0,    58,    59,    60,    61,    62,
       0,     0,     0,     0,     0,     0,     0,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,   102,     0,     0,
      65,    10,    64,    95,    96,    98,    99,     0,     0,    79,
      76,    78,     0,     0,     0,    30,    32,    33,    31,    35,
      36,    34,    66,    67,    68,    69,     0,     0,    19,    26,
      27,    28,    29,    24,    25,    23,    22,     5,     0,     0,
      80,    45,    52,    42,    52,     0,    73,    75,    70,    72,
      85,     0,     0,     0,     0,     0,    47,     0,    44,     0,
       0,     0,    90,    82,    87,     0,    50,    49,    48,     0,
       0,     0,     0,     0,     0,     0,    52,     0,    52,    40,
      74,    71,     0,    83,     0,    46,     0,    43,     0,     0,
       0,    51,     0,    91,    88,     0,    41
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -172,  -172,   -27,  -172,  -101,   -62,   -47,  -172,  -172,  -172,
    -172,  -172,  -172,   163,  -172,  -172,  -159,  -172,  -172,    -7,
     393,   -43,   -26,    40,   406,  -172,  -172,  -172,  -172,   213,
     265,  -171,  -172,  -172,  -172,  -172,  -172,   -54,  -172,   -18,
     101,   147,  -172,   -57,  -172
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    25,    26,    71,    27,    67,    68,    28,    29,    30,
      31,    32,   228,    33,   197,   194,   196,   217,    34,   114,
      35,    36,    37,    38,    39,    40,   201,   200,    41,    98,
     149,   191,    42,   147,   215,   139,   213,    43,    44,    45,
      46,    47,    48,    49,    50
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      76,   105,    69,    70,   128,    81,    82,   102,   193,     2,
       3,    51,     4,    85,    64,    86,   116,   101,   126,     2,
       3,    52,     4,    54,    64,   198,     2,     3,   107,     4,
     108,    64,    65,   117,    99,   100,   103,    55,    53,   109,
     112,   116,    65,   223,   138,    23,   116,   127,   103,    21,
      56,   182,   184,    94,    66,    23,    89,   225,   117,   227,
      95,   141,    23,   117,    66,   187,   189,   169,   170,   171,
     172,   173,   174,   175,   176,    90,    91,   140,    72,    73,
     -11,     1,   142,     2,     3,    57,     4,    58,     5,     6,
       7,     8,     9,    10,   207,    11,    12,    13,    14,   118,
      15,    16,    17,    18,    19,    20,    21,   185,    22,   -11,
      59,   103,   103,   103,   103,   103,   103,   103,   103,    23,
      60,   192,   192,    61,   118,    90,    91,    92,    93,   118,
      99,    24,   -38,    52,   -63,    62,   103,    63,   -11,     1,
      74,     2,     3,    75,     4,   210,     5,     6,     7,     8,
       9,    10,   -63,    11,    12,    13,    14,   192,    15,    16,
      17,    18,    19,    20,    21,    77,    22,   205,    90,    91,
     209,    78,   190,   211,   212,   137,    79,    23,    90,    91,
       6,     7,     8,     9,   148,    84,    90,    91,   104,    24,
     226,   143,   144,   106,    80,   229,    83,   230,   -84,     1,
      87,     2,     3,    88,     4,   235,     5,     6,     7,     8,
       9,    10,    52,    11,    12,    13,    14,   158,    15,    16,
      17,    18,    19,    20,    21,   148,    22,   -11,    90,    91,
     161,     1,   -93,     2,     3,   150,     4,    23,     5,   145,
     146,    90,    91,    10,   152,   153,    12,    13,    14,    24,
      15,    16,    17,    18,    53,    20,    21,     1,   206,     2,
       3,   154,     4,   155,     5,   156,   157,   159,   160,    23,
     162,   163,    12,    13,    14,   164,    15,    16,    17,    18,
     165,    20,    21,     1,   181,     2,     3,   166,     4,   167,
       5,   168,   177,   178,   179,    23,   199,   195,    12,    13,
      14,   202,    15,    16,    17,    18,   203,    20,    21,     1,
     183,     2,     3,   204,     4,   214,     5,   216,   218,   219,
     220,    23,   221,   222,    12,    13,    14,   224,    15,    16,
      17,    18,   231,    20,    21,     1,   186,     2,     3,   232,
       4,    96,     5,     2,     3,   233,     4,    23,    64,   234,
      12,    13,    14,   236,    15,    16,    17,    18,   208,    20,
      21,   180,   188,     0,   151,     0,    21,    97,     0,   124,
       0,     2,     3,    23,     4,    96,    64,     2,     3,    23,
       4,     0,    64,     0,     0,     2,     3,     0,     4,     0,
     113,    18,     0,     0,    21,     0,     0,     0,     0,     0,
      21,     0,    15,    16,    17,    18,     0,    23,    21,     0,
       0,     0,     0,    23,     2,     3,   110,     4,   111,    64,
       0,    23,     2,     3,     0,     4,     0,    64,     2,     3,
       0,     4,     0,    64,     0,     0,     0,    21,     2,     3,
       0,     4,    18,    64,     0,    21,     0,     0,     0,     0,
      23,    21,   115,   120,   122,   123,   125,     0,    23,     0,
       0,    65,     0,     0,    23,   119,   121,   121,   121,   121,
       0,     0,     0,     0,    23,     0,     0,   115,     0,     0,
       0,     0,   115,   129,   130,   131,   132,   133,   134,     0,
     119,     0,     0,     0,     0,   119,     0,   135,   136
};

static const yytype_int16 yycheck[] =
{
      27,    55,    20,    21,    66,    32,    33,    54,   179,     3,
       4,    45,     6,    40,     8,    42,    59,     1,    65,     3,
       4,    26,     6,    26,     8,   184,     3,     4,     5,     6,
       7,     8,    26,    59,    52,    53,    54,    26,    43,    57,
      58,    84,    26,   214,    71,    39,    89,    65,    66,    26,
      26,   152,   153,     1,    48,    39,    19,   216,    84,   218,
       8,    88,    39,    89,    48,   166,   167,   129,   130,   131,
     132,   133,   134,   135,   136,    38,    39,    84,     3,     4,
       0,     1,    89,     3,     4,    26,     6,    26,     8,     9,
      10,    11,    12,    13,   195,    15,    16,    17,    18,    59,
      20,    21,    22,    23,    24,    25,    26,   154,    28,    29,
      26,   129,   130,   131,   132,   133,   134,   135,   136,    39,
      26,   178,   179,    26,    84,    38,    39,    41,    42,    89,
     148,    51,    45,    26,    27,    26,   154,    26,     0,     1,
       8,     3,     4,     0,     6,   199,     8,     9,    10,    11,
      12,    13,    45,    15,    16,    17,    18,   214,    20,    21,
      22,    23,    24,    25,    26,    45,    28,   194,    38,    39,
     197,    45,     1,   200,   201,    27,    45,    39,    38,    39,
       9,    10,    11,    12,    44,    19,    38,    39,     8,    51,
     217,    90,    91,     8,    45,   222,    45,   224,    27,     1,
      45,     3,     4,    45,     6,   232,     8,     9,    10,    11,
      12,    13,    26,    15,    16,    17,    18,    27,    20,    21,
      22,    23,    24,    25,    26,    44,    28,    29,    38,    39,
      27,     1,    45,     3,     4,    27,     6,    39,     8,    92,
      93,    38,    39,    13,    27,    27,    16,    17,    18,    51,
      20,    21,    22,    23,    43,    25,    26,     1,    28,     3,
       4,    45,     6,    27,     8,    27,    27,    27,    27,    39,
      27,    27,    16,    17,    18,    27,    20,    21,    22,    23,
      27,    25,    26,     1,    28,     3,     4,    27,     6,    27,
       8,    27,    29,    26,    26,    39,    45,    14,    16,    17,
      18,    27,    20,    21,    22,    23,     8,    25,    26,     1,
      28,     3,     4,    27,     6,    44,     8,    29,    29,    27,
      29,    39,    29,    28,    16,    17,    18,    28,    20,    21,
      22,    23,    29,    25,    26,     1,    28,     3,     4,    28,
       6,     1,     8,     3,     4,    29,     6,    39,     8,    29,
      16,    17,    18,    29,    20,    21,    22,    23,   195,    25,
      26,   148,    28,    -1,    99,    -1,    26,    27,    -1,     1,
      -1,     3,     4,    39,     6,     1,     8,     3,     4,    39,
       6,    -1,     8,    -1,    -1,     3,     4,    -1,     6,    -1,
       8,    23,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,
      26,    -1,    20,    21,    22,    23,    -1,    39,    26,    -1,
      -1,    -1,    -1,    39,     3,     4,     5,     6,     7,     8,
      -1,    39,     3,     4,    -1,     6,    -1,     8,     3,     4,
      -1,     6,    -1,     8,    -1,    -1,    -1,    26,     3,     4,
      -1,     6,    23,     8,    -1,    26,    -1,    -1,    -1,    -1,
      39,    26,    59,    60,    61,    62,    63,    -1,    39,    -1,
      -1,    26,    -1,    -1,    39,    59,    60,    61,    62,    63,
      -1,    -1,    -1,    -1,    39,    -1,    -1,    84,    -1,    -1,
      -1,    -1,    89,    32,    33,    34,    35,    36,    37,    -1,
      84,    -1,    -1,    -1,    -1,    89,    -1,    46,    47
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     4,     6,     8,     9,    10,    11,    12,
      13,    15,    16,    17,    18,    20,    21,    22,    23,    24,
      25,    26,    28,    39,    51,    53,    54,    56,    59,    60,
      61,    62,    63,    65,    70,    72,    73,    74,    75,    76,
      77,    80,    84,    89,    90,    91,    92,    93,    94,    95,
      96,    45,    26,    43,    26,    26,    26,    26,    26,    26,
      26,    26,    26,    26,     8,    26,    48,    57,    58,    91,
      91,    55,     3,     4,     8,     0,    54,    45,    45,    45,
      45,    54,    54,    45,    19,    54,    54,    45,    45,    19,
      38,    39,    41,    42,     1,     8,     1,    27,    81,    91,
      91,     1,    58,    91,     8,    89,     8,     5,     7,    91,
       5,     7,    91,     8,    71,    72,    73,    74,    75,    76,
      72,    76,    72,    72,     1,    72,    58,    91,    57,    32,
      33,    34,    35,    36,    37,    46,    47,    27,    54,    87,
      71,    54,    71,    92,    92,    93,    93,    85,    44,    82,
      27,    82,    27,    27,    45,    27,    27,    27,    27,    27,
      27,    27,    27,    27,    27,    27,    27,    27,    27,    57,
      57,    57,    57,    57,    57,    57,    57,    29,    26,    26,
      81,    28,    56,    28,    56,    58,    28,    56,    28,    56,
       1,    83,    95,    83,    67,    14,    68,    66,    68,    45,
      79,    78,    27,     8,    27,    54,    28,    56,    65,    54,
      89,    54,    54,    88,    44,    86,    29,    69,    29,    27,
      29,    29,    28,    83,    28,    68,    54,    68,    64,    54,
      54,    29,    28,    29,    29,    54,    29
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
      82,    82,    83,    83,    83,    83,    85,    86,    84,    87,
      88,    84,    89,    90,    90,    91,    91,    91,    92,    92,
      92,    93,    93,    94,    94,    94,    94,    95,    95,    95,
      95,    96,    96,    96,    96
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
       2,     0,     2,     4,     0,     1,     0,     0,    10,     0,
       0,    10,     3,     2,     2,     3,     3,     1,     3,     3,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2
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
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2028 "test.tab.c"
        break;

    case YYSYMBOL_FLOATING: /* FLOATING  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2034 "test.tab.c"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2040 "test.tab.c"
        break;

    case YYSYMBOL_EMPTY: /* EMPTY  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2046 "test.tab.c"
        break;

    case YYSYMBOL_CHAR: /* CHAR  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2052 "test.tab.c"
        break;

    case YYSYMBOL_ID: /* ID  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2058 "test.tab.c"
        break;

    case YYSYMBOL_INT: /* INT  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2064 "test.tab.c"
        break;

    case YYSYMBOL_FLOAT: /* FLOAT  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2070 "test.tab.c"
        break;

    case YYSYMBOL_ELEM: /* ELEM  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2076 "test.tab.c"
        break;

    case YYSYMBOL_SET: /* SET  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2082 "test.tab.c"
        break;

    case YYSYMBOL_IF: /* IF  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2088 "test.tab.c"
        break;

    case YYSYMBOL_ELSE: /* ELSE  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2094 "test.tab.c"
        break;

    case YYSYMBOL_FOR: /* FOR  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2100 "test.tab.c"
        break;

    case YYSYMBOL_READ: /* READ  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2106 "test.tab.c"
        break;

    case YYSYMBOL_WRITE: /* WRITE  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2112 "test.tab.c"
        break;

    case YYSYMBOL_WRITELN: /* WRITELN  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2118 "test.tab.c"
        break;

    case YYSYMBOL_IN: /* IN  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2124 "test.tab.c"
        break;

    case YYSYMBOL_IS_SET: /* IS_SET  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2130 "test.tab.c"
        break;

    case YYSYMBOL_ADD: /* ADD  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2136 "test.tab.c"
        break;

    case YYSYMBOL_REMOVE: /* REMOVE  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2142 "test.tab.c"
        break;

    case YYSYMBOL_EXISTS: /* EXISTS  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2148 "test.tab.c"
        break;

    case YYSYMBOL_FORALL: /* FORALL  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2154 "test.tab.c"
        break;

    case YYSYMBOL_RETURN: /* RETURN  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2160 "test.tab.c"
        break;

    case YYSYMBOL_OPENPAR: /* OPENPAR  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2166 "test.tab.c"
        break;

    case YYSYMBOL_CLOSEPAR: /* CLOSEPAR  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2172 "test.tab.c"
        break;

    case YYSYMBOL_OPENCURLY: /* OPENCURLY  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2178 "test.tab.c"
        break;

    case YYSYMBOL_CLOSECURLY: /* CLOSECURLY  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2184 "test.tab.c"
        break;

    case YYSYMBOL_OPENBRAC: /* OPENBRAC  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2190 "test.tab.c"
        break;

    case YYSYMBOL_CLOSEBRAC: /* CLOSEBRAC  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2196 "test.tab.c"
        break;

    case YYSYMBOL_LESS: /* LESS  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2202 "test.tab.c"
        break;

    case YYSYMBOL_LE: /* LE  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2208 "test.tab.c"
        break;

    case YYSYMBOL_EQ: /* EQ  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2214 "test.tab.c"
        break;

    case YYSYMBOL_NEQ: /* NEQ  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2220 "test.tab.c"
        break;

    case YYSYMBOL_GREATER: /* GREATER  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2226 "test.tab.c"
        break;

    case YYSYMBOL_GE: /* GE  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2232 "test.tab.c"
        break;

    case YYSYMBOL_PLUS: /* PLUS  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2238 "test.tab.c"
        break;

    case YYSYMBOL_MINUS: /* MINUS  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2244 "test.tab.c"
        break;

    case YYSYMBOL_DBS: /* DBS  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2250 "test.tab.c"
        break;

    case YYSYMBOL_AST: /* AST  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2256 "test.tab.c"
        break;

    case YYSYMBOL_BS: /* BS  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2262 "test.tab.c"
        break;

    case YYSYMBOL_ASSIGN: /* ASSIGN  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2268 "test.tab.c"
        break;

    case YYSYMBOL_COMMA: /* COMMA  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2274 "test.tab.c"
        break;

    case YYSYMBOL_SEMICOLON: /* SEMICOLON  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2280 "test.tab.c"
        break;

    case YYSYMBOL_OR: /* OR  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2286 "test.tab.c"
        break;

    case YYSYMBOL_AND: /* AND  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2292 "test.tab.c"
        break;

    case YYSYMBOL_NOT: /* NOT  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2298 "test.tab.c"
        break;

    case YYSYMBOL_AMP: /* AMP  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2304 "test.tab.c"
        break;

    case YYSYMBOL_PCENT: /* PCENT  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2310 "test.tab.c"
        break;

    case YYSYMBOL_VOID: /* VOID  */
#line 566 "test.y"
            {((*yyvaluep).text) = NULL;}
#line 2316 "test.tab.c"
        break;

    case YYSYMBOL_statement: /* statement  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2322 "test.tab.c"
        break;

    case YYSYMBOL_single_line_statement: /* single_line_statement  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2328 "test.tab.c"
        break;

    case YYSYMBOL_comparg: /* comparg  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2334 "test.tab.c"
        break;

    case YYSYMBOL_comparison: /* comparison  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2340 "test.tab.c"
        break;

    case YYSYMBOL_read: /* read  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2346 "test.tab.c"
        break;

    case YYSYMBOL_write: /* write  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2352 "test.tab.c"
        break;

    case YYSYMBOL_writeln: /* writeln  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2358 "test.tab.c"
        break;

    case YYSYMBOL_return: /* return  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2364 "test.tab.c"
        break;

    case YYSYMBOL_for: /* for  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2370 "test.tab.c"
        break;

    case YYSYMBOL_if: /* if  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2376 "test.tab.c"
        break;

    case YYSYMBOL_else: /* else  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2382 "test.tab.c"
        break;

    case YYSYMBOL_conjuntoop: /* conjuntoop  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2388 "test.tab.c"
        break;

    case YYSYMBOL_conjuntoop1: /* conjuntoop1  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2394 "test.tab.c"
        break;

    case YYSYMBOL_pertinencia: /* pertinencia  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2400 "test.tab.c"
        break;

    case YYSYMBOL_tipagem: /* tipagem  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2406 "test.tab.c"
        break;

    case YYSYMBOL_inclusao: /* inclusao  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2412 "test.tab.c"
        break;

    case YYSYMBOL_remocao: /* remocao  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2418 "test.tab.c"
        break;

    case YYSYMBOL_selecao: /* selecao  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2424 "test.tab.c"
        break;

    case YYSYMBOL_iteracao: /* iteracao  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2430 "test.tab.c"
        break;

    case YYSYMBOL_function_call: /* function_call  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2436 "test.tab.c"
        break;

    case YYSYMBOL_args: /* args  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2442 "test.tab.c"
        break;

    case YYSYMBOL_args1: /* args1  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2448 "test.tab.c"
        break;

    case YYSYMBOL_funcargs: /* funcargs  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2454 "test.tab.c"
        break;

    case YYSYMBOL_function_declaration: /* function_declaration  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2460 "test.tab.c"
        break;

    case YYSYMBOL_assignment: /* assignment  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2466 "test.tab.c"
        break;

    case YYSYMBOL_variable_declaration: /* variable_declaration  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2472 "test.tab.c"
        break;

    case YYSYMBOL_mathop: /* mathop  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2478 "test.tab.c"
        break;

    case YYSYMBOL_mathop1: /* mathop1  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2484 "test.tab.c"
        break;

    case YYSYMBOL_mathop2: /* mathop2  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2490 "test.tab.c"
        break;

    case YYSYMBOL_matharg: /* matharg  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2496 "test.tab.c"
        break;

    case YYSYMBOL_type: /* type  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2502 "test.tab.c"
        break;

    case YYSYMBOL_num: /* num  */
#line 566 "test.y"
            {((*yyvaluep).node) = NULL;}
#line 2508 "test.tab.c"
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
#line 589 "test.y"
                                                                                        {
													raiz = (yyvsp[0].node);
													//(*raiz).escopo = 1;
													if(ProcurarTabela("main") == NULL){
														printf("ERRO SEMANTICO! NAO FOI DECLARADA UMA FUNCAO MAIN!\n");
														errorCheck = TRUE;
													}
												}
#line 2799 "test.tab.c"
    break;

  case 3: /* statement: single_line_statement statement  */
#line 604 "test.y"
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
																if(raiz == NULL) raiz = ancora;
															}
#line 2821 "test.tab.c"
    break;

  case 4: /* $@1: %empty  */
#line 622 "test.y"
                                                                                                                        {
																escopoCounter++;
																Push(pilhaEscopo,CriarStack(escopoCounter));
																(yyvsp[0].text) = NULL;
															}
#line 2831 "test.tab.c"
    break;

  case 5: /* statement: OPENCURLY $@1 statement CLOSECURLY  */
#line 627 "test.y"
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
																if(raiz == NULL) raiz = ancora;
															}
#line 2855 "test.tab.c"
    break;

  case 6: /* statement: function_declaration statement  */
#line 647 "test.y"
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
																if(raiz == NULL) raiz = ancora;
															}
#line 2877 "test.tab.c"
    break;

  case 7: /* statement: for statement  */
#line 665 "test.y"
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
																if(raiz == NULL) raiz = ancora;
															}
#line 2899 "test.tab.c"
    break;

  case 8: /* statement: if statement  */
#line 682 "test.y"
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
																if(raiz == NULL) raiz = ancora;
															}
#line 2921 "test.tab.c"
    break;

  case 9: /* statement: iteracao statement  */
#line 699 "test.y"
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
																if(raiz == NULL) raiz = ancora;
															}
#line 2943 "test.tab.c"
    break;

  case 10: /* statement: variable_declaration SEMICOLON statement  */
#line 716 "test.y"
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
																if(raiz == NULL) raiz = ancora;
															}
#line 2966 "test.tab.c"
    break;

  case 11: /* statement: %empty  */
#line 735 "test.y"
                                                                                                                {
																no* ancora = (no*)malloc(sizeof(no));
																(*ancora).numFilhos = 0;
																char ancora2[] = "epsilon";
																(*ancora).nome = strdup(ancora2);
																(*ancora).tipo = YYSYMBOL_statement;
																(*ancora).refereTabela = NULL;
																(*ancora).valor = NULL;
																(*ancora).conversion = None;
																(*ancora).tipoVirtual = 0;
																(*ancora).escopo = 0;
																(yyval.node) = ancora;
																if(raiz == NULL) raiz = ancora;
															}
#line 2985 "test.tab.c"
    break;

  case 12: /* single_line_statement: return SEMICOLON  */
#line 754 "test.y"
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
#line 3006 "test.tab.c"
    break;

  case 13: /* single_line_statement: assignment SEMICOLON  */
#line 772 "test.y"
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
#line 3026 "test.tab.c"
    break;

  case 14: /* single_line_statement: write SEMICOLON  */
#line 788 "test.y"
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
#line 3046 "test.tab.c"
    break;

  case 15: /* single_line_statement: writeln SEMICOLON  */
#line 804 "test.y"
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
#line 3066 "test.tab.c"
    break;

  case 16: /* single_line_statement: read SEMICOLON  */
#line 820 "test.y"
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
#line 3086 "test.tab.c"
    break;

  case 17: /* single_line_statement: conjuntoop SEMICOLON  */
#line 848 "test.y"
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
#line 3106 "test.tab.c"
    break;

  case 18: /* single_line_statement: error SEMICOLON  */
#line 864 "test.y"
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
#line 3126 "test.tab.c"
    break;

  case 19: /* comparg: OPENPAR comparison CLOSEPAR  */
#line 910 "test.y"
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
#line 3146 "test.tab.c"
    break;

  case 20: /* comparg: mathop  */
#line 925 "test.y"
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
#line 3164 "test.tab.c"
    break;

  case 21: /* comparison: NOT comparg  */
#line 956 "test.y"
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
#line 3195 "test.tab.c"
    break;

  case 22: /* comparison: comparg AND comparg  */
#line 982 "test.y"
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
#line 3215 "test.tab.c"
    break;

  case 23: /* comparison: comparg OR comparg  */
#line 997 "test.y"
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
#line 3235 "test.tab.c"
    break;

  case 24: /* comparison: comparg GREATER comparg  */
#line 1012 "test.y"
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
#line 3255 "test.tab.c"
    break;

  case 25: /* comparison: comparg GE comparg  */
#line 1027 "test.y"
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
#line 3275 "test.tab.c"
    break;

  case 26: /* comparison: comparg LESS comparg  */
#line 1042 "test.y"
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
#line 3295 "test.tab.c"
    break;

  case 27: /* comparison: comparg LE comparg  */
#line 1057 "test.y"
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
#line 3315 "test.tab.c"
    break;

  case 28: /* comparison: comparg EQ comparg  */
#line 1072 "test.y"
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
#line 3335 "test.tab.c"
    break;

  case 29: /* comparison: comparg NEQ comparg  */
#line 1087 "test.y"
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
#line 3355 "test.tab.c"
    break;

  case 30: /* read: READ OPENPAR ID CLOSEPAR  */
#line 1105 "test.y"
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
#line 3384 "test.tab.c"
    break;

  case 31: /* write: WRITE OPENPAR mathop CLOSEPAR  */
#line 1133 "test.y"
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
#line 3405 "test.tab.c"
    break;

  case 32: /* write: WRITE OPENPAR STRING CLOSEPAR  */
#line 1150 "test.y"
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
													(*ancora).escopo = textCounter;
													textCounter++;
													(yyval.node) = ancora;
													free((yyvsp[-1].text));
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3428 "test.tab.c"
    break;

  case 33: /* write: WRITE OPENPAR CHAR CLOSEPAR  */
#line 1168 "test.y"
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
													(*ancora).escopo = textCounter;
													textCounter++;
													(yyval.node) = ancora;
													free((yyvsp[-1].text));
													(yyvsp[-3].text) = NULL;
													(yyvsp[-2].text) = NULL;
													(yyvsp[0].text) = NULL;
												}
#line 3451 "test.tab.c"
    break;

  case 34: /* writeln: WRITELN OPENPAR mathop CLOSEPAR  */
#line 1191 "test.y"
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
#line 3472 "test.tab.c"
    break;

  case 35: /* writeln: WRITELN OPENPAR STRING CLOSEPAR  */
#line 1208 "test.y"
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
#line 3493 "test.tab.c"
    break;

  case 36: /* writeln: WRITELN OPENPAR CHAR CLOSEPAR  */
#line 1225 "test.y"
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
#line 3514 "test.tab.c"
    break;

  case 37: /* return: RETURN comparison  */
#line 1250 "test.y"
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
#line 3534 "test.tab.c"
    break;

  case 38: /* return: RETURN mathop  */
#line 1265 "test.y"
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
#line 3554 "test.tab.c"
    break;

  case 39: /* return: RETURN  */
#line 1280 "test.y"
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
#line 3573 "test.tab.c"
    break;

  case 40: /* $@2: %empty  */
#line 1297 "test.y"
                                                                                                                                                        {
																													escopoCounter++;
																													Push(pilhaEscopo,CriarStack(escopoCounter));
																													(yyvsp[-7].text) = NULL;
																													(yyvsp[-6].text) = NULL;
																													(yyvsp[-4].text) = NULL;
																													(yyvsp[-2].text) = NULL;
																													(yyvsp[0].text) = NULL;
																													
																													
																												}
#line 3589 "test.tab.c"
    break;

  case 41: /* for: FOR OPENPAR assignment SEMICOLON comparison SEMICOLON assignment CLOSEPAR $@2 OPENCURLY statement CLOSECURLY  */
#line 1308 "test.y"
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
#line 3614 "test.tab.c"
    break;

  case 42: /* $@3: %empty  */
#line 1333 "test.y"
                                                                                                                                                                                                                        {
																												escopoCounter++;
																												Push(pilhaEscopo,CriarStack(escopoCounter));
																												
																											}
#line 3624 "test.tab.c"
    break;

  case 43: /* if: IF OPENPAR comparison CLOSEPAR OPENCURLY $@3 statement CLOSECURLY else  */
#line 1338 "test.y"
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
#line 3650 "test.tab.c"
    break;

  case 44: /* if: IF OPENPAR comparison CLOSEPAR single_line_statement else  */
#line 1359 "test.y"
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
#line 3673 "test.tab.c"
    break;

  case 45: /* $@4: %empty  */
#line 1378 "test.y"
                                                                                                                                                                                                                        {
																												escopoCounter++;
																												Push(pilhaEscopo,CriarStack(escopoCounter));
																												
																											}
#line 3683 "test.tab.c"
    break;

  case 46: /* if: IF OPENPAR error CLOSEPAR OPENCURLY $@4 statement CLOSECURLY else  */
#line 1383 "test.y"
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
#line 3710 "test.tab.c"
    break;

  case 47: /* if: IF OPENPAR error CLOSEPAR single_line_statement else  */
#line 1405 "test.y"
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
#line 3734 "test.tab.c"
    break;

  case 48: /* else: ELSE if  */
#line 1427 "test.y"
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
														(yyvsp[-1].text) = NULL;
													}
#line 3753 "test.tab.c"
    break;

  case 49: /* else: ELSE single_line_statement  */
#line 1441 "test.y"
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
#line 3772 "test.tab.c"
    break;

  case 50: /* $@5: %empty  */
#line 1455 "test.y"
                                                                                        {
														escopoCounter++;
														Push(pilhaEscopo,CriarStack(escopoCounter));
														(yyvsp[-1].text) = NULL;
														(yyvsp[0].text) = NULL;
													}
#line 3783 "test.tab.c"
    break;

  case 51: /* else: ELSE OPENCURLY $@5 statement CLOSECURLY  */
#line 1461 "test.y"
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
#line 3803 "test.tab.c"
    break;

  case 52: /* else: %empty  */
#line 1476 "test.y"
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
#line 3820 "test.tab.c"
    break;

  case 53: /* conjuntoop: pertinencia  */
#line 1491 "test.y"
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
#line 3838 "test.tab.c"
    break;

  case 54: /* conjuntoop: tipagem  */
#line 1504 "test.y"
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
#line 3856 "test.tab.c"
    break;

  case 55: /* conjuntoop: inclusao  */
#line 1517 "test.y"
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
#line 3874 "test.tab.c"
    break;

  case 56: /* conjuntoop: remocao  */
#line 1530 "test.y"
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
#line 3892 "test.tab.c"
    break;

  case 57: /* conjuntoop: selecao  */
#line 1543 "test.y"
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
#line 3910 "test.tab.c"
    break;

  case 58: /* conjuntoop1: pertinencia  */
#line 1559 "test.y"
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
#line 3928 "test.tab.c"
    break;

  case 59: /* conjuntoop1: tipagem  */
#line 1572 "test.y"
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
#line 3946 "test.tab.c"
    break;

  case 60: /* conjuntoop1: inclusao  */
#line 1585 "test.y"
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
#line 3964 "test.tab.c"
    break;

  case 61: /* conjuntoop1: remocao  */
#line 1598 "test.y"
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
#line 3982 "test.tab.c"
    break;

  case 62: /* conjuntoop1: selecao  */
#line 1611 "test.y"
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
#line 4000 "test.tab.c"
    break;

  case 63: /* conjuntoop1: ID  */
#line 1624 "test.y"
                                                                                                {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "ID";
													(*ancora).nome = strdup(ancora2);
													simbolo *ancoraSimb = VerificarEscopo((yyvsp[0].text));
													if(ancoraSimb != NULL){ 
														(*ancora).refereTabela = ancoraSimb;
														(*ancora).tipoVirtual = (*ancoraSimb).tipo;
													}
													else{
														printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[0].text),linhaCount,colunaCount);
														errorCheck = TRUE;
														(*ancora).refereTabela = NULL;
													}
													(*ancora).valor = strdup((yyvsp[0].text));
													(*ancora).conversion = None;
													
													free((yyvsp[0].text));
													(yyval.node) = ancora;
												}
#line 4027 "test.tab.c"
    break;

  case 64: /* pertinencia: mathop IN conjuntoop1  */
#line 1649 "test.y"
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
#line 4047 "test.tab.c"
    break;

  case 65: /* pertinencia: selecao IN conjuntoop1  */
#line 1664 "test.y"
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
#line 4067 "test.tab.c"
    break;

  case 66: /* tipagem: IS_SET OPENPAR conjuntoop1 CLOSEPAR  */
#line 1682 "test.y"
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
#line 4088 "test.tab.c"
    break;

  case 67: /* inclusao: ADD OPENPAR pertinencia CLOSEPAR  */
#line 1701 "test.y"
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
#line 4109 "test.tab.c"
    break;

  case 68: /* remocao: REMOVE OPENPAR pertinencia CLOSEPAR  */
#line 1720 "test.y"
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
#line 4130 "test.tab.c"
    break;

  case 69: /* selecao: EXISTS OPENPAR pertinencia CLOSEPAR  */
#line 1740 "test.y"
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
#line 4151 "test.tab.c"
    break;

  case 70: /* $@6: %empty  */
#line 1760 "test.y"
                                                                                                                                                        {
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				
																			}
#line 4161 "test.tab.c"
    break;

  case 71: /* iteracao: FORALL OPENPAR pertinencia CLOSEPAR OPENCURLY $@6 statement CLOSECURLY  */
#line 1765 "test.y"
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
#line 4186 "test.tab.c"
    break;

  case 72: /* iteracao: FORALL OPENPAR pertinencia CLOSEPAR single_line_statement  */
#line 1785 "test.y"
                                                                                                {
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 2;
																				(*ancora).filhos[0] = (yyvsp[-2].node);
																				(*ancora).filhos[1] = (yyvsp[0].node);
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "single";
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
#line 4208 "test.tab.c"
    break;

  case 73: /* $@7: %empty  */
#line 1803 "test.y"
                                                                                                                                                        {
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				
																			}
#line 4218 "test.tab.c"
    break;

  case 74: /* iteracao: FORALL OPENPAR error CLOSEPAR OPENCURLY $@7 statement CLOSECURLY  */
#line 1808 "test.y"
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
#line 4244 "test.tab.c"
    break;

  case 75: /* iteracao: FORALL OPENPAR error CLOSEPAR single_line_statement  */
#line 1829 "test.y"
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
#line 4267 "test.tab.c"
    break;

  case 76: /* function_call: ID OPENPAR args CLOSEPAR  */
#line 1850 "test.y"
                                                                                                                                {
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).filhos[0] = (yyvsp[-1].node);
																				int calledArgs;
																				(*ancora).tipo = YYSYMBOL_function_call;
																				char ancora2[] = "function_call";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).valor = strdup((yyvsp[-3].text));
																				simbolo *ancoraSimb = VerificarEscopo((yyvsp[-3].text));
																				if(ancoraSimb != NULL){ 
																					(*ancora).refereTabela = ancoraSimb;
																					(*ancora).tipoVirtual = (*ancoraSimb).returnType;
																					calledArgs = ContaCallArgs((yyvsp[-1].node));

																					if((*ancora).refereTabela->numArgs != calledArgs){
																						printf("ERRO SEMANTICO! CHAMADA DE FUNCAO %s USA QUANTIDADE ERRADA DE ARGUMENTOS! Linha: %d, Coluna %d\n",(yyvsp[-3].text),linhaCount,colunaCount);
																						errorCheck = TRUE;
																					}

																					if(!VerificaTipoArgs((yyvsp[-1].node),(*ancoraSimb).funcArgsTypes,0)){
																						printf("ERRO SEMANTICO! CHAMADA DE FUNCAO %s USA ARGUMENTOS DE TIPOS ERRADOS! Linha: %d, Coluna %d\n",(yyvsp[-3].text),linhaCount,colunaCount);
																						errorCheck = TRUE;
																					}
																				}
																				else{
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[-3].text),linhaCount,colunaCount);
																					(*ancora).refereTabela = NULL;
																					(*ancora).tipoVirtual = 0;
																					errorCheck = TRUE;
																				}
																				
																				
																				
																				(*ancora).conversion = None;
																				(yyval.node) = ancora;
																				free((yyvsp[-3].text));
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[0].text) = NULL;
																			}
#line 4312 "test.tab.c"
    break;

  case 77: /* function_call: ID OPENPAR CLOSEPAR  */
#line 1890 "test.y"
                                                                                                                                        {
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 0;
																				(*ancora).tipo = YYSYMBOL_function_call;
																				char ancora2[] = "no_args";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).valor = strdup((yyvsp[-2].text));
																				simbolo *ancoraSimb = VerificarEscopo((yyvsp[-2].text));
																				if(ancoraSimb != NULL){ 
																					(*ancora).refereTabela = ancoraSimb;
																					(*ancora).tipoVirtual = (*ancoraSimb).returnType;

																					if((*ancoraSimb).numArgs != 0){
																						printf("ERRO SEMANTICO! CHAMADA DE FUNCAO %s USA QUANTIDADE ERRADA DE ARGUMENTOS! Linha: %d, Coluna %d\n",(yyvsp[-2].text),linhaCount,colunaCount);
																						errorCheck = TRUE;
																					}
																				}
																				else{
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[-2].text),linhaCount,colunaCount);
																					(*ancora).refereTabela = NULL;
																					(*ancora).tipoVirtual = 0;
																					errorCheck = TRUE;
																				}
																				
																				(*ancora).conversion = None;
																				(yyval.node) = ancora;
																				free((yyvsp[-2].text));
																				(yyvsp[-1].text) = NULL;
																				(yyvsp[0].text) = NULL;
																			}
#line 4347 "test.tab.c"
    break;

  case 78: /* args: mathop args1  */
#line 1930 "test.y"
                                        {
								no* ancora = (no*)malloc(sizeof(no));
								(*ancora).numFilhos = 2;
								(*ancora).filhos[0] = (yyvsp[-1].node);
								(*ancora).filhos[1] = (yyvsp[0].node);
								(*ancora).tipo = YYSYMBOL_args;
								char ancora2[] = "mathop";
								(*ancora).nome = strdup(ancora2);
								(*ancora).refereTabela = NULL;
								(*ancora).valor = NULL;
								(*ancora).conversion = None;
								(*ancora).tipoVirtual = ((yyvsp[-1].node))->tipoVirtual;
								(yyval.node) = ancora;
							}
#line 4366 "test.tab.c"
    break;

  case 79: /* args: error args1  */
#line 1971 "test.y"
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
#line 4385 "test.tab.c"
    break;

  case 80: /* args1: COMMA args  */
#line 1988 "test.y"
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
#line 4404 "test.tab.c"
    break;

  case 81: /* args1: %empty  */
#line 2002 "test.y"
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
#line 4421 "test.tab.c"
    break;

  case 82: /* funcargs: type ID  */
#line 2017 "test.y"
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
#line 4442 "test.tab.c"
    break;

  case 83: /* funcargs: type ID COMMA funcargs  */
#line 2033 "test.y"
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
#line 4466 "test.tab.c"
    break;

  case 84: /* funcargs: %empty  */
#line 2052 "test.y"
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
#line 4483 "test.tab.c"
    break;

  case 85: /* funcargs: error  */
#line 2064 "test.y"
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
									// ydyerrok;
								}
#line 4502 "test.tab.c"
    break;

  case 86: /* $@8: %empty  */
#line 2083 "test.y"
                                                                                                                                                        { //Declaração de função é um pouco estranha, pq o escopo da função é diferente dos argumentos
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				funcArgsCounter = 0;
																				if(dentroDeclaraFunc){
																					printf("ERRO SEMANTICO! DECLARACAO DE UMA FUNCAO DENTRO DE OUTRA! Linha: %d, Coluna: %d\n",linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				else{
																					dentroDeclaraFunc = TRUE;
																				}
																			}
#line 4519 "test.tab.c"
    break;

  case 87: /* $@9: %empty  */
#line 2095 "test.y"
                                                                                                                                {
																				int numArgumentos = 0;
																				
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[0].text) = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				int tipoRetorno = ConverteRetornoTipo((yyvsp[-5].node));
																				int realEscopo;
																				int escopoArgs;
																				(*ancora).numFilhos = 3;
																				(*ancora).filhos[0] = (yyvsp[-5].node);
																				(*ancora).filhos[1] = (yyvsp[-1].node);
																				(*ancora).tipo = YYSYMBOL_function_declaration;
																				char ancora2[] = "type";
																				(*ancora).nome = strdup(ancora2);
																				realEscopo = Top(pilhaEscopo)->anterior->valor;
																				simbolo *ancoraSimb = ProcurarTabelaEscopo((yyvsp[-4].text),realEscopo);
																				escopoArgs = Top(pilhaEscopo)->valor;
																				if(ancoraSimb != NULL){
																					printf("ERRO SEMANTICO! ID %s REDECLARADO COMO FUNCAO! LINHA: %d, COLUNA: %d \n",(yyvsp[-4].text),linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				else{
																					(*ancora).refereTabela = CriarSimboloFuncao((yyvsp[-4].text),FUNC_TABLE,NULL,realEscopo,escopoArgs,tipoRetorno);
																					numArgumentos = ContaFuncArgs((yyvsp[-1].node));
																					(*ancora).refereTabela->numArgs = numArgumentos;
																				}
																				if(numArgumentos > 0){
																					ancoraSimb = (*ancora).refereTabela;
																					(*ancoraSimb).funcArgsTypes = (int*)malloc(numArgumentos * sizeof(int));
																					(*ancoraSimb).funcArgs = (char**)malloc(numArgumentos * sizeof(char*));
																					PreencheFuncArgs((yyvsp[-1].node),0,(*ancoraSimb).funcArgsTypes,(*ancoraSimb).funcArgs);
																					
																				}
																				(*ancora).valor = strdup((yyvsp[-4].text));
																				free((yyvsp[-4].text));
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				ancoraGlobalNo = ancora;
																			}
#line 4564 "test.tab.c"
    break;

  case 88: /* function_declaration: type ID $@8 OPENPAR funcargs CLOSEPAR $@9 OPENCURLY statement CLOSECURLY  */
#line 2135 "test.y"
                                                                                                                        {
																				Pop(pilhaEscopo);
																				(*ancoraGlobalNo).filhos[2] = (yyvsp[-1].node);
																				dentroDeclaraFunc = FALSE;
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[0].text) = NULL;
																				(yyval.node) = ancoraGlobalNo;
																				ancoraGlobalNo = NULL;
																			}
#line 4578 "test.tab.c"
    break;

  case 89: /* $@10: %empty  */
#line 2146 "test.y"
                                                                                                                                                        { //Declaração de função é um pouco estranha, pq o escopo da função é diferente dos argumentos
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				funcArgsCounter = 0;
																				if(dentroDeclaraFunc){
																					printf("ERRO SEMANTICO! DECLARACAO DE UMA FUNCAO DENTRO DE OUTRA! Linha: %d, Coluna: %d\n",linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				else{
																					dentroDeclaraFunc = TRUE;
																				}
																			}
#line 4595 "test.tab.c"
    break;

  case 90: /* $@11: %empty  */
#line 2158 "test.y"
                                                                                                                                {
																				int numArgumentos = 0;
																				
																				(yyvsp[-5].text) = NULL;
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[0].text) = NULL;
																				
																				no* ancora = (no*)malloc(sizeof(no));
																				ancoraGlobalNo = ancora;
																				int realEscopo;
																				int escopoArgs;
																				(*ancora).numFilhos = 2;
																				(*ancora).filhos[0] = (yyvsp[-1].node);
																				(*ancora).tipo = YYSYMBOL_function_declaration;
																				char ancora2[] = "void";
																				(*ancora).nome = strdup(ancora2);
																				realEscopo = Top(pilhaEscopo)->anterior->valor;
																				simbolo *ancoraSimb = ProcurarTabelaEscopo((yyvsp[-4].text),realEscopo);
																				escopoArgs = Top(pilhaEscopo)->valor;
																				if(ancoraSimb != NULL){
																					printf("ERRO SEMANTICO! ID %s REDECLARADO COMO FUNCAO! LINHA: %d, COLUNA: %d \n",(yyvsp[-4].text),linhaCount,colunaCount);
																					errorCheck = TRUE;
																				}
																				else{
																					(*ancora).refereTabela = CriarSimboloFuncao((yyvsp[-4].text),FUNC_TABLE,NULL,realEscopo,escopoArgs,Void);
																					numArgumentos = ContaFuncArgs((yyvsp[-1].node));
																					(*ancora).refereTabela->numArgs = numArgumentos;
																				}
																				if(numArgumentos > 0){
																					ancoraSimb = (*ancora).refereTabela;
																					(*ancoraSimb).funcArgsTypes = (int*)malloc(numArgumentos * sizeof(int));
																					(*ancoraSimb).funcArgs = (char**)malloc(numArgumentos * sizeof(char*));
																					PreencheFuncArgs((yyvsp[-1].node),0,(*ancoraSimb).funcArgsTypes,(*ancoraSimb).funcArgs);
																				}
																				
																				(*ancora).valor = strdup((yyvsp[-4].text));
																				free((yyvsp[-4].text));
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																			}
#line 4640 "test.tab.c"
    break;

  case 91: /* function_declaration: VOID ID $@10 OPENPAR funcargs CLOSEPAR $@11 OPENCURLY statement CLOSECURLY  */
#line 2198 "test.y"
                                                                                                                        {
																				Pop(pilhaEscopo);
																				(yyvsp[-2].text) = NULL;
																				(yyvsp[0].text) = NULL;
																				(*ancoraGlobalNo).filhos[1] = (yyvsp[-1].node);
																				(yyval.node) = ancoraGlobalNo;
																				dentroDeclaraFunc = FALSE;
																				ancoraGlobalNo = NULL;
																			}
#line 4654 "test.tab.c"
    break;

  case 92: /* assignment: ID ASSIGN mathop  */
#line 2211 "test.y"
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
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[-2].text),linhaCount,colunaCount);
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
#line 4701 "test.tab.c"
    break;

  case 93: /* variable_declaration: type ID  */
#line 2256 "test.y"
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
#line 4727 "test.tab.c"
    break;

  case 94: /* variable_declaration: type error  */
#line 2277 "test.y"
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
#line 4747 "test.tab.c"
    break;

  case 95: /* mathop: mathop PLUS mathop1  */
#line 2299 "test.y"
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
#line 4767 "test.tab.c"
    break;

  case 96: /* mathop: mathop MINUS mathop1  */
#line 2314 "test.y"
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
#line 4787 "test.tab.c"
    break;

  case 97: /* mathop: mathop1  */
#line 2329 "test.y"
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
#line 4805 "test.tab.c"
    break;

  case 98: /* mathop1: mathop1 AST mathop2  */
#line 2345 "test.y"
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
#line 4825 "test.tab.c"
    break;

  case 99: /* mathop1: mathop1 BS mathop2  */
#line 2360 "test.y"
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
#line 4845 "test.tab.c"
    break;

  case 100: /* mathop1: mathop2  */
#line 2375 "test.y"
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
#line 4863 "test.tab.c"
    break;

  case 101: /* mathop2: matharg  */
#line 2392 "test.y"
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
#line 4881 "test.tab.c"
    break;

  case 102: /* mathop2: OPENPAR mathop CLOSEPAR  */
#line 2405 "test.y"
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
#line 4901 "test.tab.c"
    break;

  case 103: /* matharg: ID  */
#line 2423 "test.y"
                                                                        {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "ID";
										(*ancora).nome = strdup(ancora2);
										simbolo *ancoraSimb = VerificarEscopo((yyvsp[0].text));
										if(ancoraSimb != NULL){ 
											(*ancora).refereTabela = ancoraSimb;
											(*ancora).tipoVirtual = ConverteTableTipo((*ancoraSimb).tipo);
										}
										else{
											printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",(yyvsp[0].text),linhaCount,colunaCount);
											(*ancora).refereTabela = NULL;
											errorCheck = TRUE;
										}
										(*ancora).valor = strdup((yyvsp[0].text));
										(*ancora).conversion = None;
										(*ancora).escopo = Top(pilhaEscopo)->valor;
										free((yyvsp[0].text));
										(yyval.node) = ancora;																
									}
#line 4928 "test.tab.c"
    break;

  case 104: /* matharg: num  */
#line 2446 "test.y"
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
#line 4946 "test.tab.c"
    break;

  case 105: /* matharg: function_call  */
#line 2459 "test.y"
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
#line 4964 "test.tab.c"
    break;

  case 106: /* matharg: EMPTY  */
#line 2473 "test.y"
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
#line 4981 "test.tab.c"
    break;

  case 107: /* type: SET  */
#line 2490 "test.y"
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
#line 5001 "test.tab.c"
    break;

  case 108: /* type: INT  */
#line 2505 "test.y"
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
#line 5021 "test.tab.c"
    break;

  case 109: /* type: ELEM  */
#line 2520 "test.y"
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
#line 5041 "test.tab.c"
    break;

  case 110: /* type: FLOAT  */
#line 2535 "test.y"
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
#line 5061 "test.tab.c"
    break;

  case 111: /* num: INTEGER  */
#line 2553 "test.y"
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
#line 5079 "test.tab.c"
    break;

  case 112: /* num: FLOATING  */
#line 2567 "test.y"
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
#line 5097 "test.tab.c"
    break;

  case 113: /* num: MINUS INTEGER  */
#line 2580 "test.y"
                                                        {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "negative integer";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_num;
										(*ancora).refereTabela = NULL;
										char ancora3[50] = "-";
										strcat(ancora3,(yyvsp[0].text));
										(*ancora).valor = strdup(ancora3);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Int;
										(yyval.node) = ancora;
										(yyvsp[-1].text) = NULL;
										free((yyvsp[0].text));
									}
#line 5118 "test.tab.c"
    break;

  case 114: /* num: MINUS FLOATING  */
#line 2597 "test.y"
                                                        {
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "negative floating";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_num;
										(*ancora).refereTabela = NULL;
										char ancora3[50] = "-";
										strcat(ancora3,(yyvsp[0].text));
										(*ancora).valor = strdup(ancora3);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Float;
										(yyval.node) = ancora;
										(yyvsp[-1].text) = NULL;
										free((yyvsp[0].text));
									}
#line 5139 "test.tab.c"
    break;


#line 5143 "test.tab.c"

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

#line 2616 "test.y"

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

void EscreverArvore(no* argumento,int profund){
	int i = 0;
	char* ancora = NULL;
	char* ancoraValor = NULL;

	if(argumento == NULL) return;
	

	ancora = (*argumento).nome;
	ancoraValor = (*argumento).valor;

	//printf("%s %s\n",ancora,ancoraValor);

	switch((*argumento).tipo){
		case YYSYMBOL_statement:
			if(!strcmp(ancora,"curly")){
				PrintLines(profund);
				printf(">{\n");
				EscreverArvore((*argumento).filhos[0],profund+1);
				PrintLines(profund);
				printf(">}\n");
			}
			else if(!strcmp(ancora,"variable_declaration")){
				EscreverArvore((*argumento).filhos[0],profund);
				PrintLines(profund);
				printf(">;\n");
				EscreverArvore((*argumento).filhos[1],profund);
			}
			else if(strcmp(ancora,"epsilon")){
				EscreverArvore((*argumento).filhos[0],profund);
				EscreverArvore((*argumento).filhos[1],profund);
			}
			else{
				return;
			}
		break;

		case YYSYMBOL_single_line_statement:
			EscreverArvore((*argumento).filhos[0],profund);
			PrintLines(profund);
			printf(">;\n");
		break;

		case YYSYMBOL_comparg:
			if(!strcmp(ancora,"comparison")){
				PrintLines(profund);
				printf(">(\n");
				EscreverArvore((*argumento).filhos[0],profund+1);
				PrintLines(profund);
				printf(">)\n");
			}
			else{
				EscreverArvore((*argumento).filhos[0],profund);
			}
		break;

		case YYSYMBOL_comparison:
			if(!strcmp(ancora,"not")){
				PrintLines(profund);
				printf(">not\n");
				EscreverArvore((*argumento).filhos[0],profund+1);
			}
			else{
				EscreverArvore((*argumento).filhos[0],profund+1);
				PrintLines(profund);
				printf(">%s\n",ancora);
				EscreverArvore((*argumento).filhos[0],profund+1);
			}
		break;

		case YYSYMBOL_read:
			PrintLines(profund);
			printf(">read\n");
			PrintLines(profund);
			printf(">(\n");
			PrintLines(profund+1);
			printf(">%s\n",ancoraValor);
			PrintLines(profund);
			printf(">)\n");
		break;

		case YYSYMBOL_write:
			if(!strcmp(ancora,"mathop")){
				PrintLines(profund);
				printf(">write\n");
				PrintLines(profund);
				printf(">(\n");
				EscreverArvore((*argumento).filhos[0],profund+1);
				PrintLines(profund);
				printf(">)\n");
			}
			else{
				PrintLines(profund);
				printf(">write\n");
				PrintLines(profund);
				printf(">(\n");
				PrintLines(profund+1);
				printf(">%s\n",ancoraValor);
				PrintLines(profund);
				printf(">)\n");
			}
		break;

		case YYSYMBOL_writeln:
			if(!strcmp(ancora,"mathop")){
				PrintLines(profund);
				printf(">write\n");
				PrintLines(profund);
				printf(">(\n");
				EscreverArvore((*argumento).filhos[0],profund+1);
				PrintLines(profund);
				printf(">)\n");
			}
			else{
				PrintLines(profund);
				printf(">write\n");
				PrintLines(profund);
				printf(">(\n");
				PrintLines(profund+1);
				printf(">%s\n",ancoraValor);
				PrintLines(profund);
				printf(">)\n");
			}
		break;

		case YYSYMBOL_return:
			PrintLines(profund);
			printf(">return\n");
			if(strcmp(ancora,"null")){
				EscreverArvore((*argumento).filhos[0],profund+1);
			}
		break;

		case YYSYMBOL_for:
			PrintLines(profund);
			printf(">for\n");
			PrintLines(profund);
			printf(">(\n");
			for(i = 0; i < 3; i++){
				EscreverArvore((*argumento).filhos[i],profund+1);
				PrintLines(profund);
				if(i != 2) printf(">;\n");
			}
			PrintLines(profund);
			printf(">)\n");
			PrintLines(profund);
			printf(">{\n");
			EscreverArvore((*argumento).filhos[3],profund+1);
			PrintLines(profund);
			printf(">}\n");
		break;

		case YYSYMBOL_if:
			PrintLines(profund);
			printf(">if\n");
			PrintLines(profund);
			printf(">(\n");

			EscreverArvore((*argumento).filhos[0],profund+1);

			PrintLines(profund);
			printf(">)\n");
			PrintLines(profund);
			printf(">{\n");

			EscreverArvore((*argumento).filhos[1],profund+1);

			PrintLines(profund);
			printf(">}\n");

			EscreverArvore((*argumento).filhos[2],profund);

		break;	

		case YYSYMBOL_else:
			if(!strcmp(ancora,"epsilon")){
				return;
			}
			PrintLines(profund);
			printf(">else\n");

			if(!strcmp(ancora,"curly")){
				PrintLines(profund);
				printf(">{\n");

				EscreverArvore((*argumento).filhos[0],profund+1);

				PrintLines(profund);
				printf(">}\n");
			}
			else{
				EscreverArvore((*argumento).filhos[0],profund+1);
			}
		break;

		case YYSYMBOL_conjuntoop:
			EscreverArvore((*argumento).filhos[0],profund);
		break;
		case YYSYMBOL_conjuntoop1:
			if(!strcmp(ancora,"ID")){
				PrintLines(profund);
				printf(">%s\n",ancoraValor);
			}
			else{
				EscreverArvore((*argumento).filhos[0],profund);
			}
		break;
		case YYSYMBOL_pertinencia:
			EscreverArvore((*argumento).filhos[0],profund+1);
			PrintLines(profund);
			printf(">in\n");
			EscreverArvore((*argumento).filhos[1],profund+1);
		break;
		case YYSYMBOL_tipagem:
			PrintLines(profund);
			printf(">is_set\n");
			PrintLines(profund);
			printf(">(\n");
			EscreverArvore((*argumento).filhos[0],profund+1);
			PrintLines(profund);
			printf(">)\n");
		break;
		case YYSYMBOL_inclusao:
			PrintLines(profund);
			printf(">add\n");
			PrintLines(profund);
			printf(">(\n");
			EscreverArvore((*argumento).filhos[0],profund+1);
			PrintLines(profund);
			printf(">)\n");
		break;
		case YYSYMBOL_remocao:
			PrintLines(profund);
			printf(">remove\n");
			PrintLines(profund);
			printf(">(\n");
			EscreverArvore((*argumento).filhos[0],profund+1);
			PrintLines(profund);
			printf(">)\n");
		break;
		case YYSYMBOL_selecao:
			PrintLines(profund);
			printf(">exists\n");
			PrintLines(profund);
			printf(">(\n");
			EscreverArvore((*argumento).filhos[0],profund+1);
			PrintLines(profund);
			printf(">)\n");
		break;
		case YYSYMBOL_iteracao:
			PrintLines(profund);
			printf(">forall\n");
			PrintLines(profund);
			printf(">(\n");

			EscreverArvore((*argumento).filhos[0],profund+1);

			PrintLines(profund);
			printf(">)\n");
			if(!strcmp(ancora,"bracket")){
				PrintLines(profund);
				printf(">{\n");

				EscreverArvore((*argumento).filhos[1],profund+1);

				PrintLines(profund);
				printf(">}\n");
			}
			else{
				EscreverArvore((*argumento).filhos[1],profund+1);
			}
		break;
		case YYSYMBOL_function_call:
			PrintLines(profund);
			printf(">%s\n",ancoraValor);
			PrintLines(profund);
			printf(">(\n");
			if(!strcmp(ancora,"function_call")){
				EscreverArvore((*argumento).filhos[0],profund+1);
			}
			PrintLines(profund);
			printf(">)\n");
		break;
		case YYSYMBOL_args:
			EscreverArvore((*argumento).filhos[0],profund);
			EscreverArvore((*argumento).filhos[1],profund+1);
		break;
		case YYSYMBOL_args1:
			if(!strcmp(ancora,"comma")){
				PrintLines(profund);
				printf(">,\n");
				EscreverArvore((*argumento).filhos[0],profund);
			}
		break;
		case YYSYMBOL_funcargs:
			if(!strcmp(ancora,"epsilon")){
				return;
			}
			EscreverArvore((*argumento).filhos[0],profund);
			PrintLines(profund);
			printf(">%s\n",ancoraValor);
			if(!strcmp(ancora,"comma")){
				PrintLines(profund);
				printf(">,\n");
				EscreverArvore((*argumento).filhos[1],profund+1);
			}
		break;
		case YYSYMBOL_function_declaration:
			i = 0;
			if(!strcmp(ancora,"type")){
				EscreverArvore((*argumento).filhos[i],profund);
				i++;
			}
			else{
				PrintLines(profund);
				printf(">void\n");
			}
			PrintLines(profund);
			printf(">%s\n",ancoraValor);
			PrintLines(profund);
			printf(">(\n");
			EscreverArvore((*argumento).filhos[i],profund+1);
			i++;
			PrintLines(profund);
			printf(">)\n");
			PrintLines(profund);
			printf(">{\n");
			EscreverArvore((*argumento).filhos[i],profund+1);
			i++;
			PrintLines(profund);
			printf(">}\n");
		break;
		case YYSYMBOL_assignment:
			PrintLines(profund);
			printf(">%s\n",ancoraValor);
			PrintLines(profund);
			printf(">=");
			EscreveConversao(argumento);
			printf("\n");
			EscreverArvore((*argumento).filhos[0],profund);
		break;
		case YYSYMBOL_variable_declaration:
			EscreverArvore((*argumento).filhos[0],profund);
			PrintLines(profund);
			printf(">%s\n",ancoraValor);
		break;
		case YYSYMBOL_mathop:
			if(!strcmp(ancora,"mathop1")){
				EscreverArvore((*argumento).filhos[0],profund);
			}
			else{
				EscreverArvore((*argumento).filhos[0],profund+1);
				PrintLines(profund);
				printf(">%s",ancora);
				EscreveConversao(argumento);
				printf("\n");
				EscreverArvore((*argumento).filhos[1],profund+1);
			}
		break;
		case YYSYMBOL_mathop1:
			if(!strcmp(ancora,"mathop2")){
				EscreverArvore((*argumento).filhos[0],profund);
			}
			else{
				EscreverArvore((*argumento).filhos[0],profund+1);
				PrintLines(profund);
				printf(">%s",ancora);
				EscreveConversao(argumento);
				printf("\n");
				EscreverArvore((*argumento).filhos[1],profund+1);
			}
		break;
		case YYSYMBOL_mathop2:
			if(!strcmp(ancora,"pars")){
				PrintLines(profund);
				printf(">(\n");
				EscreverArvore((*argumento).filhos[0],profund);
				PrintLines(profund);
				printf(">)\n");
			}
			else{
				EscreverArvore((*argumento).filhos[0],profund);
			}
		break;
		case YYSYMBOL_matharg:
			if(!strcmp(ancora,"function_call") || !strcmp(ancora,"num")){
				EscreverArvore((*argumento).filhos[0],profund);
			}
			else{
				PrintLines(profund);
				printf(">%s\n",ancoraValor);
			}
		break;
		case YYSYMBOL_type:
			PrintLines(profund);
			printf(">%s\n",ancora);
		break;
		case YYSYMBOL_num:
			PrintLines(profund);
			printf(">%s\n",ancoraValor);
		break;



		default:
		break;
		}
}

void ApagarArvore(no* alvo){
	int i;
	for(i = 0;i < (*alvo).numFilhos; i++){
		ApagarArvore((*alvo).filhos[i]);
	}
	ApagarNo(alvo);
}


int main(int argc, char **argv){
	tabelaSimbolos = NULL;
	//raiz = (no*)malloc(sizeof(no));
	//variavelRaiz = TRUE;
	++argv;
	--argc;//pula o nome do programa
	escopoCounter = 0;
	funcArgsCounter = 0;
	textCounter = 0;
	errorCheck = FALSE;
	ancoraGlobalNo = NULL;
	pilhaEscopo = CriarStack(0);
	dentroDeclaraFunc = FALSE;
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
	ApagarArvore(raiz);
	
	yylex_destroy();
	return 0;
}

