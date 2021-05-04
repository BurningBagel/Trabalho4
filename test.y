%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definicoes.h"

extern FILE *yyin;
extern int linhaCount;
extern int colunaCount;
extern int yylex_destroy();

void yyerror(char const *s);
int yylex(void);


//#define ID 1
#define CHAR_TABLE 2
#define INT_TABLE 3
#define FLOAT_TABLE 4
#define STRING_TABLE 5
#define FUNC_TABLE 6
#define ELEM_TABLE 7
#define SET_TABLE 8



simbolo* tabelaSimbolos;
simbolo* topo;
no* raiz;
pilha* pilhaEscopo;

int escopoCounter;


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

int ContaFuncCallArgs(no* alvo){
	if((*alvo).tipo == YYSYMBOL_args){
		return 1 + ContaFuncArgs((*alvo).filhos[1]);
	}
	else{
		if(!strcmp((*alvo).nome,"comma")){
			return ContaFuncArgs((*alvo).filhos[1]);
		}
		else{
			return 0;
		}
	}
}

int ContaFuncArgs(no* alvo){
	if(!strcmp((*alvo).nome,"epsilon")) return 0;
	else if (!strcmp((*alvo).nome,"single")) return 1;
	else return 1 + ContaFuncArgs((*alvo).filhos[1]);
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

%}

%define parse.lac full
%define parse.error detailed

%union {
	double val;	
	char *text;
	//bool boolean;
	struct no *node;
}

%token <text> INTEGER
%token <text> FLOATING
%token <text> STRING
%token <text> EMPTY
%token <text> CHAR
%token <text> ID
%token <text> INT
%token <text> FLOAT
%token <text> ELEM
%token <text> SET
%token <text> IF
%token <text> ELSE
%token <text> FOR
%token <text> READ
%token <text> WRITE
%token <text> WRITELN
%token <text> IN
%token <text> IS_SET
%token <text> ADD
%token <text> REMOVE
%token <text> EXISTS
%token <text> FORALL
%token <text> RETURN
%token <text> OPENPAR
%token <text> CLOSEPAR
%token <text> OPENCURLY
%token <text> CLOSECURLY
%token <text> OPENBRAC
%token <text> CLOSEBRAC
%token <text> LESS
%token <text> LE
%token <text> EQ
%token <text> NEQ
%token <text> GREATER
%token <text> GE
%token <text> PLUS
%token <text> MINUS
%token <text> DBS
%token <text> AST
%token <text> BS
%token <text> ASSIGN
%token <text> COMMA
%token <text> SEMICOLON
%token <text> OR
%token <text> AND
%token <text> NOT
%token <text> AMP
%token <text> PCENT
%token <text> VOID

%start	inicio 

%type <node> statement
%type <node> assignment
%type <node> function_declaration
%type <node> conjuntoop
%type <node> mathop
%type <node> return
%type <node> for
%type <node> if
%type <node> write
%type <node> writeln
%type <node> read
%type <node> comparg
%type <node> comparison
//%type <text> in
%type <node> conjuntoop1
%type <node> pertinencia
%type <node> tipagem
%type <node> inclusao
%type <node> remocao
%type <node> selecao
%type <node> iteracao
%type <node> function_call
%type <node> funcargs
%type <node> args
%type <node> args1
%type <node> variable_declaration
%type <node> mathop1
%type <node> mathop2
%type <node> matharg
%type <node> type
%type <node> single_line_statement
%type <node> else
%type <node> num

%destructor {$$ = NULL;} <*>

%%
/*
int VerificaReturn(no* curr){
	if(curr->tipo == RETURN){
		return TRUE;
	}
	else{
		if(curr->numFilhos > 0){
			for filho in Filhos{
				if(VerificaReturn(filho)){
					return TRUE;
				}
			}
		}
		return FALSE;
	}
}
*/


inicio:
		statement								{
													raiz = $1;
													//(*raiz).escopo = 1;
													if(ProcurarTabela("main") == NULL){
														printf("ERRO SEMANTICO! NAO FOI DECLARADA UMA FUNCAO MAIN!\n");
													}
												}
	;





statement: 
		single_line_statement statement 		{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $2;
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
													$$ = ancora;
												}
	|	OPENCURLY 
												{
													escopoCounter++;
													Push(pilhaEscopo,CriarStack(escopoCounter));
													$1 = NULL;
												} 
		statement CLOSECURLY 					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $3;
													//(*(no*)$1).escopo = (*ancora).escopo;
													//(*(no*)$2).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "curly";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													// $ 3 = NULL;
													$4 = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													Pop(pilhaEscopo);
												}
		
	|	function_declaration statement 			{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $2;
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
													$$ = ancora;
												}
	
	
	/*|	mathop SEMICOLON statement				{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
												}
	*/
	
	|	for statement							{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $2;
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
													$$ = ancora;
												}
	|	if statement							{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $2;
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
													$$ = ancora;
												}
	|	iteracao statement 						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 2;
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $2;
													//(*(no*)$1).escopo = (*ancora).escopo;
													//(*(no*)$2).escopo = (*ancora).escopo;
													(*ancora).tipo = YYSYMBOL_statement;
													char ancora2[] = "iteracao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
												}
	|	variable_declaration SEMICOLON statement 			{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													//(*(no*)$1).escopo = (*ancora).escopo;
													//(*(no*)$3).escopo = (*ancora).escopo;
													char ancora2[] = "variable_declaration";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													$2 = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
	/*
	|	assignment SEMICOLON statement			{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													char ancora2[] = "assignment";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													free($2);
												}
	
	|	write SEMICOLON statement				{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													char ancora2[] = "write";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													free($2);
												}
	|	conjuntoop statement			        {
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $2;
													(*ancora).numFilhos = 2;
													char ancora2[] = "conjuntoop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
												}
	|	return statement						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $2;
													(*ancora).numFilhos = 2;
													char ancora2[] = "return";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
												}
	|	writeln SEMICOLON statement				{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													char ancora2[] = "writeln";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													free($2);
												}
	|	read SEMICOLON statement				{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													char ancora2[] = "read";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													free($2);
												}
	|	function_call SEMICOLON statement 		{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													char ancora2[] = "function_call";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													free($2);
												}
 */
	|	%empty									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													char ancora2[] = "epsilon";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
	;
	


single_line_statement:
		return SEMICOLON			 			{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "return";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													$2 = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}

	|	assignment SEMICOLON 					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "assignment";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													$2 = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}

	|	write SEMICOLON		 					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "write";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													$2 = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}

	|	writeln SEMICOLON	 					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "writeln";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													$2 = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}

	|	read SEMICOLON		 					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "read";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													$2 = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}
