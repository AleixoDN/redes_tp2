#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* O QUE EST� 'FEITO'
 * 	O PROGRAMA RECEBE A STRING, CONVERTE PARA BIT
 *	VERIFICA O NEGOCIO DE ERRO DE ACORDO COM A VARIAVEL TIPODECONTROLEDEERRO
 *	ENVIA PARA O MEIO DE COMUNICA��O
 *	DO MEIO DE COMUNICA��O ENVIA PARA O RECEPTOR
 *	QUE TRANSFORMA OS BITS EM CARACTERES E IMPRIME A STRING RECEBIDA
 */

/* O QUE PRECISA SER FEITO
 *	ADICIONAR A VERIFICA��O DE ERRO CRC-32 PARA O ENVIO
 *	AJEITAR A FUN��O DO MEIO DE COMUNICA��O QUE SIMPLESMENTE TA ENVIANDO SEM ERRO
 * NENHUM OS BITS
 *	FAZER A VERIFICA��O DE ERROS NAS FUNCOES DE CONTROLE DE ERRO DO RECEPTOR
 *
 *	E TESTAR MT PQ ACABEI DE ACHAR UM ERRO ESCREVENDO ISSO. � ISSO!
 */

int tipoDeControleDeErro = 0;

void AplicacaoTransmissora();
void CamadaDeAplicacaoTransmissora(char *mensagem);
void CamadaElaceDadosTransmissora(int *quadro, int tamBit);

void CamadaElaceDadosTransmissoraControleDeErro(int **quadro, int *tamBit);
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(int **quadro, int *tamBit);
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadeImpar(int **quadro, int *tamBit);
void CamadaEnlaceDadosTransmissoraControleDeErroCRC(int **quadro, int *tamBit);

void MeioDeComunicacao(int *quadro, int tamBit);

void CamadaElaceDadosReceptoraControleDeErro(int **quadro, int *tamBit);
void CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(int **quadro, int *tamBit);
void CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar(int **quadro, int *tamBit);
void CamadaEnlaceDadosReceptoraControleDeErroCRC(int **quadro, int *tamBit);

void CamadaEnlaceDadosReceptora(int *quadro, int tamBit);
void CamadaDeAplicacaoReceptora(int *quadro, int tamBit);
void AplicacaoReceptora(char *mensagem);

void main() {
	AplicacaoTransmissora();

	return;
}

