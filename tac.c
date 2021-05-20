#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definicoes.h"

#define YYSYMBOL_statement  54
#define YYSYMBOL_write  60                
#define YYSYMBOL_writeln  61   

extern no* raiz;
extern simbolo* tabelaSimbolos;

int contadorGeral;//Contador para podermos gerar simbolos únicos sempre que necessário
int jumpCounter;             
int inFunctionDeclaration;
/*
Certo, a ideia vai ser percorrer a árvore sintática, gerando primeiro a tabela de símbolos. Nela vamos colocar todas as variáveis, e as strings.
Para evitar conflito de nomes, todos os nomes gerados começam com '_'


*/

int TACMathop(no* alvo, FILE* arq);
int TACComparison(no* alvo, FILE* arq);
void TACStatement(no* alvo, FILE* arq);
void TACIf(no* alvo, FILE* arq);

int VerificaArgumento(char* alvo, int escopo){
	//Verifica se no escopo indicado existe uma função com argumento = alvo.
	int i;
	simbolo* ancoraSimb = tabelaSimbolos;
	//char* ancoraString;
	while(ancoraSimb != NULL){
		if((*ancoraSimb).tipo == FUNC_TABLE && (*ancoraSimb).escopo == escopo){
			for(i = 0;i < (*ancoraSimb).numArgs;i++){
				if(!strcmp(alvo,(*ancoraSimb).funcArgs[i])){
					return i;
				}
			}
		}
		ancoraSimb = (*ancoraSimb).seguinte;
	}
	return -1;//Não achei
}



void DecideConversaoTAC(FILE* arq, no* alvo, int arg1, int arg2){
	int value = (*alvo).conversion;
	if(value == IntToFloatLeft || value == IntToFloatRight){
		fprintf(arq,"inttofl $%d, $%d\n", arg1, arg1);
	}
	else if(value == IntToFloatBoth){
		fprintf(arq,"inttofl $%d, $%d\n", arg1, arg1);
		fprintf(arq,"inttofl $%d, $%d\n", arg2, arg2);
	}
	else if(value == FloatToIntLeft || value == FloatToIntRight){
		fprintf(arq,"fltoint $%d, $%d\n", arg1, arg1);
	}
	else if(value == FloatToIntBoth){
		fprintf(arq,"fltoint $%d, $%d\n", arg1, arg1);
		fprintf(arq,"fltoint $%d, $%d\n", arg2, arg2);
	}
}

/*
Um problema que enfrentamos é o de tratar strings. O TAC precisa que as strings estejam declaradas
na tabela para imprimir sem problemas. Então, precisamos de algum jeito de correlacionar o texto de uma string
com seu nome na .table, sem estourar algum limite de tamanho de nome dos simbolos da .table, ou causar conflito de nomes.

Aqui podemos fazer uma pequena gambiarra. Os nós sintáticos que usam estas strings não usam o campo de escopo, então podemos roubá-lo
para este propósito.
*/
void PercorrerArvoreString(FILE *arq, no* alvo){
	int j;
	if((*alvo).tipo == YYSYMBOL_write || (*alvo).tipo == YYSYMBOL_writeln){
		if(!strcmp((*alvo).nome,"string")){
			fprintf(arq,"char _string%d [] = \"%s\"\n",(*alvo).escopo,(*alvo).valor);
		}
		else if(!strcmp((*alvo).nome,"char")){
			fprintf(arq,"char _char%d = '%s'\n",(*alvo).escopo,(*alvo).valor);
		}
	}
	
	for(j = 0; j < (*alvo).numFilhos;j++){
		PercorrerArvoreString(arq,(*alvo).filhos[j]);
	}
}

void TACStackArgs(no* alvo, FILE* arq){
	//char* ancora = (*alvo).nome;
	char* ancoraFilho = (*alvo).filhos[1]->nome;
	int arg = TACMathop((*alvo).filhos[0],arq);

	fprintf(arq,"push $%d\n",arg);

	if(!strcmp(ancoraFilho,"comma")){
		TACStackArgs((*alvo).filhos[1]->filhos[1],arq);
	}
}