/*
	|	mathop 									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).numFilhos = 1;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
												}
*/
	|	conjuntoop 	SEMICOLON		 			{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													//(*(no*)$1).escopo = (*ancora).escopo;
													(*ancora).numFilhos = 1;
													char ancora2[] = "conjuntoop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													$$ = ancora;
													$2 = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
												}

	|	error SEMICOLON 						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													char ancora2[] = "ERROR";
													(*ancora).nome = strdup(ancora2);
													(*ancora).tipo = YYSYMBOL_single_line_statement;
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$2 = NULL;
													yyerrok;
												}

	;


comparg:
	/*
		ID										{
													no* ancora = (no*)malloc(sizeof(no));
													$$ = ancora;
													(*ancora).tipo = YYSYMBOL_comparg;
													(*ancora).numFilhos = 0;
													(*ancora).valor = strdup($1);
													char ancora2[] = "ID";
													(*ancora).nome = strdup(ancora2);
													simbolo *ancoraSimb = VerificarEscopo($1);
													if(ancoraSimb != NULL){ //Vamos começar a usar a tabela de simbolos! Se não acharmos este ID na tabela, devemos colocar-lo lá, mas sem valor! Só em assignment a gente coloca valor
														(*ancora).refereTabela = ancoraSimb;
													}
													else{
														//(*ancora).refereTabela = CriarSimbolo($1,0,NULL,escopoCounter); Tirei essa linha pq é hora de acusar erros semânticos! Se acharmos um ID que não foi declarado, temos q dar erro!
														printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO!\n",$1);
														(*ancora).refereTabela = NULL;
													}
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													free($1);

													

												}
	*/
		OPENPAR comparison CLOSEPAR				{	
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $2;
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_comparg;
													char ancora2[] = "comparison";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ($2)->tipoVirtual;
													$$ = ancora;
													$1 = NULL;
													$3 = NULL;
												}
	|	mathop									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_comparg;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).valor = NULL;
													(*ancora).refereTabela = NULL;
													(*ancora).tipoVirtual = ($1)->tipoVirtual;
													(*ancora).conversion = None;
													$$ = ancora;
												}
	/*
	|	function_call 							{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_comparg;
													char ancora2[] = "function_call";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ($1)->tipoVirtual;
													$$ = ancora;
												}
	*/
	;

comparison:
		NOT comparg								{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $2;
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "not";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													switch(($2)->tipoVirtual){
														case Elem:
															(*ancora).conversion = ElemToIntLeft;
															(*ancora).tipoVirtual = Int;
															break;
														case Set:
															printf("ERRO SEMANTICO! EXPRESSAO RESULTANTE EM SET USADA EM OPERACAO INVALIDA! Linha: %d, Coluna: %d\n",linhaCount,colunaCount);
															break;
														default:
															(*ancora).conversion = None;
															(*ancora).tipoVirtual = ($2)->tipoVirtual;
															break;
													}
													$$ = ancora;
													$1 = NULL;
												}
	|	comparg AND comparg						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "and";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
													(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
													$$ = ancora;
													$2 = NULL;
												}
	|	comparg OR comparg						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "or";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
													(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
													$$ = ancora;
													$2 = NULL;
												}
	|	comparg GREATER comparg					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "greater";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
													(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
													$$ = ancora;
													$2 = NULL;
												}
	|	comparg GE comparg						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "greater_or_equal";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
													(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
													$$ = ancora;
													$2 = NULL;
												}
	|	comparg LESS comparg					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "less";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
													(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
													$$ = ancora;
													$2 = NULL;
												}
	|	comparg LE comparg						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "less_or_equal";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
													(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
													$$ = ancora;
													$2 = NULL;
												}
	|	comparg EQ comparg						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "equal";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
													(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
													$$ = ancora;
													$2 = NULL;
												}
	|	comparg NEQ comparg						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).numFilhos = 2;
													(*ancora).tipo = YYSYMBOL_comparison;
													char ancora2[] = "not_equal";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
													(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
													$$ = ancora;
													$2 = NULL;
												}
	;

