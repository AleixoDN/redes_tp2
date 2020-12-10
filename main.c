#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int tipoDeControleDeErro = 2;
int chave[32] = {1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1};

void AplicacaoTransmissora();
void CamadaDeAplicacaoTransmissora(char *mensagem);
void CamadaElaceDadosTransmissora(int *quadro, int tamBit);

void CamadaElaceDadosTransmissoraControleDeErro(int **quadro, int *tamBit);
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(int **quadro, int *tamBit);
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadeImpar(int **quadro, int *tamBit);
void CamadaEnlaceDadosTransmissoraControleDeErroCRC(int **quadro, int *tamBit);

void MeioDeComunicacao(int *quadro, int tamBit);

short CamadaElaceDadosReceptoraControleDeErro(int **quadro, int *tamBit);
short CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(int **quadro, int *tamBit);
short CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar(int **quadro, int *tamBit);
short CamadaEnlaceDadosReceptoraControleDeErroCRC(int **quadro, int *tamBit);

void CamadaEnlaceDadosReceptora(int *quadro, int tamBit);
void CamadaDeAplicacaoReceptora(int *quadro, int tamBit, short erro);
void AplicacaoReceptora(char *mensagem, short erro);

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
		return;
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

	string = (char*) malloc((tamanho) * sizeof(char));

	k = 0;
	for (i = 0; i < tamanho; i++) {
		for (j = 0; j < 8; j++) {
			ajuda[j] = bits[(i * 8) + j];
		}

		carac = BitParaChar(ajuda);

		string[k++] = carac;
	}

	string[k] = '\0';

	return string;
}

void AplicacaoTransmissora() {
	char mensagem[1024];

	printf("Digite a mensagem a ser enviada:\n");

	fgets(mensagem, 1024, stdin);

	printf("A mensagem envidada foi: %s", mensagem);

	CamadaDeAplicacaoTransmissora(mensagem);
}

void CamadaDeAplicacaoTransmissora(char *mensagem) {
	int *quadro, i, tam = (strlen(mensagem) - 1) * 8;

	quadro = StringParaBit(mensagem);

	printf("\nBits string enviado:\n");
	for (i = 0; i < tam; i++) {
		printf("%d", quadro[i]);
	}
	printf("\n");

	CamadaElaceDadosTransmissora(quadro, tam);
}

void CamadaElaceDadosTransmissora(int *quadro, int tamBit) {
	CamadaElaceDadosTransmissoraControleDeErro(&quadro, &tamBit);

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
			CamadaEnlaceDadosTransmissoraControleDeErroCRC(quadro, tamBit);
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
	int i;
	int novoTam = (*tamBit) + 31;
	int *ajuda = (int*) malloc(novoTam * sizeof(int));
	int pos = 0;

	for (i = 0; i < novoTam; i++) {
		if (i < (*tamBit)) {
			ajuda[i] = (*quadro)[i];
		}
		else {
			ajuda[i] = 0;
		}
	}

	while (novoTam - pos >= 32) {
		if (ajuda[pos] == 1) {
			for (i = 0; i < 32; i++) {
				if (ajuda[i + pos] == chave[i]) {
					ajuda[i + pos] = 0;
				}
				else {
					ajuda[i + pos] = 1;
				}
			}
		}

		pos++;
	}

	(*quadro) = (int*) realloc((*quadro), novoTam * sizeof(int));
	(*tamBit) = novoTam;

	for (i = 0; i < 31; i++) {
		(*quadro)[pos + i] = ajuda[pos + i];
	}

	free(ajuda);
}

void MeioDeComunicacao(int *quadro, int tamBit) {
	int erro, porcentagemDeErros;
	int bitsPassados = 0;

	porcentagemDeErros = 2;

	while (bitsPassados != tamBit) {
		if ((rand() % 1000) <= porcentagemDeErros) {
			if (quadro[bitsPassados] == 0) {
				quadro[bitsPassados] = 1;
			}
			else {
				quadro[bitsPassados] = 0;
			}
		}

		bitsPassados++;
	}

	CamadaEnlaceDadosReceptora(quadro, tamBit);
}

