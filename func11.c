#include <stdio.h>
#include <stdlib.h>
#include "utilidades.h" // Para as structs e funções auxiliares
#include "func.h"       // Para a declaração da func11


// FUNÇÃO PARA DEBUGGAR CODIGO E ACHAR ERROS --> NÃO É PARTE DO TRABALHO
/**
 * Funcionalidade 11: Imprimir todos os registros do arquivo de índice.
 * * Esta função é utilitária para depuração. Ela abre um arquivo de índice,
 * lê seu cabeçalho, e depois itera por todos os registros de dados (id, byteOffset),
 * imprimindo-os no terminal.
 */
void func11() {
    // --- PREPARAÇÃO E LEITURA INICIAL ---
    char nameFileIndice[MAX_STRING_TAMANHO];
    
    // Ler do terminal o nome do arquivo de índice
    printf("Digite o nome do arquivo de índice que deseja ler: ");
    scanf(" %s", nameFileIndice);

    // Abre o arquivo para leitura binária ("rb")
    FILE *fpIndice = fopen(nameFileIndice, "rb");

    // Verificar o arquivo
    if (verificaArquivo(fpIndice) == 0) {
        // A função verificaArquivo já imprime a mensagem de falha
        return;
    } 

    // --- LEITURA E VERIFICAÇÃO DO CABEÇALHO ---
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice); //

    // Verificando a consistência do arquivo
    // Vamos pular a verificação de consistência para podermos depurar
    // até mesmo arquivos inconsistentes ('0').
    // Apenas avisamos o usuário.
    if (headerIndice.status == '0') {
        printf("Aviso: O arquivo de índice está marcado como INCONSISTENTE ('0').\n");
    } else {
        printf("Arquivo de índice marcado como CONSISTENTE ('1').\n");
    }

    // --- LEITURA E IMPRESSÃO DOS REGISTROS ---
    
    // Pega o tamanho total do arquivo para saber onde parar
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoArquivo = ftell(fpIndice);
    
    // Reposiciona o ponteiro logo após o cabeçalho (definido em utilidades.h)
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET); //

    RegistroIndice registro;
    int contador = 0;

    printf("\n--- Imprimindo Registros do Arquivo de Índice (%s) ---\n", nameFileIndice);
    printf("------------------------------------------------------\n");

    // Loop principal: lê enquanto a posição atual for menor que o fim do arquivo
    while (ftell(fpIndice) < tamanhoArquivo) {
        // Lê o próximo registro de índice (id, byteOffSet)
        lerRegistroIndice(fpIndice, &registro); //
        
        // Imprime os dados formatados
        // O %-10d alinha o ID à esquerda com 10 espaços, facilitando a leitura
        printf("Registro %03d: ID = %-10d | ByteOffset = %lld\n", 
               contador, registro.id, registro.byteOffSet);
        
        contador++;
    }

    // --- VERIFICAÇÃO FINAL ---
    if (contador == 0) {
        printf("Arquivo de índice não contém registros de dados (apenas cabeçalho).\n");
    }

    printf("------------------------------------------------------\n");
    printf("Total de registros lidos: %d\n", contador);

    // --- FINALIZAÇÃO ---
    fclose(fpIndice);
}