read:
		READ OPENPAR ID CLOSEPAR				{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_read;
													char ancora2[] = "read";
													(*ancora).nome = strdup(ancora2);
													simbolo *ancoraSimb = VerificarEscopo($3);
													if(ancoraSimb != NULL){ 
														(*ancora).refereTabela = ancoraSimb;
													}
													else{
														printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",$3,linhaCount,colunaCount);
														(*ancora).refereTabela = NULL;
													}
													$$ = ancora;
													(*ancora).valor = strdup($3);
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													free($3);
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
	;

write:
//		WRITE OPENPAR ID CLOSEPAR
		WRITE OPENPAR mathop CLOSEPAR			{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $3;
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_write;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
//	|	WRITE OPENPAR NUM CLOSEPAR	
	|	WRITE OPENPAR  STRING  CLOSEPAR			{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_write;
													char ancora2[] = "string";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = strdup($3);
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													free($3);
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
	|	WRITE OPENPAR CHAR CLOSEPAR				{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_write;
													char ancora2[] = "char";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = strdup($3);
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													free($3);
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
	;


writeln:
//		WRITELN OPENPAR ID CLOSEPAR
		WRITELN OPENPAR mathop CLOSEPAR			{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $3;
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_writeln;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
//	|	WRITELN OPENPAR NUM CLOSEPAR
	|	WRITELN OPENPAR  STRING  CLOSEPAR		{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_writeln;
													char ancora2[] = "string";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = strdup($3);
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													free($3);
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
	;
/*in:
		ID IN ID
	|	ID IN conjuntoop
	;
*/
return:
//		RETURN ID
//	|	RETURN function_call
		RETURN comparison						{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $2;
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_return;
													char ancora2[] = "comparison";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													//free($3);
												}
	|	RETURN mathop							{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).filhos[0] = $2;
													(*ancora).numFilhos = 1;
													(*ancora).tipo = YYSYMBOL_return;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													//free($3);
												}
	|	RETURN 									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_return;
													char ancora2[] = "null";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													//free($2);
												}
	;

for:
		FOR OPENPAR assignment SEMICOLON comparison SEMICOLON assignment CLOSEPAR  								{
																													escopoCounter++;
																													Push(pilhaEscopo,CriarStack(escopoCounter));
																													$1 = NULL;
																													$2 = NULL;
																													$4 = NULL;
																													$6 = NULL;
																													$8 = NULL;
																													
																													
																												} 
			OPENCURLY statement CLOSECURLY																		{
																													no* ancora = (no*)malloc(sizeof(no));
																													(*ancora).filhos[0] = $3;
																													(*ancora).filhos[1] = $5;
																													(*ancora).filhos[2] = $7;
																													//$ $ = ancora;
																													(*ancora).numFilhos = 4;
																													(*ancora).tipo = YYSYMBOL_for;
																													char ancora2[] = "for";
																													(*ancora).nome = strdup(ancora2);
																													(*ancora).refereTabela = NULL;
																													(*ancora).valor = NULL;
																													(*ancora).filhos[3] = $11;
																													(*ancora).conversion = None;
																													(*ancora).tipoVirtual = 0;
																													$12 = NULL;
																													$10 = NULL;
																													$$ = ancora;
																													Pop(pilhaEscopo);
																												}
	;

if:
		
	IF OPENPAR comparison CLOSEPAR OPENCURLY 
																											{
																												escopoCounter++;
																												Push(pilhaEscopo,CriarStack(escopoCounter));
																												
																											} 
		statement CLOSECURLY else																			{
																												no* ancora = (no*)malloc(sizeof(no));
																												(*ancora).filhos[0] = $3;
																												$1 = NULL;
																												$2 = NULL;
																												$4 = NULL;
																												$5 = NULL;
																												char ancora2[] = "if";
																												(*ancora).nome = strdup(ancora2);
																												(*ancora).refereTabela = NULL;
																												(*ancora).valor = NULL;
																												(*ancora).numFilhos = 3;
																												(*ancora).tipo = YYSYMBOL_if;
																												(*ancora).filhos[1] = $7;
																												(*ancora).filhos[2] = $9;
																												(*ancora).conversion = None;
																												(*ancora).tipoVirtual = 0;
																												$8 = NULL;
																												Pop(pilhaEscopo);
																												$$ = ancora;
																											}
	|  IF OPENPAR comparison CLOSEPAR single_line_statement else											{
																												no* ancora = (no*)malloc(sizeof(no));
																												(*ancora).filhos[0] = $3;
																												(*ancora).filhos[1] = $5;
																												(*ancora).filhos[2] = $6;
																												(*ancora).numFilhos = 3;
																												(*ancora).tipo = YYSYMBOL_if;
																												char ancora2[] = "single_line_statement";
																												(*ancora).nome = strdup(ancora2);
																												(*ancora).refereTabela = NULL;
																												(*ancora).valor = NULL;
																												(*ancora).conversion = None;
																												(*ancora).tipoVirtual = 0;
																												$$ = ancora;
																												$1 = NULL;
																												$2 = NULL;
																												$4 = NULL;
																											}
	|	IF OPENPAR error CLOSEPAR OPENCURLY 
																											{
																												escopoCounter++;
																												Push(pilhaEscopo,CriarStack(escopoCounter));
																												
																											} 
		statement CLOSECURLY else																			{
																												no* ancora = (no*)malloc(sizeof(no));
																												$1 = NULL;
																												$2 = NULL;
																												$4 = NULL;
																												$5 = NULL;
																												char ancora2[] = "ERROR";
																												(*ancora).nome = strdup(ancora2);
																												(*ancora).refereTabela = NULL;
																												(*ancora).valor = NULL;
																												(*ancora).numFilhos = 2;
																												(*ancora).tipo = YYSYMBOL_if;
																												(*ancora).filhos[0] = $7;
																												(*ancora).filhos[1] = $9;
																												(*ancora).conversion = None;
																												(*ancora).tipoVirtual = 0;
																												$8 = NULL;
																												Pop(pilhaEscopo);
																												$$ = ancora;
																												yyerrok;
																											}
	|  IF OPENPAR error CLOSEPAR single_line_statement else													{
																												no* ancora = (no*)malloc(sizeof(no));
																												(*ancora).filhos[0] = $5;
																												(*ancora).filhos[1] = $6;
																												(*ancora).numFilhos = 2;
																												(*ancora).tipo = YYSYMBOL_if;
																												char ancora2[] = "ERROR";
																												(*ancora).nome = strdup(ancora2);
																												(*ancora).refereTabela = NULL;
																												(*ancora).valor = NULL;
																												(*ancora).conversion = None;
																												(*ancora).tipoVirtual = 0;
																												$$ = ancora;
																												$1 = NULL;
																												$2 = NULL;
																												$4 = NULL;
																												yyerrok;
																											}
	;

else:
	ELSE if 										{
														no* ancora = (no*)malloc(sizeof(no));
														(*ancora).filhos[0] = $2;
														(*ancora).numFilhos = 1;
														char ancora2[] = "if";
														(*ancora).nome = strdup(ancora2);
														(*ancora).tipo = YYSYMBOL_else;
														(*ancora).refereTabela = NULL;
														(*ancora).valor = NULL;
														(*ancora).conversion = None;
														(*ancora).tipoVirtual = 0;
														$$ = ancora;
														free($1);
													}
|	ELSE single_line_statement						{
														no* ancora = (no*)malloc(sizeof(no));
														(*ancora).filhos[0] = $2;
														(*ancora).numFilhos = 1;
														char ancora2[] = "single_line_statement";
														(*ancora).nome = strdup(ancora2);
														(*ancora).tipo = YYSYMBOL_else;
														(*ancora).refereTabela = NULL;
														(*ancora).valor = NULL;
														(*ancora).conversion = None;
														(*ancora).tipoVirtual = 0;
														$$ = ancora;
														$1 = NULL;
													}
|	ELSE OPENCURLY									{
														escopoCounter++;
														Push(pilhaEscopo,CriarStack(escopoCounter));
														$1 = NULL;
														$2 = NULL;
													}
		statement CLOSECURLY						{
														no* ancora = (no*)malloc(sizeof(no));
														(*ancora).filhos[0] = $4;
														(*ancora).numFilhos = 1;
														char ancora2[] = "curly";
														(*ancora).nome = strdup(ancora2);
														(*ancora).tipo = YYSYMBOL_else;
														(*ancora).refereTabela = NULL;
														(*ancora).valor = NULL;
														(*ancora).conversion = None;
														(*ancora).tipoVirtual = 0;
														$$ = ancora;
														$5 = NULL;
														Pop(pilhaEscopo);
													}
|	%empty											{
														no* ancora = (no*)malloc(sizeof(no));
														(*ancora).numFilhos = 0;
														char ancora2[] = "epsilon";
														(*ancora).nome = strdup(ancora2);
														(*ancora).tipo = YYSYMBOL_else;
														(*ancora).refereTabela = NULL;
														(*ancora).valor = NULL;
														(*ancora).conversion = None;
														(*ancora).tipoVirtual = 0;
														$$ = ancora;
													}
;

conjuntoop:
		pertinencia								{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "pertinencia";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ($1)->tipoVirtual;
													$$ = ancora;
												}
	|	tipagem									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "tipagem";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ($1)->tipoVirtual;
													$$ = ancora;
												}
	|	inclusao								{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "inclusao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													$$ = ancora;
												}
	|	remocao									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "remocao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													$$ = ancora;
												}
	|	selecao									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop;
													char ancora2[] = "selecao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													$$ = ancora;
												}
	;
	
conjuntoop1:
		pertinencia								{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "pertinencia";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ($1)->tipoVirtual;
													$$ = ancora;
												}
	|	tipagem									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "tipagem";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = ($1)->tipoVirtual;
													$$ = ancora;
												}
	|	inclusao								{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "inclusao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													$$ = ancora;
												}
	|	remocao									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "remocao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													$$ = ancora;
												}
	|	selecao									{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $1;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "selecao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = Untyped;
													$$ = ancora;
												}
	|	ID 										{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 0;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "ID";
													(*ancora).nome = strdup(ancora2);
													simbolo *ancoraSimb = VerificarEscopo($1);
													if(ancoraSimb != NULL){ 
														(*ancora).refereTabela = ancoraSimb;
													}
													else{
														printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",$1,linhaCount,colunaCount);
														(*ancora).refereTabela = NULL;
													}
													(*ancora).valor = strdup($1);
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = (*ancoraSimb).tipo;
													free($1);
													$$ = ancora;
												}
	;