short CamadaElaceDadosReceptoraControleDeErro(int **quadro, int *tamBit) {
	int i;
	short opcao;

	switch (tipoDeControleDeErro) {
		case 0:
			opcao = CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(quadro, tamBit);
			break;
		case 1:
			opcao = CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar(quadro, tamBit);
			break;
		case 2:
			opcao = CamadaEnlaceDadosReceptoraControleDeErroCRC(quadro, tamBit);
			break;
	}

	return opcao;
}

short CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(int **quadro, int *tamBit) {
	int valor = 0;
	int i;
	short opcao;

	for (i = 0; i < (*tamBit) - 1; i++) {
		valor += (*quadro)[i];
	}

	if (valor % 2 == 0) {
		if ((*quadro)[(*tamBit) - 1] == 0) {
			opcao = 1;
		}
		else {
			opcao = 0;
		}
	}
	else {
		if ((*quadro)[(*tamBit) - 1] == 1) {
			opcao = 1;
		}
		else {
			opcao = 0;
		}
	}

	(*tamBit)--;

	return opcao;
}

short CamadaEnlaceDadosReceptoraControleDeErroBitParidadeImpar(int **quadro, int *tamBit) {
	int valor = 0;
	int i;
	short opcao;

	for (i = 0; i < (*tamBit) - 1; i++) {
		valor += (*quadro)[i];
	}

	if (valor % 2 == 0) {
		if ((*quadro)[(*tamBit) - 1] == 1) {
			opcao = 1;
		}
		else {
			opcao = 0;
		}
	}
	else {
		if ((*quadro)[(*tamBit) - 1] == 0) {
			opcao = 1;
		}
		else {
			opcao = 0;
		}
	}

	(*tamBit)--;

	return opcao;
}

short CamadaEnlaceDadosReceptoraControleDeErroCRC(int **quadro, int *tamBit) {
	int i;
	int novoTam = (*tamBit) - 31;
	int *ajuda = (int*) malloc((*tamBit) * sizeof(int));
	int pos = 0;

	for (i = 0; i < (*tamBit); i++) {
		ajuda[i] = (*quadro)[i];
	}

	while ((*tamBit) - pos >= 32) {
		if (ajuda[pos] == 1) {
			for (i = 0; i < 32; i++) {
				if (ajuda[i + pos] == chave[i]) {
					ajuda[i + pos] = 0;
				}
				else {
					ajuda[i + pos] = 1;
				}
			}
		}

		pos++;
	}

	(*quadro) = (int*) realloc((*quadro), novoTam * sizeof(int));
	(*tamBit) = novoTam;

	for (i = 0; i < 31; i++) {
		if (ajuda[i + pos] == 1) {
			free(ajuda);

			return 0;
		}
	}

	free(ajuda);

	return 1;
}

void CamadaEnlaceDadosReceptora(int *quadro, int tamBit) {
	short erro;
	int i;

	erro = CamadaElaceDadosReceptoraControleDeErro(&quadro, &tamBit);

	CamadaDeAplicacaoReceptora(quadro, tamBit, erro);
}

void CamadaDeAplicacaoReceptora(int *quadro, int tamBit, short erro) {
	char *mensagem;
	int i;

	printf("\nBits string recebido:\n");
	for (i = 0; i < tamBit; i++) {
		printf("%d", quadro[i]);
	}
	printf("\n");

	mensagem = BitParaString(quadro, tamBit);

	AplicacaoReceptora(mensagem, erro);
}

void AplicacaoReceptora(char *mensagem, short erro) {

	if (erro == 1) {
		printf("\nNao existe erro na mensagem!");
	}
	else {
		printf("\nExiste erro na mensagem!");
	}

	printf("\nA mensagem recebida foi: %s\n", mensagem);
}

void main() {
	printf("Tipo de Erro a ser verificado: ");

	switch (tipoDeControleDeErro) {
		case 0:
			printf("Paridade Par");
			break;
		case 1:
			printf("Paridade Impar");
			break;
		case 2:
			printf("CRC");
			break;
	}

	printf("\n");

	AplicacaoTransmissora();

	return;
}
