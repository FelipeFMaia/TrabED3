#define MAX_STRING_TAMANHO 100
#define CABECALHO_INDICE_TAMANHO 12
#define CABECALHO_PESSOA_TAMANHO 17

#include <stdio.h>
#include <stdlib.h>
#include "utilidades.h"
#include "func.h"

int removerPessoaPorIndice (FILE *fpIndice, FILE *fpPessoa, int idBuscado) {
    int encontrou = 0;

    // Pegar o tamanho do arquivo
    fseek(fpIndice, 0, SEEK_END);
    int tamanhoBytesArquivo = ftell(fpIndice);
    fseek(fpIndice, 0,SEEK_SET);

    // Ler cabeçalho do arquivo de índice
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice);

    // Instanciar o registro do arquivo de índice
    RegistroIndice registroIndice;

    
    // Loop para ler o arquivo de índice
    while (ftell(fpIndice) < tamanhoBytesArquivo) {

        // Lê o par (id, byteOffset) do arquivo de índice
        lerRegistroIndice(fpIndice, &registroIndice);

        // Compara o ID lido com o ID que estamos buscando
        if(registroIndice.id == idBuscado) {
            encontrou = 1;
            break; // Sai do loop com o byteoffset
        }
    }

    // Se, depois de ler o arquivo de índice inteiro, não achamos o ID...
    if (encontrou == 0) {
        return 0; // ID não encontrado
    }

    // ler cabeçalho da arquivo de dados
    CabecalhoPessoa headerPessoa;
    fseek(fpPessoa, 0, SEEK_SET); // cursor para o início
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    // Se achamos, byteOffset agora contém a localização exata do registro
    // no arquivo de dados fpPessoa
    fseek(fpPessoa, registroIndice.byteOffSet, SEEK_SET); // Damos um "salto" direto para a posição correta

    // Agora, lemos os dados da pessoa do arquivo principal
    RegistroPessoa pessoa;
    int removido = lerRegistroPessoa(fpPessoa, &pessoa);

    // se já estiver removido
    if(removido == 1) {
        return 0;
    }
    // se não estiver removido
    else if (removido == 0) {
        // reposicionar o cursor para o início do registro e fazer a remocao
        fseek(fpPessoa, registroIndice.byteOffSet, SEEK_SET);
        removerRegistroPessoa(fpPessoa);
        // Atualizar cabecalho
        headerPessoa.qtdPessoas -= 1;
        headerPessoa.qtdRemovidos += 1;
        escreveCabecalhoPessoa(fpPessoa, headerPessoa);
    }

    return 1;
}

int removerPessoaSequencial (FILE *fpPessoa, char *nomeCampo, char *valorCampo) {
    // Colocar o curso no inicio do arquivo
    fseek(fpPessoa, 0, SEEK_SET);

    // Instanciar cabecalho do arquivo de dados e ler o cabecalho
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    int encontrou = 0, removido;
    RegistroPessoa pessoa;
    long long posicao;

    while ((posicao = ftell(fpPessoa)) < headerPessoa.proxByteOffSet) {
        removido = lerRegistroPessoa(fpPessoa, &pessoa);

        if (pessoa.removido == '1') {
            continue; // Foi removido logicamente
        }

        // Agora, verificamos se este é o registro que procuramos
        int correspondeu = correspondeRegistro(nomeCampo, valorCampo, pessoa);
        
        encontrou += correspondeu;

        // se encontramos, então removemos logicamente
        if (correspondeu == 1) {
            // cursor no inicio do registro
            fseek(fpPessoa, posicao, SEEK_SET);
            removerRegistroPessoa(fpPessoa); // remover logicamente
            // Atualizar cabecalho
            headerPessoa.qtdPessoas -= 1;
            headerPessoa.qtdRemovidos += 1;
            // cursor para o proximo registro
            fseek(fpPessoa, posicao + 5 + pessoa.tamanhoRegistro , SEEK_SET);
        }
        
    }
    // atualizar o cabecalho do arquivo aqui para maior eficiência
    escreveCabecalhoPessoa(fpPessoa, headerPessoa);

    if (encontrou == 0) {
        return 0;
    }
    else {
        return 1;
    }

}


/**
 * Funcionalidade 5: buscas e remoção de registros
 * Esta função lê o nome de um arquivo de dados de pessoas e um arquivo de índice.
 * Em seguida, ela executa 'n' operações de remoção, onde cada remoção é especificada
 * por um campo (ex: "idPessoa") e um valor.
 * A estratégia de busca muda com base no campo:
 * 1.  Se o campo for "idPessoa", a busca e remoção é OTIMIZADA, usando o arquivo de índice
 * para encontrar rapidamente a posição do registro no arquivo de dados e fazendo a remoção.
 * 2.  Para qualquer outro campo ("nomePessoa", "idadePessoa", etc.), a busca é SEQUENCIAL,
 * lendo o arquivo de dados registro por registro do início ao fim e fazendo a remoção quando corresponder.
 */