pertinencia:
		mathop IN conjuntoop1 					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 2;
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).tipo = YYSYMBOL_pertinencia;
													char ancora2[] = "mathop";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$2 = NULL;
												}
	|	selecao IN conjuntoop1 					{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 2;
													(*ancora).filhos[0] = $1;
													(*ancora).filhos[1] = $3;
													(*ancora).tipo = YYSYMBOL_pertinencia;
													char ancora2[] = "selecao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$2 = NULL;
												}
	;

tipagem:
		IS_SET OPENPAR conjuntoop1 CLOSEPAR		{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $3;
													(*ancora).tipo = YYSYMBOL_tipagem;
													char ancora2[] = "tipagem";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
	;

inclusao:
		ADD OPENPAR pertinencia CLOSEPAR		{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $3;
													(*ancora).tipo = YYSYMBOL_inclusao;
													char ancora2[] = "inclusao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
	;

remocao:
		REMOVE OPENPAR pertinencia CLOSEPAR		{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $3;
													(*ancora).tipo = YYSYMBOL_remocao;
													char ancora2[] = "remocao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
	;
	
	
selecao:
		EXISTS OPENPAR pertinencia CLOSEPAR		{
													no* ancora = (no*)malloc(sizeof(no));
													(*ancora).numFilhos = 1;
													(*ancora).filhos[0] = $3;
													(*ancora).tipo = YYSYMBOL_conjuntoop1;
													char ancora2[] = "iteracao";
													(*ancora).nome = strdup(ancora2);
													(*ancora).refereTabela = NULL;
													(*ancora).valor = NULL;
													(*ancora).conversion = None;
													(*ancora).tipoVirtual = 0;
													$$ = ancora;
													$1 = NULL;
													$2 = NULL;
													$4 = NULL;
												}
	;

iteracao:
		FORALL OPENPAR pertinencia CLOSEPAR OPENCURLY
																			{
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				
																			}
		 statement CLOSECURLY												{
		 																		$1 = NULL;
																				$2 = NULL;
																				$4 = NULL;
																				$5 = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 2;
																				(*ancora).filhos[0] = $3;
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "bracket";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).refereTabela = NULL;
																				(*ancora).valor = NULL;
																				(*ancora).filhos[1] = $7;
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				$8 = NULL;
																				Pop(pilhaEscopo);
																				$$ = ancora;
																			}
	|	FORALL OPENPAR pertinencia CLOSEPAR single_line_statement 			{
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 2;
																				(*ancora).filhos[0] = $3;
																				(*ancora).filhos[1] = $5;
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "semicolon";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).refereTabela = NULL;
																				(*ancora).valor = NULL;
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				$$ = ancora;
																				$1 = NULL;
																				$2 = NULL;
																				$4 = NULL;
																			}
	|	FORALL OPENPAR error CLOSEPAR OPENCURLY
																			{
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																				
																			}
		 statement CLOSECURLY												{
		 																		$1 = NULL;
																				$2 = NULL;
																				$4 = NULL;
																				$5 = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).filhos[0] = $7;
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "ERROR";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).refereTabela = NULL;
																				(*ancora).valor = NULL;
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				$8 = NULL;
																				Pop(pilhaEscopo);
																				$$ = ancora;
																				yyerrok;
																			}
	|	FORALL OPENPAR error CLOSEPAR single_line_statement 				{
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).filhos[0] = $5;
																				(*ancora).tipo = YYSYMBOL_iteracao;
																				char ancora2[] = "ERROR SINGLE";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).refereTabela = NULL;
																				(*ancora).valor = NULL;
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				$$ = ancora;
																				$1 = NULL;
																				$2 = NULL;
																				$4 = NULL;
																				yyerrok;
																			}
	;