void TACFunctionCall(no* alvo, FILE* arq){//Função espera que você vá dar pop depois de chamá-la
	char* ancora = (*alvo).nome;
	simbolo* ancoraSimb = (*alvo).refereTabela;
	int escopoAlvo = (*ancoraSimb).escopo;

	if(!strcmp(ancora,"function_call")){
		TACStackArgs((*alvo).filhos[0],arq);
	}
	fprintf(arq,"call %s%d\n",(*alvo).valor,escopoAlvo);
}


int TACMathop2(no* alvo, FILE* arq){
	char* ancora = (*alvo).nome;
	int final;
	int ancoraArg;
	no* filho;


	if(!strcmp(ancora,"pars")){
		final = TACMathop((*alvo).filhos[0],arq);
	}
	else{//Aqui temos, finalmente, os números das operações aritméticas! Este nó pode ser uma variável, 
			//funcion call, ou número. Como nossa estrutura espera um número de temporário a ser colocado, vamos precisar colocar valores de funções e variáveis em temporários.
		contadorGeral++;
		final = contadorGeral;
		filho = (*alvo).filhos[0];
		ancora = (*filho).nome;//<----- ANCORA AGORA É O NOME DO FILHO!

		if(!strcmp(ancora,"ID")){
			if(inFunctionDeclaration){
				ancoraArg = VerificaArgumento((*filho).valor,(*filho).escopo);
				if(ancoraArg != -1){
					fprintf(arq,"mov $%d, #%d\n",final,ancoraArg);
				}
			}
			else{
				fprintf(arq,"mov $%d, %s%d\n",final,(*filho).valor,(*filho).refereTabela->escopo);
			}
			
		}
		else{
			filho = (*filho).filhos[0];
			if(!strcmp(ancora,"num")){
				fprintf(arq,"mov $%d, %s\n",final,(*filho).valor);
			}
			else{
				TACFunctionCall(filho,arq);
				fprintf(arq,"pop $%d\n",final);
			}
		}
	}
	return final;

}


int TACMathop1(no* alvo, FILE* arq){
	char* ancora = (*alvo).nome;
	int final;
	int arg1,arg2;
	if(!strcmp(ancora,"mathop2")){
		final = TACMathop2((*alvo).filhos[0],arq);
	}
	else{
		contadorGeral++;
		final = contadorGeral;
		arg1 = TACMathop1((*alvo).filhos[0],arq);
		arg2 = TACMathop2((*alvo).filhos[1],arq);
		DecideConversaoTAC(arq,alvo,arg1,arg2);
		if(!strcmp(ancora,"ast")){
			fprintf(arq,"mul $%d, $%d, $%d\n",final,arg1,arg2);
		}
		else{
			fprintf(arq,"div $%d, $%d, $%d\n",final,arg1,arg2);
		}
	}

	return final;

}


int TACMathop(no* alvo, FILE* arq){
	char* ancora = (*alvo).nome;
	int final;
	int arg1,arg2;
	if(!strcmp(ancora,"mathop1")){
		final = TACMathop1((*alvo).filhos[0],arq);
	}
	else{
		contadorGeral++;
		final = contadorGeral;
		arg1 = TACMathop((*alvo).filhos[0],arq);
		arg2 = TACMathop1((*alvo).filhos[1],arq);
		DecideConversaoTAC(arq,alvo,arg1,arg2);
		if(!strcmp(ancora,"plus")){
			fprintf(arq,"add $%d, $%d, $%d\n",final,arg1,arg2);
		}
		else{
			fprintf(arq,"sub $%d, $%d, $%d\n",final,arg1,arg2);
		}

	}
	return final;
}



int TACComparg(no* alvo, FILE* arq){
	contadorGeral++;
	int final = contadorGeral;
	char* ancora = (*alvo).nome;

	if(!strcmp(ancora,"comparison")){
		//Aqui tem uma coisa perigosa. Para que o TACComparison coloque o resultado no temp correto, precisamos reduzir 
		//o contador geral antes de chamar a função
		contadorGeral--;
		TACComparison((*alvo).filhos[0],arq);
	}	
	else{
		(void)TACMathop((*alvo).filhos[0],arq);
	}
	
	return final;

}

