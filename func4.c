#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilidades.h"
#include "func.h"


/**
 * Funcionalidade 4: buscas em arquivos de dados e de índice.
 * Esta função lê o nome de um arquivo de dados de pessoas e um arquivo de índice.
 * Em seguida, ela executa 'n' operações de busca, onde cada busca é especificada
 * por um campo (ex: "idPessoa") e um valor.
 * A estratégia de busca muda com base no campo:
 * 1.  Se o campo for "idPessoa", a busca é OTIMIZADA, usando o arquivo de índice
 * para encontrar rapidamente a posição do registro no arquivo de dados.
 * 2.  Para qualquer outro campo ("nomePessoa", "idadePessoa", etc.), a busca é SEQUENCIAL,
 * lendo o arquivo de dados registro por registro do início ao fim.
 */
void func4 () {
    // --- PREPARAÇÃO E LEITURA INICIAL ---
    int n; // Número de buscas que serão realizadas
    char nameFileIndice[MAX_STRING_TAMANHO], nameFilePessoa[MAX_STRING_TAMANHO];
    
    // Ler do terminal os nomes do arquivo e o número de buscas a serem realizadas
    scanf(" %s %s", nameFilePessoa, nameFileIndice);
    scanf("%d", &n);

    // Abre os arquivos para leitura binária ("rb")
    FILE *fpPessoa = fopen(nameFilePessoa, "rb");
    FILE *fpIndice = fopen(nameFileIndice, "rb");

    // Verificar os arquivos
    if (verificaArquivo(fpIndice) == 0) {
        return;
    } 
    if (verificaArquivo(fpPessoa) == 0) {
        fclose(fpIndice);
        return;
    }

    // Leitura e verificação dos cabeçalhos

    // Lendo o cabeçalho do arquivo de índice
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice);
    // Lendo o cabeçalho do arquivo de pessoas
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    // Verificando a consistência dos arquivos
    if (verificaConsistenciaArquivo(fpIndice, 1) == 0) {
        return;
    }
    if (verificaConsistenciaArquivo(fpPessoa, 2) == 0) {
        return;
    }

    // Alocando vetor de offsets
    // Alocando um vetor que será utilizado em todas as 'n' buscas
    // O tamanho máximo de resultados de uma busca sequencial é qtdPessoas
    long long *offsetsEncontrados = (long long*)malloc(headerPessoa.qtdPessoas * sizeof(long long));
    if (offsetsEncontrados == NULL && headerPessoa.qtdPessoas > 0) {
        printf("Falha ao alocar memória para busca.\n");
        fclose(fpIndice);
        fclose(fpPessoa);
        return;
    }

    // --- LOOP PRINCIPAL DE BUSCAS ---
    for(int i = 0; i < n; i++) {
        int temp; 
        scanf("%d", &temp); // Consumir do terminal o número da linha
        
        char nomeCampo[MAX_STRING_TAMANHO];
        char valorCampo[MAX_STRING_TAMANHO];
        lerCriteriosDeBusca(nomeCampo, valorCampo);
        
        int numEncontrados;

        // --- CHAMADA DA FUNÇÃO DE BUSCA CENTRALIZADA ---
        numEncontrados = buscarPessoas(fpPessoa, fpIndice, &headerPessoa, 
                                       nomeCampo, valorCampo, offsetsEncontrados);

        // --- PROCESSAMENTO DOS RESULTADOS ---
        if (numEncontrados == 0) {
            printf("Registro inexistente.\n\n");
        } 
        else {
            // Itera sobre os offsets encontrados, lê e imprime cada registro
            for (int j = 0; j < numEncontrados; j++) {
                // Posiciona o ponteiro no offset encontrado
                fseek(fpPessoa, offsetsEncontrados[j], SEEK_SET);
                
                RegistroPessoa pessoa;
                lerRegistroPessoa(fpPessoa, &pessoa); // Lê o registro completo
                imprimePessoa(pessoa); // Imprime formatado
            }
        }
    }

    // --- FINALIZAÇÃO ---
    free(offsetsEncontrados); // Libera o vetor de offsets

    fclose(fpIndice);
    fclose(fpPessoa);
}