function_call:
		ID OPENPAR args CLOSEPAR 											{
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).filhos[0] = $3;
																				(*ancora).tipo = YYSYMBOL_function_call;
																				char ancora2[] = "function_call";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).valor = strdup($1);
																				simbolo *ancoraSimb = VerificarEscopo($1);
																				if(ancoraSimb != NULL){ 
																					(*ancora).refereTabela = ancoraSimb;
																					(*ancora).tipoVirtual = (*ancoraSimb).returnType;
																				}
																				else{
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO! Linha: %d, Coluna: %d\n",$1,linhaCount,colunaCount);
																					(*ancora).refereTabela = NULL;
																					(*ancora).tipoVirtual = 0;
																				}
																				int calledArgs = ContaFuncArgs();
																				if((*ancora).refereTabela->numArgs != calledArgs){
																					printf("ERRO SEMANTICO! CHAMADA DE FUNCAO %s USA QUANTIDADE ERRADA DE ARGUMENTOS! Linha: %d, Coluna %d\n",$1,linhaCount,colunaCount);
																				}
																				if(!VerificaTipoArgs($3,(*ancora).refereTabela->funcArgsTypes,0)){
																					printf("ERRO SEMANTICO! CHAMADA DE FUNCAO %s USA ARGUMENTOS DE TIPOS ERRADOS! Linha: %d, Coluna %d\n",$1,linhaCount,colunaCount);

																				}
																				(*ancora).conversion = None;
																				$$ = ancora;
																				$1 = NULL;
																				$2 = NULL;
																				$4 = NULL;
																			}
	|	ID OPENPAR CLOSEPAR													{
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 0;
																				(*ancora).tipo = YYSYMBOL_function_call;
																				char ancora2[] = "function_call";
																				(*ancora).nome = strdup(ancora2);
																				(*ancora).valor = strdup($1);
																				simbolo *ancoraSimb = VerificarEscopo($1);
																				if(ancoraSimb != NULL){ 
																					(*ancora).refereTabela = ancoraSimb;
																					(*ancora).tipoVirtual = (*ancoraSimb).returnType;
																				}
																				else{
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO!\n",$1);
																					(*ancora).refereTabela = NULL;
																					(*ancora).tipoVirtual = 0;
																				}
																				if((*ancoraSimb).)
																				(*ancora).conversion = None;
																				$$ = ancora;
																				$1 = NULL;
																				$2 = NULL;
																				$3 = NULL;
																			}
	;