int TACComparison(no* alvo, FILE* arq){//Aqui apareceu um problema. Precisamos que o valor do cálculo volte para o temporário inicial pra podermos retornar.
	char* ancora = (*alvo).nome;
	contadorGeral++;
	int final = contadorGeral;
	int arg1,arg2;
	if(!strcmp(ancora,"not")){
		arg1 = TACComparg((*alvo).filhos[0],arq);
		DecideConversaoTAC(arq,alvo,final,0);
		fprintf(arq, "not $%d, $%d\n",final,final);
	}
	else {
		arg1 = TACComparg((*alvo).filhos[0],arq);
		arg2 = TACComparg((*alvo).filhos[1],arq);

		DecideConversaoTAC(arq,alvo,arg1,arg2);

		if(!strcmp(ancora,"and")){
			fprintf(arq, "and $%d, $%d, $%d\n",final,arg1,arg2);
		}
		else if(!strcmp(ancora,"or")){
			fprintf(arq, "or $%d, $%d, $%d\n",final,arg1,arg2);
		}
		else if(!strcmp(ancora,"greater")){
			fprintf(arq, "slt $%d, $%d, $%d\n",final,arg2,arg1);
		}
		else if(!strcmp(ancora,"greater_or_equal")){
			fprintf(arq, "sleq $%d, $%d, $%d\n",final,arg2,arg1);
		}
		else if(!strcmp(ancora,"less")){
			fprintf(arq, "slt $%d, $%d, $%d\n",final,arg1,arg2);
		}
		else if(!strcmp(ancora,"less_or_equal")){
			fprintf(arq, "sleq $%d, $%d, $%d\n",final,arg1,arg2);
		}
		else if(!strcmp(ancora,"equal")){
			fprintf(arq, "seq $%d, $%d, $%d\n",final,arg1,arg2);
		}
		else if(!strcmp(ancora,"not_equal")){
			fprintf(arq, "seq $%d, $%d, $%d\n",final,arg1,arg2);
			fprintf(arq, "not $%d, $%d\n",final,final);
		}
	}
	return final;
}


void TACReturn(no* alvo, FILE* arq){
	char* ancora = (*alvo).nome;
	int final;
	contadorGeral++;
	if(!strcmp(ancora,"comparison")){
		TACComparison((*alvo).filhos[0],arq);
		fprintf(arq,"return $%d\n",contadorGeral);
	}
	else if(!strcmp(ancora,"mathop")){
		final = TACMathop((*alvo).filhos[0],arq);
		fprintf(arq,"return $%d\n",final);
	}
	else{
		fprintf(arq,"return\n");
	}
}

void TACWrite(no* alvo, FILE* arq){
	char* item;
	int final;
	char* ancora = (*alvo).nome;
	contadorGeral++;

	if(!strcmp(ancora,"mathop")){
		final = TACMathop((*alvo).filhos[0],arq);
		fprintf(arq,"print $%d\n",final);
	}
	else{
		if(!strcmp(ancora,"string")){
			item = "_string";
		}
		else{
			item = "_char";
		}


		fprintf(arq,"mov $%d, %d\n",contadorGeral,(int)strlen((*alvo).valor));
		fprintf(arq,"mov $%d, 0\n",contadorGeral+1);
		fprintf(arq, "_L%d\n",jumpCounter);
		jumpCounter++;
		fprintf(arq,"mov $%d, &%s%d\n",contadorGeral+2,item,(*alvo).escopo);
		fprintf(arq,"mov $%d, [$%d]\n",contadorGeral+2,contadorGeral+1);
		fprintf(arq,"print $%d\n",contadorGeral+2);
		fprintf(arq, "add $%d, $%d, 1\n",contadorGeral+1,contadorGeral+1);
		fprintf(arq,"sub $%d, $%d, $%d\n",contadorGeral+3,contadorGeral,contadorGeral+1);
		fprintf(arq, "brnz _L%d\n", jumpCounter-1);
	}

	contadorGeral--;	
}

void TACWriteLn(no* alvo, FILE* arq){
	int final = contadorGeral + 1;
	TACWrite(alvo,arq);
	fprintf(arq,"mov $%d, _endl\n",final);
	fprintf(arq,"print $%d\n",final);
}

