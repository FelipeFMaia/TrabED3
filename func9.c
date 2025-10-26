#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilidades.h"
#include "func.h"


/*
Funcionalidade 9
Ordenar o arquivo
*/



// Função auxiliar para comparar inteiros (trata nulos)
int compareInts(int a, int b) {
    int nuloA = (a == -1);
    int nuloB = (b == -1);

    if (nuloA && nuloB) return 0;  // Ambos nulos, são iguais
    if (!nuloA && nuloB) return -1; // A (não nulo) vem ANTES de B (nulo)
    if (nuloA && !nuloB) return 1;  // A (nulo) vem DEPOIS de B (não nulo)
    
    // Ambos não são nulos, comparação padrão
    return a - b;
}

// Função auxiliar para comparar datas (trata nulos e formato)
int compareDatas(const char *dataA, const char *dataB) {
    int nuloA = (dataA[0] == '$');
    int nuloB = (dataB[0] == '$');

    if (nuloA && nuloB) return 0;
    if (!nuloA && nuloB) return -1; // A (não nulo) vem ANTES de B (nulo)
    if (nuloA && !nuloB) return 1;  // A (nulo) vem DEPOIS de B (não nulo)

    // Ambos não são nulos, comparar AAAA, depois MM, depois DD
    
    // Comparar Ano (pos 6, 4 chars)
    int cmpAno = strncmp(dataA + 6, dataB + 6, 4);
    if (cmpAno != 0) return cmpAno;

    // Comparar Mês (pos 3, 2 chars)
    int cmpMes = strncmp(dataA + 3, dataB + 3, 2);
    if (cmpMes != 0) return cmpMes;

    // Comparar Dia (pos 0, 2 chars)
    return strncmp(dataA, dataB, 2);
}


int funcaoDeComparacao(const void *a, const void *b) {
    // Fazer o "cast" dos ponteiros genéricos
    RegistroSegue *regA = (RegistroSegue *)a;
    RegistroSegue *regB = (RegistroSegue *)b;

    // Chamar os comparadores auxiliares na ordem de prioridade
    
    // Critério 1: idPessoaQueSegue
    int cmp = compareInts(regA->idPessoaQueSegue, regB->idPessoaQueSegue);
    if (cmp != 0) return cmp;

    // Critério 2: idPessoaQueESeguida (desempate)
    cmp = compareInts(regA->idPessoaQueESeguida, regB->idPessoaQueESeguida);
    if (cmp != 0) return cmp;

    // Critério 3: dataInicioQueSegue (desempate)
    cmp = compareDatas(regA->dataInicioQueSegue, regB->dataInicioQueSegue);
    if (cmp != 0) return cmp;

    // Critério 4: dataFimQueSegue (desempate final)
    return compareDatas(regA->dataFimQueSegue, regB->dataFimQueSegue);
}


void func9() {
    // nomes dos arquivos
    char nameFileDesordenado[MAX_STRING_TAMANHO];
    char nameFileOrdenado[MAX_STRING_TAMANHO];

    // Ler nomes dos arquivos do terminal
    scanf(" %s %s", nameFileDesordenado, nameFileOrdenado);

    // Abrir arquivo desordenado
    FILE *fpDesordenado;
    fpDesordenado = fopen(nameFileDesordenado, "rb");
    // verificar arquivo desordenado
    if (verificaArquivo(fpDesordenado) == 0) {
        return; // aborta funcionalidade
    }
    // Verificar consistência do arquivo desordenado
    int consistencia = 1;
    // int consistencia = verificaConsistenciaArquivo(fpDesordenado, 3);
    // if (consistencia == 0) { // se o arquivo estiver inconsistente
    //     fclose(fpDesordenado);
    //     return; // abortar funcionalidade
    // }

    // Criar o arquivo ordenado
    FILE *fpOrdenado;
    // verificar arquivo ordenado
    fpOrdenado = fopen(nameFileOrdenado, "wb");
    if (verificaArquivo(fpOrdenado) == 0) {
    fclose(fpDesordenado);
    return; // aborta funcionalidade
    }

    // Ler cabeçalho do arquivo desordenado, que será o mesmo para os dois
    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpDesordenado, &headerSegue);

    // Escrever o cabecalho no Ordenado
    escreverCabecalhoSegue(fpOrdenado, headerSegue);

    // Se o arquivo não tiver pessoa, não há nada a ser ordenado
    // Basta atualizar a consistência, fechar os arquivos e abortar a funcionalidade
    if (headerSegue.quantidadePessoas == 0) {
        consistencia = 1;
        atualizarConsistencia(fpOrdenado, consistencia);
        fclose(fpOrdenado);
        fclose(fpDesordenado);
        binarioNaTela(nameFileOrdenado);
        return;
    }
    // se tiver pessoas, continuar

    // Alocar um vetor de registros de Segue
    // para colocar o arquivo desordenado na memória e ordenar
    RegistroSegue *registros = (RegistroSegue*)malloc(headerSegue.quantidadePessoas * sizeof(RegistroSegue));
    if (registros == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fpDesordenado);
        fclose(fpOrdenado);
        return;
    }

    // Ler todos os registros de Segue do arquivo desordenado e salva num vetor para ordenar
    for (int i = 0; i < headerSegue.quantidadePessoas; i++) {
        lerRegistroSegue(fpDesordenado, &registros[i]);
    }

    // Uma vez que os registros estejam na RAM, podemos fechar o arquivo Desordenado
    fclose(fpDesordenado);

    // Ordenar o vetor da memória
    qsort(registros, headerSegue.quantidadePessoas, sizeof(RegistroSegue), funcaoDeComparacao);

    // Reposicionar o cursor de fpOrdenado depois do cabecalho
    fseek(fpOrdenado, TAMANHO_CABECALHO_SEGUE, SEEK_SET);

    // Uma vez ordenado, basta escrever esse vetor ordenado de registro no arquivo Ordenado
    for (int i = 0; i < headerSegue.quantidadePessoas; i++) {
        escreverRegistroSegue(fpOrdenado, registros[i]);     
    }

    // Atualizar a consistência e fechar o arquivo Ordenado
    atualizarConsistencia(fpOrdenado, '1');
    fclose(fpOrdenado);

    // Liberar memória alocada
    free(registros);

    // binario na tela do arquivo ordenado
    binarioNaTela(nameFileOrdenado);
}