#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definicoes.h"

#define YYSYMBOL_statement  54
#define YYSYMBOL_write  60                
#define YYSYMBOL_writeln  61   

extern no* raiz;
extern simbolo* tabelaSimbolos;

             

/*
Certo, a ideia vai ser percorrer a árvore sintática, gerando primeiro a tabela de símbolos. Nela vamos colocar todas as variáveis, e as strings.
Para evitar conflito de nomes, todos os nomes gerados começam com '_'


*/
int stringCounter;

void PercorrerArvoreString(FILE *arq, no* alvo){
	int j;
	if((*alvo).tipo == YYSYMBOL_write || (*alvo).tipo == YYSYMBOL_writeln){
		if(!strcmp((*alvo).nome,"string")){
			fprintf(arq,"char _string%d [] = \"%s\"\n",stringCounter,(*alvo).valor);
			stringCounter++;
		}
		else if(!strcmp((*alvo).nome,"char")){
			fprintf(arq,"char _char%d = '%s'\n",stringCounter,(*alvo).valor);
			stringCounter++;
		}
	}
	
	for(j = 0; j < (*alvo).numFilhos;j++){
		PercorrerArvoreString(arq,(*alvo).filhos[j]);
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
		if((*alvo).escopo == 0 && (*alvo).tipo != FUNC_TABLE){//Para que o sistema de escopo funcione, o id de cada variavel é seguido do número de seu escopo
			switch((*alvo).tipo){
				case INT_TABLE:
				fprintf(saida,"int %s%d = %s\n",(*alvo).nome,(*alvo).escopo,(*alvo).valor);
				break;

				case FLOAT_TABLE:
				fprintf(saida,"float %s%d = %sf\n",(*alvo).nome,(*alvo).escopo,(*alvo).valor);
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

	stringCounter = 0;
	PercorrerArvoreString(saida,raiz);

	fprintf(saida,".code\n");

	/*
	Certo, agora acabou a parte fácil...
	Temos que percorrer a árvore sintática, traduzindo cada statement em código TAC equivalente.

	os statements possíveis são:

		single line

		{ statement }
			-Esta construção só afeta escopos, os quais já são lidados pela tabela(nome em tac = nome + escopo)
		
		function declaration
			-Uma declaração de função vai tirar argumentos da pilha e usá-los de diversos modos.
				*Cada variável de função está na tabela de símbolos, com um valor de ordemFuncArgs. As chamadas de função vão empilhar seus argumentos e fazer um jump.
					A partir daqui, ao invés de usar as variáveis da tabela de símbolos, usamos #x, para indicar o x-esimo valor da pilha, sendo x = ordemFuncArgs.
		for
			-O for precisa fazer 3 coisas:
				*fazer um assignment
				--depois de executar o statement
				*realizar a comparação booleana listada. Se for verdadeira, prossiga para o próximo passo. Se não, saia.
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