int *CharParaBit(char letra) {
	int bits[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int *ajuda = malloc(8 * sizeof(int));
	int num = letra, j = 0;
	int resto;

	while(num != 1) {
		resto = num % 2;
		bits[j++] = resto;
		num = num / 2;
	}
	bits[j] = 1;

	for(j = 0; j < 8; j++) {
		ajuda[j] = bits[7 - j];
	}

	return ajuda;
}

int *StringParaBit(char *string) {
	int tam = strlen(string) - 1, i, j, k;
	int *bits = (int*)malloc(tam * 8 * sizeof(int));
	unsigned char byte1, byte2;
	int *ajuda;

	if (bits == NULL) {
		return 0;
	}


	j = 0;
	for (i = 0; i < tam; i++) {
		ajuda = CharParaBit(string[i]);

		for (k = 0; k < 8; k++) {
			bits[j++] = ajuda[k];
		}
	}

	return bits;
}

char BitParaChar(int *bits) {
	char caractere;
	int valor = 0, i;

	for (i = 0; i < 8; i++) {
		valor += bits[i] * pow(2, 7 - i);
	}

	caractere = valor;

	return caractere;
}

char *BitParaString(int *bits, int tamBit) {
	char *string, carac;
	int *ajuda = (int*)malloc(8 * sizeof(int));
	int i, j, tamanho = tamBit / 8, k;

	string = (char*) malloc(tamanho * sizeof(char));

	k = 0;
	for (i = 0; i < tamanho; i++) {
		for (j = 0; j < 8; j++) {
			ajuda[j] = bits[(i * 8) + j];
		}

		carac = BitParaChar(ajuda);

		string[k++] = carac;
	}

	return string;
}

void AplicacaoTransmissora() {
	char mensagem[1024];

	fgets(mensagem, 1024, stdin);

	printf("\nA mensagem envidada foi: %s", mensagem);

	CamadaDeAplicacaoTransmissora(mensagem);
}

void CamadaDeAplicacaoTransmissora(char *mensagem) {
	int *bits, i, tam = (strlen(mensagem) - 1) * 8;

	bits = StringParaBit(mensagem);

	/*printf("\nBits string:\n");
	for (i = 0; i < tam; i++) {
		printf("%d", bits[i]);
	}
	printf("\n");*/

	CamadaElaceDadosTransmissora(bits, tam);
}

void CamadaElaceDadosTransmissora(int *quadro, int tamBit) {
	int i;

	CamadaElaceDadosTransmissoraControleDeErro(&quadro, &tamBit);

	printf("\nNumero de bits da mensagem: %d\n", tamBit);
	printf("Mensagem em bits: ");
	for (i = 0; i < tamBit; i++) {
		printf("%d", quadro[i]);
	}
	printf("\n");

	MeioDeComunicacao(quadro, tamBit);
}

void CamadaElaceDadosTransmissoraControleDeErro(int **quadro, int *tamBit) {
	int i;

	switch (tipoDeControleDeErro) {
		case 0:
			CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(quadro, tamBit);
			break;
		case 1:
			CamadaEnlaceDadosTransmissoraControleDeErroBitParidadeImpar(quadro, tamBit);
			break;
		case 2:
			// CRC
			break;
	}
}

void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(int **quadro, int *tamBit) {
	int i;
	int valor = 0;

	for (i = 0; i < (*tamBit); i++) {
		valor += (*quadro)[i];
	}

	(*quadro) = (int*) realloc((*quadro), ((*tamBit) + 1) * sizeof(int));

	if (valor % 2 == 0) {
		(*quadro)[(*tamBit)] = 0;
	}
	else {
		(*quadro)[(*tamBit)] = 1;
	}

	(*tamBit)++;
}

void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadeImpar(int **quadro, int *tamBit) {
	int i;
	int valor = 0;

	for (i = 0; i < (*tamBit); i++) {
		valor += (*quadro)[i];
	}

	(*quadro) = (int*) realloc((*quadro), ((*tamBit) + 1) * sizeof(int));

	if (valor % 2 == 0) {
		(*quadro)[(*tamBit)] = 1;
	}
	else {
		(*quadro)[(*tamBit)] = 0;
	}

	(*tamBit)++;
}

void CamadaEnlaceDadosTransmissoraControleDeErroCRC(int **quadro, int *tamBit) {

}

void MeioDeComunicacao(int *quadro, int tamBit) {
	int erro, porcentagemDeErros;
	int *fluxoBrutoDeBitsPontoA, *fluxoBrutoDeBitsPontoB;
	int tamanhoB;

	porcentagemDeErros = 0;

	fluxoBrutoDeBitsPontoA = quadro;

	while (tamanhoB != tamBit) {
		fluxoBrutoDeBitsPontoB = fluxoBrutoDeBitsPontoA;
		tamanhoB = tamBit;
	}

	CamadaEnlaceDadosReceptora(fluxoBrutoDeBitsPontoB, tamBit);
}

void CamadaElaceDadosReceptoraControleDeErro(int **quadro, int *tamBit) {

}

void CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(int **quadro, int *tamBit) {

}

void CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar(int **quadro, int *tamBit) {

}

void CamadaEnlaceDadosReceptoraControleDeErroCRC(int **quadro, int *tamBit) {

}

void CamadaEnlaceDadosReceptora(int *quadro, int tamBit) {
	CamadaElaceDadosReceptoraControleDeErro(&quadro, &tamBit);

	CamadaDeAplicacaoReceptora(quadro, tamBit - 1);
}

void CamadaDeAplicacaoReceptora(int *quadro, int tamBit) {
	char *mensagem;
	int i;

	mensagem = BitParaString(quadro, tamBit);

	AplicacaoReceptora(mensagem);
}

void AplicacaoReceptora(char *mensagem) {
	printf("\nA mensagem recebida foi: %s\n\n", mensagem);
}
