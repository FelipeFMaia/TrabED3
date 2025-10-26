#define MAX_STRING_TAMANHO 100
#define CABECALHO_INDICE_TAMANHO 12
#define CABECALHO_PESSOA_TAMANHO 17

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilidades.h"
#include "func.h"


// Lê o novo registro a ser inserido
void lerNovoRegistro (RegistroPessoa *novoRegistro) {
    char buffer[MAX_STRING_TAMANHO];
    
    // Lê idPessoa
    scanf("%d,", &novoRegistro->idPessoa);

    // Lê nomePessoa (pode ser NULO ("") ou "string")
    scan_quote_string(buffer);
    if (strlen(buffer) == 0) {
        novoRegistro->tamanhoNomePessoa = 0;
    } else {
        novoRegistro->tamanhoNomePessoa = strlen(buffer);
        strcpy(novoRegistro->nomePessoa, buffer);
    }

    // Lê idadePessoa (pode ser NULO ou int)
    scanf(", %[^,]", buffer); // Lê como string
    if (strcmp(buffer, "NULO") == 0) {
        novoRegistro->idadePessoa = -1;
    } else {
        novoRegistro->idadePessoa = atoi(buffer);
    }

    // Lê nomeUsuario (pode ser NULO ("") ou "string")
    scanf(", "); // Consome a vírgula e o espaço
    scan_quote_string(buffer);
    if (strlen(buffer) == 0) {
        novoRegistro->tamanhoNomeUsuario = 0;
    } else {
        novoRegistro->tamanhoNomeUsuario = strlen(buffer);
        strcpy(novoRegistro->nomeUsuario, buffer);
    }
}



/**
 * Funcionalidade 6: Inserção de registros no arquivo de dados (pessoa.bin)
    * Encontra o primeiro registro removido que caiba o novo registro e aplica a a inserção,
preenchendo o restante do registro com $.
    * Reordenar o arquivo binario de índice após as inserções
    * 
 */
void func6 () {
    // --- PREPARAÇÃO E LEITURA INICIAL ---
    
    int numRegistrosNovos; // Número de novos registros a serem inseridos
    char nameFileIndice[MAX_STRING_TAMANHO], nameFilePessoa[MAX_STRING_TAMANHO];
    
    // Lê os nomes dos arquivos de dados e de índice
    scanf(" %s %s", nameFilePessoa, nameFileIndice);
    // Lê a quantidade de inserções que o usuário quer fazer
    scanf("%d", &numRegistrosNovos);

    FILE *fpIndice, *fpPessoa;

    // Tenta abrir os dois arquivos em modo de leitura binária e escrita ("r+b")
    // "r" é para leitura (read) e "b" é para binário (binary)
    fpPessoa = fopen(nameFilePessoa, "r+b");
    fpIndice = fopen(nameFileIndice, "r+b");

    // Verificação de segurança
    if (verificaArquivo(fpIndice) == 0) {
        return; // Aborta funcionalidade
    }
    if (verificaArquivo(fpPessoa) == 0) {
        fclose(fpIndice);
        return; // Aborta funcionalidade
    }


    // --- LEITURA E VERIFICAÇÃO DOS CABEÇALHOS ---

    // Lendo o cabeçalho do arquivo de ÍNDICE
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice);

    // Lendo o cabeçalho do arquivo de DADOS (Pessoas)
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    // Verificando a consistência dos arquivos
    // int consistencia;
    // consistencia = verificaConsistenciaArquivo(fpIndice, 1);
    // if(consistencia == 0) {
    //     return;
    // }
    // consistencia = verificaConsistenciaArquivo(fpPessoa, 2);
    // if(consistencia == 0) {
    //     return;
    // }

    // modificar a consistência dos arquivos, pois estão abertos para escrita
    // inconsistente
    headerIndice.status = '0';
    atualizarConsistencia(fpIndice, headerIndice.status);
    // inconsistente
    headerPessoa.status = '0';
    atualizarConsistencia(fpPessoa, headerPessoa.status);

    // Alocar um vetor para os novos registros de índice
    RegistroIndice *novosRegistros = (RegistroIndice*)malloc(sizeof(RegistroIndice) * numRegistrosNovos);


    // --- LOOP PRINCIPAL ---

    // Este loop vai rodar numeroRegistroNovo vezes, uma para cada registro a ser inserido
    for(int i = 0; i < numRegistrosNovos; i++) {
        int temp; // Variável temporária
        
        scanf("%d", &temp); // Consumir do terminal o número da linha
        
        // instanciar novos registros
        RegistroPessoa novoRegistroPessoa;

        // Ler (do terminal) o novo registro de pessoa a ser inserido
        lerNovoRegistro(&novoRegistroPessoa);

        // Inserir o novo registro pessoa no arquivo pessoa
        // e pegar o byteOffset dele e passar para o registro de índice
        novosRegistros[i].byteOffSet = inserirPessoa(fpPessoa, novoRegistroPessoa, &headerPessoa);
        
        // Passando o id para o registro de índice
        novosRegistros[i].id = novoRegistroPessoa.idPessoa;

    } // Fim do loop de inserções

    // Adicionar os novos registros de índice no arquivo de índices e ordenar o arquivo
    atualizarEOrdenarArquivoIndice(fpIndice, nameFileIndice, novosRegistros, numRegistrosNovos);


    // modificar a consistência dos arquivos, pois vamos fechar
    // consistente
    headerPessoa.status = '1';
    escreveCabecalhoPessoa(fpPessoa, headerPessoa);

    // --- FINALIZAÇÃO ---
    // liberar memória
    free(novosRegistros);

    // Após todas as buscas, fechamos os arquivos
    fclose(fpIndice);
    fclose(fpPessoa);

    // usar binarioNaTela
    binarioNaTela(nameFilePessoa);
    binarioNaTela(nameFileIndice);
}

