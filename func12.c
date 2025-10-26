#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // Precisamos para strncpy
#include "utilidades.h" // Para as structs CabecalhoSegue, RegistroSegue e funções de leitura
#include "func.h"       // Para a declaração da func12

// O cabeçalho do arquivo segue tem 9 bytes[cite: 48]:
// status (1) [cite: 43] + quantidadePessoas (4) [cite: 44] + proxRRN (4) [cite: 46]
#define TAMANHO_CABECALHO_SEGUE 9 

// O registro de dados do segue tem 30 bytes[cite: 80]:
// removido (1) [cite: 68] + idPessoaQueSegue (4) [cite: 70] + idPessoaQueESeguida (4) [cite: 71] +
// dataInicio (10) [cite: 72] + dataFim (10) [cite: 73] + grauAmizade (1) [cite: 74]
#define TAMANHO_REGISTRO_SEGUE 30 

/**
 * Funcionalidade 12: Imprimir todo o conteúdo do arquivo segue.bin (Header e Dados).
 * * Esta função é uma ferramenta de depuração. Ela abre um arquivo segue.bin,
 * lê e imprime seu cabeçalho formatado, e depois itera por todos os registros
 * de dados fisicamente presentes no arquivo, imprimindo cada campo.
 * * Ela também compara a contagem de registros lidos com os valores no cabeçalho
 * (quantidadePessoas e proxRRN) para expor inconsistências.
 */
void func12() {
    
    // --- PREPARAÇÃO E LEITURA INICIAL ---
    char nameFileSegue[MAX_STRING_TAMANHO];
    
    // Ler do terminal o nome do arquivo segue.bin
    printf("Digite o nome do arquivo segue.bin que deseja ler: ");
    scanf(" %s", nameFileSegue);

    // Abre o arquivo para leitura binária ("rb")
    FILE *fpSegue = fopen(nameFileSegue, "rb");

    // Verificar se o arquivo foi aberto com sucesso
    if (verificaArquivo(fpSegue) == 0) {
        // A função verificaArquivo já imprime a mensagem de falha
        return;
    } 

    // --- LEITURA E IMPRESSÃO DO CABEÇALHO ---
    CabecalhoSegue headerSegue;
    // Lê o cabeçalho campo a campo, conforme definido em utilidades.c
    lerCabecalhoSegue(fpSegue, &headerSegue); 

    printf("\n--- Imprimindo Cabeçalho do Arquivo (%s) ---\n", nameFileSegue);
    
    // Imprime o status
    if (headerSegue.status == '0') {
        printf("Status: '%c' (ARQUIVO INCONSISTENTE)\n", headerSegue.status);
    } else {
        printf("Status: '%c' (ARQUIVO CONSISTENTE)\n", headerSegue.status);
    }

    // Imprime os outros campos do cabeçalho
    printf("Quantidade de Pessoas (Registros): %d\n", headerSegue.quantidadePessoas);
    printf("Próximo RRN (Próximo Registro):   %d\n", headerSegue.proxRRN); 
    printf("---------------------------------------------------\n\n");


    // --- LEITURA E IMPRESSÃO DOS REGISTROS DE DADOS ---
    
    // Pega o tamanho total do arquivo para saber onde parar
    fseek(fpSegue, 0, SEEK_END);
    long long tamanhoArquivo = ftell(fpSegue);
    
    // Reposiciona o ponteiro logo após o cabeçalho (9 bytes)
    fseek(fpSegue, TAMANHO_CABECALHO_SEGUE, SEEK_SET); 

    RegistroSegue registro;
    int contadorRegistros = 0;
    
    // Buffers para impressão segura das strings (que não são null-terminadas)
    char dataInicio[11], dataFim[11];

    printf("--- Imprimindo Registros de Dados ---\n");
    printf("(RRN) | Rem | ID Segue   | ID Seguida | Data Início  | Data Fim     | Grau\n");
    printf("------|-----|------------|------------|--------------|--------------|-----\n");

    // Loop principal: lê enquanto a posição atual for menor que o fim do arquivo
    while (ftell(fpSegue) < tamanhoArquivo) {
        
        // Checagem de segurança: verifica se há bytes suficientes para um registro completo
        if (ftell(fpSegue) + TAMANHO_REGISTRO_SEGUE > tamanhoArquivo) {
            long bytesSobrantes = tamanhoArquivo - ftell(fpSegue);
            if (bytesSobrantes > 0) {
               printf("\nAVISO: %ld bytes de lixo (incompletos) encontrados no final do arquivo.\n", bytesSobrantes);
            }
            break; // Não há espaço para um registro completo
        }

        // Lê o próximo registro de dados campo a campo
        lerRegistroSegue(fpSegue, &registro); 
        
        // --- Impressão Segura das Strings ---
        // Copia as datas para buffers locais e garante a terminação nula
        // para que o printf funcione com segurança, sem ler memória aleatória.
        strncpy(dataInicio, registro.dataInicioQueSegue, 10);
        dataInicio[10] = '\0'; // Garante o terminador nulo
        
        strncpy(dataFim, registro.dataFimQueSegue, 10);
        dataFim[10] = '\0'; // Garante o terminador nulo
        
        // Imprime os dados formatados
        // %-10d significa alinhar o inteiro à esquerda com 10 espaços
        printf("(%03d) | '%c' | %-10d | %-10d | [%s] | [%s] | '%c'\n", 
               contadorRegistros,
               registro.removido,
               registro.idPessoaQueSegue,
               registro.idPessoaQueESeguida,
               dataInicio, // Imprime o buffer seguro
               dataFim,    // Imprime o buffer seguro
               registro.grauAmizade);
        
        contadorRegistros++;
    }

    // --- VERIFICAÇÃO FINAL E SUMÁRIO ---
    printf("--------------------------------------------------------------------------\n");
    if (contadorRegistros == 0) {
        printf("Arquivo não contém registros de dados (apenas cabeçalho).\n");
    }
    
    printf("Total de registros lidos fisicamente: %d\n", contadorRegistros);

    // Verificação de consistência: Compara contagem física com o cabeçalho
    if (contadorRegistros != headerSegue.quantidadePessoas) {
        printf("\nAVISO DE INCONSISTÊNCIA:\n");
        printf("  -> O cabeçalho (quantidadePessoas) indica %d registros.\n", headerSegue.quantidadePessoas);
        printf("  -> Mas %d registros foram lidos fisicamente.\n", contadorRegistros);
    }
    
    if (contadorRegistros != headerSegue.proxRRN) {
         printf("\nAVISO DE INCONSISTÊNCIA:\n");
        printf("  -> O cabeçalho (proxRRN) indica %d.\n", headerSegue.proxRRN);
        printf("  -> Mas %d registros foram lidos fisicamente.\n", contadorRegistros);
    }

    // --- FINALIZAÇÃO ---
    fclose(fpSegue);
}