enum typeConversion{None, IntToFloatLeft, IntToFloatRight, IntToFloatBoth, FloatToIntLeft, FloatToIntRight, FloatToIntBoth, ElemToIntLeft, ElemToIntRight, ElemToFloatLeft, ElemToFloatRight, ElemToIntBoth, ElemToFloatBoth};

enum typeVirtual{Untyped, Void, Int, Float, Elem, Set};

#define CHAR_TABLE 2
#define INT_TABLE 3
#define FLOAT_TABLE 4
#define STRING_TABLE 5
#define FUNC_TABLE 6
#define ELEM_TABLE 7
#define SET_TABLE 8

#define TRUE 1
#define FALSE 0

typedef struct simbolo{				//elemento da tabela de simbolos. Contém ponteiros para os simbolos seguinte e anterior, o nome do simbolo, o valor associado a ele(em string,se tiver), qual tipo

	struct simbolo* anterior;		//de simbolo é (vide defines acima), e o tamanho do valor(caso exista)
	struct simbolo* seguinte;
	char *nome;
	char *valor;
	char **funcArgs;
	int ordemFuncArgs;
	int tipo;
	int tamanhoValor;
	int escopo;
	int returnType;
	int *funcArgsTypes;
	int numArgs;
	int escopoArgs;
	} simbolo;
	

typedef struct no{						//Elemento da árvore sintática! Contém um vetor de filhos e o número de quantos existem dentro.
	struct no* filhos[5];
	int numFilhos;

	int tipo;					//Contém também um número único que identifica qual variável foi usada para criar este nó(números definidos pelo próprio Bison)

	simbolo* refereTabela;		//Para uso futuro, caso o nó contenha referência a um ID, guardamos um ponteiro para onde ele se localiza na tabela de símbolos
	char *valor;				//Contém o valor associado ao item abaixo, caso exista. Para números, contém o número em string. Para IDs, contém o nome do ID, etc.
	char *nome;					//Contém uma string que identifica qual específica transição da variável foi usada para gerar este nó.
	int escopo;
	int conversion;
	int tipoVirtual;
} no;		


typedef struct pilha{
	struct pilha* seguinte;
	struct pilha* anterior;
	int valor;
} pilha;