/*args:
		args COMMA args
	|	ID
	|	NUM
	;
*/
args:
		mathop args1		{
								no* ancora = (no*)malloc(sizeof(no));
								(*ancora).numFilhos = 2;
								(*ancora).filhos[0] = $1;
								(*ancora).filhos[1] = $2;
								(*ancora).tipo = YYSYMBOL_args;
								char ancora2[] = "mathop";
								(*ancora).nome = strdup(ancora2);
								(*ancora).refereTabela = NULL;
								(*ancora).valor = strdup(($1)->valor);
								(*ancora).conversion = None;
								(*ancora).tipoVirtual = ($1)->tipoVirtual;
								$$ = ancora;
							}
		/*|	num args1		{	
								no* ancora = (no*)malloc(sizeof(no));
								(*ancora).numFilhos = 1;
								(*ancora).filhos[0] = $2;
								(*ancora).tipo = YYSYMBOL_args;
								char ancora2[] = "num";
								(*ancora).nome = strdup(ancora2);
								(*ancora).refereTabela = NULL;
								(*ancora).valor = strdup($1);
								(*ancora).conversion = None;
								(*ancora).tipoVirtual = ($1)->tipoVirtual;
								$$ = ancora;
							}
	|	function_call args1 {	
								no* ancora = (no*)malloc(sizeof(no));
								(*ancora).numFilhos = 2;
								(*ancora).filhos[0] = $1;
								(*ancora).filhos[0] = $2;
								(*ancora).tipo = YYSYMBOL_args;
								char ancora2[] = "function_call";
								(*ancora).nome = strdup(ancora2);
								(*ancora).refereTabela = NULL;
								(*ancora).conversion = None;
								(*ancora).tipoVirtual = 0;
								$$ = ancora;
							}
	*/
	|	error args1 		{	
								no* ancora = (no*)malloc(sizeof(no));
								(*ancora).numFilhos = 1;
								(*ancora).filhos[0] = $2;
								(*ancora).tipo = YYSYMBOL_args;
								char ancora2[] = "ERROR";
								(*ancora).nome = strdup(ancora2);
								(*ancora).refereTabela = NULL;
								(*ancora).conversion = None;
								(*ancora).tipoVirtual = 0;
								$$ = ancora;
								yyerrok;
							}
	;

args1:
		COMMA args		{
							no* ancora = (no*)malloc(sizeof(no));
							(*ancora).numFilhos = 1;
							(*ancora).filhos[0] = $2;
							(*ancora).tipo = YYSYMBOL_args1;
							char ancora2[] = "comma";
							(*ancora).nome = strdup(ancora2);
							(*ancora).refereTabela = NULL;
							(*ancora).valor = NULL;
							(*ancora).conversion = None;
							(*ancora).tipoVirtual = 0;
							$$ = ancora;
							$1 = NULL;
						}
	|	%empty			{
							no* ancora = (no*)malloc(sizeof(no));
							(*ancora).numFilhos = 0;
							char ancora2[] = "epsilon";
							(*ancora).nome = strdup(ancora2);
							(*ancora).tipo = YYSYMBOL_args1;
							(*ancora).refereTabela = NULL;
							(*ancora).valor = NULL;
							(*ancora).conversion = None;
							(*ancora).tipoVirtual = 0;
							$$ = ancora;
						}
	;

funcargs:
		type ID					{
									no* ancora = (no*)malloc(sizeof(no));
									(*ancora).numFilhos = 1;
									(*ancora).filhos[0] = $1;
									(*ancora).tipo = YYSYMBOL_funcargs;
									char ancora2[] = "single";
									(*ancora).nome = strdup(ancora2);
									//simbolo *ancoraSimb = VerificarEscopo($2);
									(*ancora).valor = strdup($2);
									(*ancora).refereTabela = CriarSimbolo($2,atoi(((no*)$1)->valor),NULL,escopoCounter);
									
									(*ancora).conversion = None;
									(*ancora).tipoVirtual = 0;
									free($2);
									$$ = ancora;
								}
	|	type ID COMMA funcargs	{
									no* ancora = (no*)malloc(sizeof(no));
									(*ancora).numFilhos = 2;
									(*ancora).filhos[0] = $1;
									(*ancora).filhos[1] = $4;
									(*ancora).tipo = YYSYMBOL_funcargs;
									char ancora2[] = "comma";
									(*ancora).nome = strdup(ancora2);
									//printf("\n\nOI %s OI\n\n",$2);
									(*ancora).valor = strdup($2);
									(*ancora).refereTabela = CriarSimbolo($2,atoi(((no*)$1)->valor),NULL,escopoCounter);
									(*ancora).conversion = None;
									(*ancora).tipoVirtual = 0;
									free($2);
									$3 = NULL;
									$$ = ancora;
								}
	|	%empty					{
									no* ancora = (no*)malloc(sizeof(no));
									(*ancora).numFilhos = 0;
									char ancora2[] = "epsilon";
									(*ancora).nome = strdup(ancora2);
									(*ancora).tipo = YYSYMBOL_funcargs;
									(*ancora).refereTabela = NULL;
									(*ancora).valor = NULL;
									(*ancora).conversion = None;
									(*ancora).tipoVirtual = 0;
									$$ = ancora;
								}
	|	error					{
									no* ancora = (no*)malloc(sizeof(no));
									(*ancora).numFilhos = 0;
									char ancora2[] = "ERROR";
									(*ancora).nome = strdup(ancora2);
									(*ancora).tipo = YYSYMBOL_funcargs;
									(*ancora).refereTabela = NULL;
									(*ancora).valor = NULL;
									(*ancora).conversion = None;
									(*ancora).tipoVirtual = 0;
									$$ = ancora;
									yyerrok;
								}
	;
	