void TACAssignment(no* alvo, FILE* arq){
	contadorGeral++;
	int result = TACMathop((*alvo).filhos[0],arq);
	DecideConversaoTAC(arq,alvo,result,0);
	fprintf(arq,"mov %s%d, $%d\n",(*alvo).valor,(*alvo).refereTabela->escopo,result);
	contadorGeral--;
}

void TACRead(no* alvo, FILE* arq){
	simbolo* ancoraSimb = (*alvo).refereTabela;

	switch((*ancoraSimb).tipo){
		case FLOAT_TABLE:
		fprintf(arq,"scanf %s%d\n",(*alvo).valor,(*ancoraSimb).escopo);
		break;

		case INT_TABLE:
		fprintf(arq,"scani %s%d\n",(*alvo).valor,(*ancoraSimb).escopo);
		break;

		case CHAR_TABLE:
		fprintf(arq,"scanc %s%d\n",(*alvo).valor,(*ancoraSimb).escopo);
		break;
	}
}

void TACSingleLine(no* alvo, FILE* arq){
	char* ancora = (*alvo).nome;
	if(!strcmp(ancora,"return")){
		TACReturn((*alvo).filhos[0],arq);
	}
	else if(!strcmp(ancora,"assignment")){
		TACAssignment((*alvo).filhos[0],arq);
	}
	else if(!strcmp(ancora,"write")){
		TACWrite((*alvo).filhos[0],arq);
	}
	else if(!strcmp(ancora,"writeln")){
		TACWriteLn((*alvo).filhos[0],arq);
	}
	else if(!strcmp(ancora,"read")){
		TACRead((*alvo).filhos[0],arq);
	}

}

/*
Puts, agora pensei numa coisa: como vai funcionar a passagem de parâmetros? Até agora, assumimos que todos os valores vão
ser variáveis, disponíveis em diversos escopos, então só usamos o nome da variável. Mas agora podemos empilhar valores???
Teríamos que, dentro das outras funções TAC, toda vez que usamos um id, ver se o escopo desse símbolo 


IDEIA vou roubar usando a tabela de simbolos. Os argumentos das funções vão pra lá(vide funcargs), então teremos variáveis na tabela
com nome "xxxd", xxx = nome do argumento, d = escopo do interior da função. Então, ao invés de empilhar argumentos diretamente,
vamos "empilhar" colocando eles na tabela

NÃO FUNCIONA PRA FUNÇÃO RECURSIVA FOCK
e pior que o teste clássico de programa é fibonacci

vamos ter q usar a pilha mesmo então

SOLUÇÃO fazer uma função que percorra a subárvore da declaração de uma função, que é chamada depois dela ser executada.
Usa o ponteiro do 

TBM NÃO FUNCIONA pq eu já escrevi no maldito arquivo FOCK DE NOVO

PRONTO coloquei os nomes dos argumentos na tabela num vetor de strings. Agora o TAC procura por esses nomes qdo vai colocar um argumento,
e se ver que está dentro de uma função com esse ID como argumento, ele coloca #x no lugar, sendo x a posição dele no vetor.

*/

void TACFunctionDeclaration(no* alvo, FILE* arq){
	char* ancora = (*alvo).valor;
	inFunctionDeclaration = TRUE;
	if(!strcmp(ancora,"main")){
		fprintf(arq,"_main:\n");
	}
	else{
		fprintf(arq,"%s%d:\n",ancora,(*alvo).refereTabela->escopo);
	}
	TACStatement((*alvo).filhos[2],arq);
	inFunctionDeclaration = FALSE;
}



void TACFor(no* alvo, FILE* arq){
	int forStart = jumpCounter;
	int forEnd;
	int comp;

	TACAssignment((*alvo).filhos[0],arq);
	fprintf(arq,"_L%d:\n",forStart);
	jumpCounter++;
	forEnd = jumpCounter;
	jumpCounter++;

	comp = TACComparison((*alvo).filhos[1],arq);

	fprintf(arq,"brz _L%d, $%d\n",forEnd,comp);

	TACStatement((*alvo).filhos[3],arq);

	TACAssignment((*alvo).filhos[2],arq);

	fprintf(arq,"jump _L%d\n",forStart);
	
	fprintf(arq,"_L%d:\n",forEnd);
}

