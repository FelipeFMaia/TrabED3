#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilidades.h"
#include "func.h"

/**
 * Busca binária para encontrar o PRIMEIRO registro 'segue' com o ID buscado.
 * Retorna o RRN (índice do registro) ou -1 se não encontrar.
 * (Arquivo segue.bin é de registros de tamanho fixo, ordenado pela func9)
 */
int buscaBinariaPrimeiroSegue(FILE *fpSegue, int idBuscado, int numRegistros) {
    int inicio = 0, fim = numRegistros - 1, meio;
    int rrnEncontrado = -1; // RRN (Record Relative Number)

    if (numRegistros == 0) {
        return -1;
    }

    while (inicio <= fim) {
        meio = inicio + (fim - inicio) / 2;
        
        // Posicionar no registro do meio (Tamanho Fixo)
        long long byteOffsetDoMeio = TAMANHO_CABECALHO_SEGUE + (long long)meio * TAMANHO_REGISTRO_SEGUE;
        fseek(fpSegue, byteOffsetDoMeio, SEEK_SET);
        
        RegistroSegue regSegue;
        lerRegistroSegue(fpSegue, &regSegue);

        // Compara o ID lido com o ID que estamos buscando
        if (regSegue.idPessoaQueSegue < idBuscado) {
            inicio = meio + 1;
        } else if (regSegue.idPessoaQueSegue > idBuscado) {
            fim = meio - 1;
        } else {
            // Achamos um! Mas pode não ser o primeiro.
            rrnEncontrado = meio;
            fim = meio - 1; // Tenta achar um registro anterior com o mesmo ID
        }
    }
    
    // Retorna o RRN (índice) do primeiro registro que encontrou
    return rrnEncontrado; 
}


/**
 * Imprime os dados de um registro 'segue' no formato da Funcionalidade 10.
 */
void imprimeSegue(RegistroSegue segue) {
    // ID da pessoa que é seguida
    printf("Segue a pessoa de codigo: ");
    if (segue.idPessoaQueESeguida == -1) {
        printf("-\n");
    } else {
        printf("%d\n", segue.idPessoaQueESeguida);
    }
    
    // Justificativa (grauAmizade)
    printf("Justificativa para seguir: ");
    if (segue.grauAmizade == '0') {
        printf("celebridade\n");
    } else if (segue.grauAmizade == '1') {
        printf("amiga de minha amiga\n");
    } else if (segue.grauAmizade == '2') {
        printf("minha amiga\n");
    } else {
        printf("-\n"); // Tratar caso nulo ($)
    }

    // Data Início
    printf("Comecou a seguir em: ");
    if (segue.dataInicioQueSegue[0] == '$') {
        printf("-\n");
    } else {
        // Imprime os 10 bytes da data (já está formatada DD/MM/AAAA)
        printf("%.10s\n", segue.dataInicioQueSegue);
    }

    // Data Fim
    printf("Parou de seguir em: ");
    if (segue.dataFimQueSegue[0] == '$') {
        printf("-\n");
    } else {
        // Imprime os 10 bytes da data
        printf("%.10s\n", segue.dataFimQueSegue);
    }
    
    printf("\n"); // Linha em branco entre cada registro 'segue'
}


/**
 * Funcionalidade 10: Junção (JOIN) entre pessoa.bin e segueOrdenado.bin
 * 1.  Busca registros em 'pessoa.bin' usando a func4 (buscarPessoas).
 * 2.  Para cada pessoa encontrada, imprime seus dados.
 * 3.  Usa o 'idPessoa' para fazer uma busca binária em 'segueOrdenado.bin'.
 * 4.  Imprime todos os registros 'segue' correspondentes.
 */