function_declaration:
		type ID  
																			{ //Declaração de função é um pouco estranha, pq o escopo da função é diferente dos argumentos
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																			}
		OPENPAR funcargs CLOSEPAR OPENCURLY statement CLOSECURLY 			{
																				int numArgumentos;
																				numArgumentos = ContaFuncArgs($5);
																				$4 = NULL;
																				$6 = NULL;
																				$7 = NULL;
																				$9 = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				int tipoRetorno = ConverteRetornoTipo($1);
																				int realEscopo;
																				(*ancora).numFilhos = 3;
																				(*ancora).filhos[0] = $1;
																				(*ancora).filhos[1] = $5;
																				(*ancora).filhos[2] = $8;
																				(*ancora).tipo = YYSYMBOL_function_declaration;
																				char ancora2[] = "type";
																				(*ancora).nome = strdup(ancora2);
																				simbolo *ancoraSimb = ProcurarTabela($2);
																				Pop(pilhaEscopo);
																				realEscopo = Top(pilhaEscopo)->valor;
																				if(ancoraSimb != NULL){
																					printf("ERRO SEMANTICO! ID %s REDECLARADO COMO FUNCAO! LINHA: %d, COLUNA: %d \n",$2,linhaCount,colunaCount);
																				}
																				else{
																					(*ancora).refereTabela = CriarSimboloFuncao($2,FUNC_TABLE,NULL,realEscopo,tipoRetorno);
																				}
																				if(numArgumentos > 0){
																					ancoraSimb = (*ancora).refereTabela;
																					(*ancora).funcArgsTypes = (int*)malloc(numArgumentos * sizeof(int));
																					PreencheFuncArgs($5,0,(*ancora).funcArgsTypes);
																				}
																				(*ancora).refereTabela->numArgs = numArgumentos;
																				(*ancora).valor = strdup($2);
																				free($2);
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				$$ = ancora;
																			}

	|	VOID ID  
																			{ //Declaração de função é um pouco estranha, pq o escopo da função é diferente dos argumentos
																				escopoCounter++;
																				Push(pilhaEscopo,CriarStack(escopoCounter));
																			}
		OPENPAR funcargs CLOSEPAR OPENCURLY statement CLOSECURLY 			{
																				int numArgumentos;
																				numArgumentos = ContaFuncArgs($5);
																				$1 = NULL;
																				$4 = NULL;
																				$6 = NULL;
																				$7 = NULL;
																				$9 = NULL;
																				no* ancora = (no*)malloc(sizeof(no));
																				int realEscopo;
																				(*ancora).numFilhos = 2;
																				(*ancora).filhos[0] = $5;
																				(*ancora).filhos[1] = $8;
																				(*ancora).tipo = YYSYMBOL_function_declaration;
																				char ancora2[] = "void";
																				(*ancora).nome = strdup(ancora2);
																				simbolo *ancoraSimb = ProcurarTabela($2);
																				Pop(pilhaEscopo);
																				realEscopo = Top(pilhaEscopo)->valor;
																				if(ancoraSimb != NULL){
																					printf("ERRO SEMANTICO! ID %s REDECLARADO COMO FUNCAO! LINHA: %d, COLUNA: %d \n",$2,linhaCount,colunaCount);
																				}
																				else{
																					(*ancora).refereTabela = CriarSimboloFuncao($2,FUNC_TABLE,NULL,realEscopo,Void);
																				}
																				if(numArgumentos > 0){
																					ancoraSimb = (*ancora).refereTabela;
																					(*ancora).funcArgsTypes = (int*)malloc(numArgumentos * sizeof(int));
																					PreencheFuncArgs($5,0,(*ancora).funcArgsTypes);
																				}
																				(*ancora).refereTabela->numArgs = numArgumentos;
																				(*ancora).valor = strdup($2);
																				free($2);
																				(*ancora).conversion = None;
																				(*ancora).tipoVirtual = 0;
																				$$ = ancora;
																			}
	;
	
	
assignment:
		ID ASSIGN mathop													{
																				no* ancora = (no*)malloc(sizeof(no));
																				(*ancora).numFilhos = 1;
																				(*ancora).tipo = YYSYMBOL_assignment;
																				(*ancora).filhos[0] = $3;
																				char ancora2[] = "ID";
																				(*ancora).nome = strdup(ancora2);
																				simbolo *ancoraSimb = VerificarEscopo($1);
																				if(ancoraSimb != NULL){ 
																					(*ancora).refereTabela = ancoraSimb;
																					(*ancora).tipoVirtual = ConverteTableTipo((*ancoraSimb).tipo);
																					if(($3)->tipoVirtual == (*ancora).tipoVirtual){
																						(*ancora).conversion = None;
																					}
																					else if(($3)->tipoVirtual == Float && (*ancora).tipoVirtual == Int){
																						(*ancora).conversion = FloatToIntLeft;
																					}
																					else if(($3)->tipoVirtual == Int && (*ancora).tipoVirtual == Float){
																						(*ancora).conversion = IntToFloatLeft;
																					}
																					else if((*ancora).tipoVirtual == Elem){
																						(*ancora).conversion = DecideConversao(Elem,($3)->tipoVirtual,($3)->tipoVirtual);
																					}
																					else{
																						printf("ERRO SEMANTICO! NAO HA COMO CONVERTER ESTA EXPRESSAO PARA O ID %s! Linha: %d, Coluna: %d\n",$1,linhaCount,colunaCount);
																						(*ancora).conversion = None;
																						(*ancora).tipoVirtual = 0;
																					}
																				}
																				else{
																					printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO!\n",$1);
																					(*ancora).refereTabela = NULL;
																					(*ancora).conversion = None;
																					(*ancora).tipoVirtual = 0;
																				}
																				(*ancora).valor = strdup($1);
																				free($1);
																				$2 = NULL;
																				$$ = ancora;
																			}
	;