void TACElse(no* alvo, FILE* arq){
	char* ancora = (*alvo).nome;
	//char* ancoraValor = (*alvo).valor;

	if(!strcmp(ancora,"epsilon")){
		return;
	}
	else if(!strcmp(ancora,"if")){
		TACIf((*alvo).filhos[0],arq);
	}
	else if(!strcmp(ancora,"curly")){
		TACStatement((*alvo).filhos[0],arq);
	}
	else{
		TACSingleLine((*alvo).filhos[0],arq);
	}
}

void TACIf(no* alvo, FILE* arq){
	int comp;
	int ifEnd = jumpCounter;
	char* ancora = (*alvo).nome;
	//char* ancoraValor = (*alvo).valor;

	jumpCounter++;

	comp = TACComparison((*alvo).filhos[0],arq);
	fprintf(arq,"brz _L%d, $%d\n",ifEnd,comp);
	
	if(!strcmp(ancora,"if")){
		TACStatement((*alvo).filhos[1],arq);
	}
	else{
		TACSingleLine((*alvo).filhos[1],arq);
	}
	fprintf(arq,"_L%d:\n",ifEnd);
	TACElse((*alvo).filhos[2],arq);
}

void TACStatement(no* alvo, FILE* arq){
	no* atual = alvo;

	if(!strcmp((*atual).nome,"single_line_statement")){
			TACSingleLine((*atual).filhos[0],arq);
			TACStatement((*atual).filhos[1],arq);
		}
		else if(!strcmp((*atual).nome,"function_declaration")){
			TACFunctionDeclaration((*atual).filhos[0],arq);
			TACStatement((*atual).filhos[1],arq);
		}
		else if(!strcmp((*atual).nome,"for")){
			TACFor((*atual).filhos[0],arq);
			TACStatement((*atual).filhos[1],arq);
		}
		else if(!strcmp((*atual).nome,"if")){
			TACIf((*atual).filhos[0],arq);
			TACStatement((*atual).filhos[1],arq);
		}
		else if(!strcmp((*atual).nome,"variable_declaration")){
			TACStatement((*atual).filhos[1],arq);
		}
		else if(!strcmp((*atual).nome,"curly")){
			TACStatement((*atual).filhos[0],arq);
		}

}