void func10() {
    // --- PREPARAÇÃO E LEITURA INICIAL ---
    int n; // Número de buscas (junções) que serão realizadas
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];
    
    // Lê os nomes dos três arquivos e o número de buscas
    scanf(" %s %s %s %d", nameFilePessoa, nameFileIndice, nameFileSegue, &n);

    // Abre os três arquivos para leitura binária ("rb")
    FILE *fpPessoa = fopen(nameFilePessoa, "rb");
    FILE *fpIndice = fopen(nameFileIndice, "rb");
    FILE *fpSegue = fopen(nameFileSegue, "rb");

    // Verificação de segurança (um para cada arquivo)
    if (verificaArquivo(fpPessoa) == 0) {
        return; 
    }
    if (verificaArquivo(fpIndice) == 0) {
        fclose(fpPessoa);
        return;
    }
    if (verificaArquivo(fpSegue) == 0) {
        fclose(fpPessoa);
        fclose(fpIndice);
        return;
    }

    // --- LEITURA E VERIFICAÇÃO DOS CABEÇALHOS ---
    
    // Lendo cabeçalho do arquivo de Pessoas
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    // Lendo cabeçalho do arquivo de Índice
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice);

    // Lendo cabeçalho do arquivo Segue
    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpSegue, &headerSegue);

    // Verificando a consistência dos três arquivos
    // if (verificaConsistenciaArquivo(fpPessoa, 2) == 0) {
    //     return;
    // }
    // if (verificaConsistenciaArquivo(fpIndice, 1) == 0) {
    //     return;
    // }
    //  if (verificaConsistenciaArquivo(fpSegue, 3) == 0) {
    //     return;
    // }

    // --- ALOCAÇÃO DO VETOR DE OFFSETS ---
    // (Igual fizemos na func4, para guardar os resultados da buscaPessoas)
    long long *offsetsEncontrados = (long long*)malloc(headerPessoa.qtdPessoas * sizeof(long long));
    if (offsetsEncontrados == NULL && headerPessoa.qtdPessoas > 0) {
        printf("Falha ao alocar memória para busca.\n");
        fclose(fpPessoa);
        fclose(fpIndice);
        fclose(fpSegue);
        return;
    }

    // --- LOOP PRINCIPAL DE BUSCAS (JUNÇÕES) ---
    for(int i = 0; i < n; i++) {
        int temp; 
        scanf("%d", &temp); // Consumir do terminal o número da linha
        
        char nomeCampo[MAX_STRING_TAMANHO];
        char valorCampo[MAX_STRING_TAMANHO];
        lerCriteriosDeBusca(nomeCampo, valorCampo);
        
        int numEncontradosPessoa;
        int encontrouAlgumaPessoa = 0; // Flag para a msg "Registro inexistente"

        // 1. BUSCAR AS PESSOAS (usando a super-função)
        numEncontradosPessoa = buscarPessoas(fpPessoa, fpIndice, &headerPessoa, 
                                             nomeCampo, valorCampo, offsetsEncontrados);

        if (numEncontradosPessoa == 0) {
            printf("Registro inexistente.\n");
            continue; // Pula para a próxima busca 'n'
        }

        // 2. ITERAR SOBRE AS PESSOAS ENCONTRADAS
        for (int j = 0; j < numEncontradosPessoa; j++) {
            
            // Posiciona e lê a pessoa
            fseek(fpPessoa, offsetsEncontrados[j], SEEK_SET);
            RegistroPessoa pessoa;
            lerRegistroPessoa(fpPessoa, &pessoa);

            // 3. IMPRIME OS DADOS DA PESSOA
            imprimePessoa(pessoa); // (Formato da func3/4)

            int idParaJuncao = pessoa.idPessoa;

            // 4. BUSCA BINÁRIA NO ARQUIVO SEGUE
            int rrnInicial = buscaBinariaPrimeiroSegue(fpSegue, idParaJuncao, headerSegue.quantidadePessoas);

            // 5. PROCESSAR RESULTADOS DO ARQUIVO SEGUE
            if (rrnInicial != -1) {
                // Encontramos o primeiro. Vamos ler sequencialmente a partir dele
                // até o ID mudar.
                long long byteOffsetAtual = TAMANHO_CABECALHO_SEGUE + (long long)rrnInicial * TAMANHO_REGISTRO_SEGUE;
                fseek(fpSegue, byteOffsetAtual, SEEK_SET);

                RegistroSegue regSegue;
                
                // Loop de leitura sequencial após a busca binária
                for (int k = rrnInicial; k < headerSegue.quantidadePessoas; k++) {
                    
                    lerRegistroSegue(fpSegue, &regSegue);

                    // Se o ID ainda bate E o registro é válido ('1')
                    if (regSegue.idPessoaQueSegue == idParaJuncao) {
                        if (regSegue.removido == '0') { // '0' para não removido
                            imprimeSegue(regSegue);
                        }
                    } else {
                        // O ID mudou, significa que já lemos todos
                        // os registros 'segue' dessa pessoa.
                        break; 
                    }
                }
            }
            // Se rrnInicial == -1, a pessoa não segue ninguém,
            // então não imprimimos nada (só os dados da pessoa).
        }
    } // Fim do loop for (de n buscas)


    // --- FINALIZAÇÃO ---
    free(offsetsEncontrados); // Libera o vetor de offsets
    fclose(fpPessoa);
    fclose(fpIndice);
    fclose(fpSegue);
}