variable_declaration:
		type ID															{
																			no* ancora = (no*)malloc(sizeof(no));
																			(*ancora).numFilhos = 1;
																			(*ancora).filhos[0] = $1;
																			(*ancora).tipo = YYSYMBOL_variable_declaration;
																			char ancora2[] = "variable_declaration";
																			(*ancora).nome = strdup(ancora2);
																			simbolo *ancoraSimb = ProcurarTabelaEscopo($2,(Top(pilhaEscopo)->valor));
																			if(ancoraSimb != NULL){
																				printf("ERRO SEMANTICO! VARIAVEL %s REDECLARADA! LINHA: %d, COLUNA: %d\n",$2,linhaCount,colunaCount);
																			}
																			else{
																				(*ancora).refereTabela = CriarSimbolo($2,atoi(((no*)$1)->valor),NULL,escopoCounter);
																			}
																			(*ancora).valor = strdup($2);
																			(*ancora).conversion = None;
																			(*ancora).tipoVirtual = 0;
																			free($2);
																			$$ = ancora;
																		}
	|	type error															{
																			no* ancora = (no*)malloc(sizeof(no));
																			(*ancora).numFilhos = 1;
																			(*ancora).filhos[0] = $1;
																			char ancora2[] = "ERROR";
																			(*ancora).nome = strdup(ancora2);
																			(*ancora).tipo = YYSYMBOL_variable_declaration;
																			(*ancora).refereTabela = NULL;
																			(*ancora).valor = NULL;
																			(*ancora).conversion = None;
																			(*ancora).tipoVirtual = 0;
																			$$ = ancora;
																			yyerrok;
																		}
/*		INT ID SEMICOLON
	|	FLOAT ID SEMICOLON
	|	SET ID SEMICOLON
	|	ELEM ID SEMICOLON
*/	;

mathop:
		mathop PLUS mathop1			{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 2;
										(*ancora).filhos[0] = $1;
										(*ancora).filhos[1] = $3;
										char ancora2[] = "plus";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
										(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
										$$ = ancora;
										$2 = NULL;
									}
	|	mathop MINUS mathop1 		{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 2;
										(*ancora).filhos[0] = $1;
										(*ancora).filhos[1] = $3;
										char ancora2[] = "minus";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
										(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
										$$ = ancora;
										$2 = NULL;
									}
	|	mathop1						{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = $1;
										char ancora2[] = "mathop1";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ($1)->tipoVirtual;
										$$ = ancora;
									}
	;

mathop1:
		mathop1 AST mathop2			{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 2;
										(*ancora).filhos[0] = $1;
										(*ancora).filhos[1] = $3;
										char ancora2[] = "ast";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop1;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).tipoVirtual = DecideTipo(($1)->tipoVirtual,($3)->tipoVirtual);
										(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
										$$ = ancora;
										$2 = NULL;
									}
	|	mathop1 BS mathop2			{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 2;
										(*ancora).filhos[0] = $1;
										(*ancora).filhos[1] = $3;
										char ancora2[] = "bs";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop1;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).tipoVirtual = Float; //Melhor que toda divisão resulte em um float
										(*ancora).conversion = DecideConversao(($1)->tipoVirtual,($3)->tipoVirtual,(*ancora).tipoVirtual);
										$$ = ancora;
										$2 = NULL;
									}
	|	mathop2						{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = $1;
										char ancora2[] = "mathop2";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop1;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ($1)->tipoVirtual;
										$$ = ancora;
									}
	;


mathop2:
		matharg						{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = $1;
										char ancora2[] = "matharg";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop2;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ($1)->tipoVirtual;
										$$ = ancora;
									}
	|	OPENPAR mathop CLOSEPAR		{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = $2;
										char ancora2[] = "pars";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_mathop2;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ($2)->tipoVirtual;
										$$ = ancora;
										$1 = NULL;
										$3 = NULL;
									}
	;

matharg:
		ID							{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "ID";
										(*ancora).nome = strdup(ancora2);
										simbolo *ancoraSimb = VerificarEscopo($1);
										if(ancoraSimb != NULL){ 
											(*ancora).refereTabela = ancoraSimb;
										}
										else{
											printf("ERRO SEMANTICO! ID %s USADO FORA DE ESCOPO!\n",$1);
											(*ancora).refereTabela = NULL;
										}
										(*ancora).valor = strdup($1);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ConverteTableTipo((*ancoraSimb).tipo);
										free($1);
										$$ = ancora;																
									}

	|	num 						{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = $1;
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "num";
										(*ancora).nome = strdup(ancora2);
										(*ancora).valor = NULL;
										(*ancora).refereTabela = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ($1)->tipoVirtual;
										$$ = ancora;																
									}
	|	function_call				{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 1;
										(*ancora).filhos[0] = $1;
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "function_call";
										(*ancora).nome = strdup(ancora2);
										(*ancora).valor = NULL;
										(*ancora).refereTabela = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = ($1)->tipoVirtual;
										$$ = ancora;																
									}

	|	EMPTY 						{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										(*ancora).tipo = YYSYMBOL_matharg;
										char ancora2[] = "empty";
										(*ancora).nome = strdup(ancora2);
										(*ancora).valor = strdup($1);
										(*ancora).refereTabela = NULL;
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Set;
										$$ = ancora;																
									}
	;



type:
		SET 						{
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
										$$ = ancora;
										$1 = NULL;
									}
	|	INT 						{
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
										$$ = ancora;
										$1 = NULL;
									}
	|	ELEM 						{
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
										$$ = ancora;
										$1 = NULL;
									}
	|	FLOAT 						{
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
										$$ = ancora;
										$1 = NULL;
									}
	;

num:
		INTEGER 					{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "integer";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_num;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = strdup($1);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Int;
										$$ = ancora;
										free($1);
									}

	|	FLOATING 					{
										no* ancora = (no*)malloc(sizeof(no));
										(*ancora).numFilhos = 0;
										char ancora2[] = "floating";
										(*ancora).nome = strdup(ancora2);
										(*ancora).tipo = YYSYMBOL_num;
										(*ancora).refereTabela = NULL;
										(*ancora).valor = strdup($1);
										(*ancora).conversion = None;
										(*ancora).tipoVirtual = Float;
										$$ = ancora;
										free($1);
									}
	;


%%
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
	ApagarTabela();
	LimparStack(pilhaEscopo);
	
	yylex_destroy();
	return 0;
}