void func5 () {
    // --- PREPARAÇÃO E LEITURA INICIAL ---
    
    int n; // Número de buscas que serão realizadas
    char nameFileIndice[MAX_STRING_TAMANHO], nameFilePessoa[MAX_STRING_TAMANHO];
    
    // Lê os nomes dos arquivos de dados e de índice
    scanf(" %s %s", nameFilePessoa, nameFileIndice);
    // Lê a quantidade de remocões que o usuário quer fazer
    scanf("%d", &n);

    FILE *fpIndice, *fpPessoa;

    // Tenta abrir os dois arquivos em modo de leitura binária e escrita ("r+b")
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


    // --- ALOCAÇÃO DE VETORES AUXILIARES ---
    int maxRegistros = headerPessoa.qtdPessoas + headerPessoa.qtdRemovidos; // Tamanho seguro
    
    // Aloca vetor para guardar os offsets encontrados pela busca
    long long *offsetsEncontrados = (long long*)malloc(maxRegistros * sizeof(long long));
    // Aloca vetor para guardar os IDs que precisam ser removidos do índice
    int *idsParaRemoverDoIndice = (int*)malloc(maxRegistros * sizeof(int));
    int totalRemovidosDoIndice = 0;



    // --- LOOP PRINCIPAL DE BUSCAS ---

    // Este loop vai rodar 'n' vezes, uma para cada busca solicitada
    for(int i = 0; i < n; i++) {
        int temp; // Variável temporária
        
        scanf("%d", &temp); // Consumir do terminal o número da linha
        
        char nomeCampo[MAX_STRING_TAMANHO];
        char valorCampo[MAX_STRING_TAMANHO];

        lerCriteriosDeBusca(nomeCampo, valorCampo);

        // Número de registro encontrados de acordo com os critérios de busca
        // Os offsets dos registros encontrados são armazenados no vetor offsetsEncontrados 
        int numEncontrados = buscarPessoas(fpPessoa, fpIndice, &headerPessoa, 
                                           nomeCampo, valorCampo, offsetsEncontrados);

        // Loop para deletar todos os registros encontrados de acordo com os critérios de busca
        if (numEncontrados > 0) {
            for (int j = 0; j < numEncontrados; j++) {
                long long offsetAtual = offsetsEncontrados[j];
                
                // Ler o id antes de remover
                int idParaRemover;
                fseek(fpPessoa, offsetAtual + 1 + 4, SEEK_SET); // Pular removido e tamanho
                fread(&idParaRemover, sizeof(int), 1, fpPessoa); // Ler o idPessoa do registro a ser removido

                // Adicionar o id ao vetor de ids para remover no arquivo de índice
                idsParaRemoverDoIndice[totalRemovidosDoIndice] = idParaRemover;
                totalRemovidosDoIndice++;

                // Fazer a remoção lógica no arquivo de Pessoas
                fseek(fpPessoa, offsetAtual, SEEK_SET);
                removerRegistroPessoa(fpPessoa);

                // Atualizar o cabeçalho em memória
                headerPessoa.qtdPessoas--;
                headerPessoa.qtdRemovidos++;        
            }
        }
    } 

    // Atualizar o arquivo de índice (sem os removidos)
    // CAPTURE O PONTEIRO RETORNADO AQUI:
    fpIndice = reescreverIndiceComRemocoes(fpIndice, nameFileIndice, headerIndice, idsParaRemoverDoIndice, totalRemovidosDoIndice);

    // Adicionar verificação de segurança
    if (fpIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fpPessoa); // Fechar o outro arquivo
        free(offsetsEncontrados);
        free(idsParaRemoverDoIndice);
        return; // Abortar
    }

    // Atualizaro o cabeçalho do arquivo de Pessoas no disco
    escreveCabecalhoPessoa(fpPessoa, headerPessoa);

    // modificar a consistência dos arquivos, pois vamos fechar
    // consistente
    headerIndice.status = '1';
    atualizarConsistencia(fpIndice, headerIndice.status);
    // consistente
    headerPessoa.status = '1';
    atualizarConsistencia(fpPessoa, headerPessoa.status);

    // --- FINALIZAÇÃO ---
    // Liberar memória
    free(offsetsEncontrados);
    free(idsParaRemoverDoIndice);

    // Após todas as buscas, fechamos os arquivos
    fclose(fpIndice);
    fclose(fpPessoa);

    // usar binarioNaTela
    binarioNaTela(nameFilePessoa);
    binarioNaTela(nameFileIndice);
}