void ConverterTac(){

	FILE *saida;

	saida = fopen("out.tac","w+");
	if(saida == NULL){
		printf("ERRO! NAO CONSEGUI CRIAR ARQUIVO DE SAIDA!\n");
		return;
	}
	fprintf(saida,".table\n");

	simbolo *alvo = tabelaSimbolos;
	while(alvo != NULL){
		if((*alvo).tipo != FUNC_TABLE){//Para que o sistema de escopo funcione, o id de cada variavel é seguido do número de seu escopo
			switch((*alvo).tipo){
				case INT_TABLE:
				fprintf(saida,"int %s%d = 0\n",(*alvo).nome,(*alvo).escopo);
				break;

				case FLOAT_TABLE:
				fprintf(saida,"float %s%d = 0.0f\n",(*alvo).nome,(*alvo).escopo);
				break;

				case CHAR_TABLE:
				fprintf(saida,"char %s%d = '%s'\n",(*alvo).nome,(*alvo).escopo,(*alvo).valor);
				break;

				case STRING_TABLE:
				fprintf(saida,"char %s%d [] = \"%s\"\n",(*alvo).nome,(*alvo).escopo,(*alvo).valor);
				break;

				default:
				break;
			}
		}
		alvo = (*alvo).seguinte;
	}

	/*
	Agora que colocamos todos os simbolos globais, temos que procurar pelas strings usadas no código.
	Devido a necessidades do TAC, elas tem que estar na tabela :/
	*/

	contadorGeral = 0;
	jumpCounter = 0;
	inFunctionDeclaration = FALSE;
	PercorrerArvoreString(saida,raiz);
	fprintf(saida,"char _endl = '\\n'\n");
	fprintf(saida,".code\n");


	TACStatement(raiz,saida);
	/*
	while(atual != NULL){
		if(!strcmp((*atual).nome,"single_line_statement")){
			TACSingleLine((*atual).filhos[0],saida);
			atual = (*atual).filhos[1];
		}
		else if(!strcmp((*atual).nome,"function_declaration")){
			TACFunctionDeclaration((*atual).filhos[0],saida);
			atual = (*atual).filhos[1];
		}
		else if(!strcmp((*atual).nome,"for")){
			TACFor((*atual).fillhos[0],saida);
			atual = (*atual).filhos[1];
		}
		else if(!strcmp((*atual).nome,"if")){
			TACIf((*atual).filhos[0],saida);
			atual = (*atual).filhos[1];
		}
		else if(!strcmp((*atual).nome,"variable_declaration")){
			atual = (*atual).filhos[1];
		}
		else if(!strcmp((*atual).nome,"curly")){
			atual = (*atual).filhos[0];
		}
		else if(!strcmp((*atual).nome,"epsilon")){
			atual = NULL;
		}

	}
	*/

	fprintf(saida,"main:\n");//Main do TAC. A main do arquivo de entrada terá seu nome convertido para "_main"
	fprintf(saida,"call _main\n");


	/*
	Certo, agora acabou a parte fácil...
	Temos que percorrer a árvore sintática, traduzindo cada statement em código TAC equivalente.

	os statements possíveis são:

		DONE single line
			Entre estes:
				DONE return
					Voltar pra de onde foi chamado. Se tiver argumento, colocá-lo na pilha.

				DONE assignment
					Colocar um valor a uma variavel depois de resolver uma mathop

				DONE read
					Dependendo do tipo da variável, usamos 
						scanc
						scani
						scanf

				DONE WRITE(LN)
					write(string/char)
					writeln(string/char)

					"mov $0, %d\n", strlen(valor).
					"mov $1,0\n"
					"_L%d:\n",contadorGeral; contadorGeral++;
					"mov $2, &%s\n," Aqui temos que colocar o nome do ID, ou o nome na .table desta string(("_string" ou "_char") + escopo).
					"mov $2,[$1]\n"
					"print $2\n"
					"add $1, $1, 1\n"
					"sub $3, $0, $1\n"
					"brnz _L%d\n",contadorGeral - 1;

					SE FOR WRITELN, ADICIONE

					"mov $0, _endl\n"
					"print $0\n"


					write(mathop)
					writeln(mathop)
					...resolve mathop, colocando resultado em $0...
					"print $0\n"
					ou
					"println $0\n"



		{ statement }
			-Esta construção só afeta escopos, os quais já são lidados pela tabela(nome em tac = nome + escopo)
		
		function declaration
			-O escopo vai mudar no TAC. Representamos isso resetando o contadorGeral
			-Uma declaração de função vai tirar argumentos da pilha e usá-los de diversos modos.
				*Cada variável de função está na tabela de símbolos, com um valor de ordemFuncArgs. As chamadas de função vão empilhar seus argumentos e fazer um jump.
					A partir daqui, ao invés de usar as variáveis da tabela de símbolos, usamos #x, para indicar o x-esimo valor da pilha, sendo x = ordemFuncArgs.
		for
			-O for precisa fazer 3 coisas:
				*fazer um assignment
				*realizar a comparação booleana listada. Se for verdadeira, prossiga para o próximo passo. Se não, saia.
				*executar o statement
				*realizar o último assignment
		if
			-If pode resultar em n caminhos lógicos diferentes. Temos q pensar de passo a passo.
				*Se a comparação listada resultar em V, executar o segmento abaixo.
				*Se for F, pular o segmento abaixo.
					*Se houver um Else, pular para o statement do else
					*Se houver um Else if, pular o segmento abaixo e repetir este processo do ponto de vista do novo if

		iteração

		variable declaration
			-Acho que posso completamente ignorar as declarações de variável.

		empty :/
			-Faço nada ué

		a raiz deve apontar para um nó statement dentre os acima. Cada nó deve
		ter outro statement em seguida.



	*/






	fclose(saida);
}
