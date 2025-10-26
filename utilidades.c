#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h>
#include "utilidades.h"

// // ----------------- Arquivo de Índice -----------------
// typedef struct{
//     char status;    // 1 byte      
//     char lixo[11];  // 11 bytes
// } CabecalhoIndice;  // 12 bytes 

// typedef struct{
//     int id;                 // 4 bytes
//     long long byteOffSet;   // 8 bytes
// } RegistroIndice;           // 12 bytes


// // ----------------- Arquivo de Dados -----------------

// typedef struct{
//     char status;                // 1 byte 
//     int qtdPessoas;             // 4 bytes
//     int qtdRemovidos;           // 4 bytes
//     long long proxByteOffSet;   // 8 bytes
// } CabecalhoPessoa;              // 17 bytes

// // Estrutura para os registros de dados em memória (pessoa.bin)
// typedef struct {
//     char removido;                          // 1 bytes
//     int tamanhoRegistro;                    // 4 bytes
//     int idPessoa;                           // 4 bytes
//     int idadePessoa;                        // 4 bytes
//     int tamanhoNomePessoa;                  // 4 bytes
//     char nomePessoa[MAX_STRING_TAMANHO];    // MAX_STRING_TAMANHO bytes
//     int tamanhoNomeUsuario;                 // 4 bytes
//     char nomeUsuario[MAX_STRING_TAMANHO];   // MAX_STRING_TAMANHO bytes
// } RegistroPessoa;

void binarioNaTela(char *nomeArquivoBinario) { 

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}

void imprimePessoa(RegistroPessoa pessoa) {
    printf("Dados da pessoa de codigo %d\n", pessoa.idPessoa);
    // Tratamento para campos que podem ser nulos ou vazios

    if (pessoa.tamanhoNomePessoa == 0) {
        printf("Nome: -\n");
    } else {
        printf("Nome: %s\n", pessoa.nomePessoa);
    }
    
    if (pessoa.idadePessoa == -1) {
        printf("Idade: -\n");
    } else {
        printf("Idade: %d\n", pessoa.idadePessoa);
    }

    if (pessoa.tamanhoNomeUsuario == 0) {
        printf("Usuario: -\n");
    } else {
        printf("Usuario: %s\n", pessoa.nomeUsuario);
    }

    printf("\n"); // Deixa uma linha em branco para separar as saídas
}

int verificaArquivo (FILE *arquivo) {
	if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0; // ERRO
	}
	else {
		return 1; // OK
	}
}

// Função para verificar a consistência
// arqID: 1) Índice 2) Pessoa 3) Segue
// Se consistente, retorna 1, senão retorna 0
int verificaConsistenciaArquivo(FILE *arquivo, int arqID) {
	// arqID == 1 indica que é para o arquivo de índice
	if (arqID == 1) {
		CabecalhoIndice headerIndice;
		lerCabecalhoIndice(arquivo, &headerIndice);
		if (headerIndice.status == '0') {
			printf("Falha no processamento do arquivo.\n");
			return 0; // retornar a inconsistência
		}
		else {
			fseek(arquivo, 0, SEEK_SET);
			return 1; // retornar a consistência
		}
	}

	// arqID == 2 indica que é para o arquivo de Pessoas
	else if (arqID == 2) {
		CabecalhoPessoa headerPessoa;
		lerCabecalhoPessoa(arquivo, &headerPessoa);
		if (headerPessoa.status == '0') {
			printf("Falha no processamento do arquivo.\n");
			return 0; // retornar a inconsistência
		}
		else {
			fseek(arquivo, 0, SEEK_SET);
			return 1; // retornar a consistência
		}
	}

	// arqID == 3 indica que é para o arquivo Segue
	else if (arqID == 3) {
		CabecalhoSegue headerSegue;
		lerCabecalhoSegue(arquivo, &headerSegue);
		if (headerSegue.status == '0') {
			printf("Falha no processamento do arquivo.\n");
			return 0; // retornar a inconsistência
		}
		else {
			fseek(arquivo, 0, SEEK_SET);
			return 1; // retornar a consistência
		}
	}

	else {
		return 0;
	}
}

void atualizarConsistencia(FILE *arquivo, char consistencia) {
	// cursor no inicio do arquivo
	fseek(arquivo, 0, SEEK_SET);
	// atualizando a consistencia
	fwrite(&consistencia, sizeof(char), 1, arquivo);
	// cursor no inicio do arquivo novamente
	fseek(arquivo, 0, SEEK_SET);
}

// -------------------------------------------------------------------------

void lerCabecalhoIndice(FILE *fpIndice, CabecalhoIndice *headerIndice) {
	fread(&headerIndice->status, sizeof(char), 1, fpIndice);
    fread(headerIndice->lixo, sizeof(char), 11, fpIndice);
}

void escreverCabecalhoIndice(FILE *fpIndice, CabecalhoIndice headerIndice) {
    // Garantir escrever no início do arquivo
    fseek(fpIndice, 0, SEEK_SET);
    // Escrever cabecalho índice
    fwrite(&headerIndice.status, sizeof(headerIndice.status), 1, fpIndice);
    fwrite(headerIndice.lixo, sizeof(headerIndice.lixo), 1, fpIndice);
}

void lerRegistroIndice(FILE *fpIndice, RegistroIndice *registroIndice) {
	fread(&registroIndice->id, sizeof(int), 1, fpIndice);
	fread(&registroIndice->byteOffSet, sizeof(long long), 1, fpIndice);
}

// Escreve o um registro de índice no arquivo de índice
void escreveRegistroIndice(FILE *fpIndice, RegistroIndice novoRegistro) {
	fwrite(&novoRegistro.id, sizeof(int), 1, fpIndice);
	fwrite(&novoRegistro.byteOffSet, sizeof(long long), 1, fpIndice);
}




// -------------------------------------------------------------------------

void lerCabecalhoPessoa(FILE *fpPessoa, CabecalhoPessoa *headerPessoa) {
    fread(&headerPessoa->status, sizeof(char), 1, fpPessoa);
    fread(&headerPessoa->qtdPessoas, sizeof(int), 1, fpPessoa);
    fread(&headerPessoa->qtdRemovidos, sizeof(int), 1, fpPessoa);
    fread(&headerPessoa->proxByteOffSet, sizeof(long long), 1, fpPessoa);
}

// Escreve o cabecalho do arquivo pessoa usando o cabecalho da memória
void escreveCabecalhoPessoa(FILE *fpPessoa, CabecalhoPessoa headerPessoa) {
    // Vai para o início do arquivo
	fseek(fpPessoa, 0, SEEK_SET);
    // Escreve
	fwrite(&headerPessoa.status, sizeof(char), 1, fpPessoa);
    fwrite(&headerPessoa.qtdPessoas, sizeof(int), 1, fpPessoa);
    fwrite(&headerPessoa.qtdRemovidos, sizeof(int), 1, fpPessoa);
    fwrite(&headerPessoa.proxByteOffSet, sizeof(long long), 1, fpPessoa);
}

int lerRegistroPessoa(FILE *fpPessoa, RegistroPessoa *pessoa) {

	fread(&pessoa->removido, sizeof(char), 1, fpPessoa);
	// Mesmo encontrando no índice, o registro pode ter sido removido logicamente
	if (pessoa->removido == '1') {
		int tamanhoRegistro;
		fread(&tamanhoRegistro, sizeof(int), 1, fpPessoa); // ler o tamanho do registro
		fseek(fpPessoa, tamanhoRegistro, SEEK_CUR);		// para pular
		return 1; // Foi removido logicamente
	}
	// caso o registro não tenha sido removido, continua a leitura
	// Se não foi removido, lemos o resto dos dados do registro
	fread(&pessoa->tamanhoRegistro, sizeof(int), 1, fpPessoa);
	fread(&pessoa->idPessoa, sizeof(int), 1, fpPessoa);
	fread(&pessoa->idadePessoa, sizeof(int), 1, fpPessoa);

	// Para campos de tamanho variável (strings), primeiro lemos o tamanho,
	// depois lemos a quantidade exata de bytes correspondente
	fread(&pessoa->tamanhoNomePessoa, sizeof(int), 1, fpPessoa);
	fread(pessoa->nomePessoa, sizeof(char), pessoa->tamanhoNomePessoa, fpPessoa);
	pessoa->nomePessoa[pessoa->tamanhoNomePessoa] = '\0'; // Adicionar o terminador de string

	fread(&pessoa->tamanhoNomeUsuario, sizeof(int), 1, fpPessoa);
	fread(pessoa->nomeUsuario, sizeof(char), pessoa->tamanhoNomeUsuario, fpPessoa);
	pessoa->nomeUsuario[pessoa->tamanhoNomeUsuario] = '\0'; // Adicionar o terminador de string
	
	// Calcular os bytes uteis lidos dos fread's
	int bytesUteisLidos = tamanhoUtilRegistro(*pessoa);

	// Calcular o tamanho do lixo no final do registro, caso tenha
	int tamanhoLixo = pessoa->tamanhoRegistro - bytesUteisLidos;

	// Pular lixo
	if (tamanhoLixo > 0) {
		fseek(fpPessoa, tamanhoLixo, SEEK_CUR);
	}

	return 0; // Não foi removido logicamente
}


void lerCabecalhoSegue(FILE *fpSegue, CabecalhoSegue *headerSegue) {
	fread(&headerSegue->status, sizeof(char), 1, fpSegue);
    fread(&headerSegue->quantidadePessoas, sizeof(int), 1, fpSegue);
    fread(&headerSegue->proxRRN, sizeof(int), 1, fpSegue);
}


// -------------------------------------------------------------------------

void lerRegistroSegue(FILE *fpSegue, RegistroSegue *registroSegue) {
    // Lê campo a campo, na ordem exata em que foi escrito
    fread(&registroSegue->removido, sizeof(char), 1, fpSegue);
    fread(&registroSegue->idPessoaQueSegue, sizeof(int), 1, fpSegue);
    fread(&registroSegue->idPessoaQueESeguida, sizeof(int), 1, fpSegue);
    fread(registroSegue->dataInicioQueSegue, sizeof(char), 10, fpSegue);
    fread(registroSegue->dataFimQueSegue, sizeof(char), 10, fpSegue);
    fread(&registroSegue->grauAmizade, sizeof(char), 1, fpSegue);
}


void escreverRegistroSegue(FILE *fpSegue, RegistroSegue regSegue) {
  // 
  fwrite(&regSegue.removido, sizeof(char), 1, fpSegue);
  //
  fwrite(&regSegue.idPessoaQueSegue, sizeof(int), 1, fpSegue);
  //
  fwrite(&regSegue.idPessoaQueESeguida, sizeof(int), 1, fpSegue);
  //
  fwrite(regSegue.dataInicioQueSegue, sizeof(char), 10, fpSegue);
  //
  fwrite(regSegue.dataFimQueSegue, sizeof(char), 10, fpSegue);
  //
  fwrite(&regSegue.grauAmizade, sizeof(char), 1, fpSegue);
}

void escreverCabecalhoSegue(FILE *fpSegue, CabecalhoSegue headerSegue) {
    // Vai para o início do arquivo
	fseek(fpSegue, 0, SEEK_SET);
    // Escreve
	fwrite(&headerSegue.status, sizeof(char), 1, fpSegue);
    fwrite(&headerSegue.quantidadePessoas, sizeof(int), 1, fpSegue);
    fwrite(&headerSegue.proxRRN, sizeof(int), 1, fpSegue);
}


// -------------------------------------------------------------------------

void lerCriteriosDeBusca (char nomeCampo[MAX_STRING_TAMANHO], char valorCampo[MAX_STRING_TAMANHO]) {
    // Ler o nome do campo a ser buscado (até encontrar o caractere '=')
    scanf(" %49[^=]%*c", nomeCampo);
    
    // Lendo o valor da busca a depender se é uma string ou não
    // Para campo de string
    if (strcmp(nomeCampo, "nomePessoa") == 0 || strcmp(nomeCampo, "nomeUsuario") == 0) {
        // Usar a funcao scan_quote_string para armazenar na string sem as aspas
        scan_quote_string(valorCampo); 
    } else {
        // Para campo numérico. Ler como string e depois usar atoi
        scanf("%s", valorCampo); 
    }

}

// -------------------------------------------------------
// Funções auxiliares de atualizaçãod de registro por campo e valor


void lerCriteriosDeAtualizacao (char *campoBusca, char *valorBusca, char *campoAtualiza, char *valorAtualiza) {
    // Leitura de campo e valor de campo que vai ser atualizado
		// Ler o nome do campo a ser buscado (até encontrar o caractere '=')
    scanf(" %49[^=]%*c", campoBusca);
    
    // Lendo o valor do campo a ser atualizado a depender se é uma string ou não
    // Para campo de string
    if (strcmp(campoBusca, "nomePessoa") == 0 || strcmp(campoBusca, "nomeUsuario") == 0) {
        // Usar a funcao scan_quote_string para armazenar na string sem as aspas
        scan_quote_string(valorBusca); 
    } else {
        // Para campo numérico. Ler como string e depois usar atoi
        scanf("%s", valorBusca); 
    }

	// Leitura de campo e valor de campo que vai atualizar
    // Ler o nome do campo a ser buscado (até encontrar o caractere '=')
    scanf(" %49[^=]%*c", campoAtualiza);
    
    // Lendo o valor do campo que vai atualizar a depender se é uma string ou não
    // Para campo de string
    if (strcmp(campoAtualiza, "nomePessoa") == 0 || strcmp(campoAtualiza, "nomeUsuario") == 0) {
        // Usar a funcao scan_quote_string para armazenar na string sem as aspas
        scan_quote_string(valorAtualiza); 
    } else {
        // Para campo numérico. Ler como string e depois usar atoi
        scanf("%s", valorAtualiza); 
    }
}


void aplicarAtualizacao (RegistroPessoa *pessoa, char *campoAtualiza, char *valorAtualiza) {
    // atualizar o valor no campo de atualizacao
    
    // Se o campo é idPessoa
    if (strcmp(campoAtualiza, "idPessoa") == 0) {
        pessoa->idPessoa = atoi(valorAtualiza);
    
    // Se o campo é idadePessoa
    } else if (strcmp(campoAtualiza, "idadePessoa") == 0) {
        if (strcmp(valorAtualiza, "NULO") == 0) {
            pessoa->idadePessoa = -1;
        } else {
            pessoa->idadePessoa = atoi(valorAtualiza);
        }
    
    // Se o campo é nomePessoa
    } else if (strcmp(campoAtualiza, "nomePessoa") == 0) {
        if (strcmp(valorAtualiza, "NULO") == 0) {
            pessoa->tamanhoNomePessoa = 0;
            pessoa->nomePessoa[0] = '\0';
        } else {
            strcpy(pessoa->nomePessoa, valorAtualiza);
            pessoa->tamanhoNomePessoa = strlen(valorAtualiza);
        }
    
    // Se o campo é nomeUsuario
    } else if (strcmp(campoAtualiza, "nomeUsuario") == 0) {
        if (strcmp(valorAtualiza, "NULO") == 0) {
            pessoa->tamanhoNomeUsuario = 0;
            pessoa->nomeUsuario[0] = '\0';
        } else {
            strcpy(pessoa->nomeUsuario, valorAtualiza);
            pessoa->tamanhoNomeUsuario = strlen(valorAtualiza);
        }
    }
}





// -------------------------------------------------------


void preencheStringCifrao (char *string) {
    for (int i = 0; i < 10; i++) {
		string[i] = '$';
	}
}


// Função que busca (binária) o ID procurado no arquivo de índice
// retorna -1 se não encontrado 
// retorna o byteOffset do registro no arquivo Pessoa
long long buscaByteOffsetPorID (FILE *fpIndice, int idBuscado) {
    int encontrou = 0;

    // Pegar o tamanho do arquivo
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoBytesArquivo = ftell(fpIndice);
    fseek(fpIndice, 0,SEEK_SET);

	// Calcular número de registros
	int numRegistros = (tamanhoBytesArquivo - TAMANHO_CABECALHO_INDICE) / TAMANHO_REGISTRO_INDICE;

	// Início, Meio e Fim
	int inicio = 0;
	int fim = numRegistros - 1;
	int meio;

    // Ler cabeçalho do arquivo de índice
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice);

    // Instanciar o registro do arquivo de índice
    RegistroIndice registroIndice;
    
	// Loop da busca binária
    while (inicio <= fim) {
        // Calcula o índice do registro do meio
        meio = inicio + (fim - inicio) / 2;

        // Calcular o byte offset exato onde o registro do meio começa
        long byteOffsetDoRegistroMeio = TAMANHO_CABECALHO_INDICE + (long)meio * TAMANHO_REGISTRO_INDICE;

        // Posicionar o ponteiro e ler o registro
        fseek(fpIndice, byteOffsetDoRegistroMeio, SEEK_SET);
        lerRegistroIndice(fpIndice, &registroIndice);

        // Comparar
        if (registroIndice.id == idBuscado) {
            encontrou = 1;
            registroIndice.byteOffSet = registroIndice.byteOffSet;
            break; // Encontrou. Sai do loop com o byteOffset
        }
        else if (registroIndice.id < idBuscado) {
            // O ID que lemos é menor, então o que buscamos está na metade direita
            inicio = meio + 1;
        }
        else { // registroIndice.id > idBuscado
            // O ID que lemos é maior, então o que buscamos está na metade esquerda
            fim = meio - 1;
        }
    }

    // Se, depois de ler o arquivo de índice inteiro, não achamos o ID...
    if (encontrou == 0) {
        return -1; // ID não encontrado
    }

    // Se achamos, byteOffset agora contém a localização exata do registro
    // no arquivo de dados fpPessoa
    return registroIndice.byteOffSet;
}

int correspondeRegistro (char nomeCampo[MAX_STRING_TAMANHO], char valorCampo[MAX_STRING_TAMANHO], RegistroPessoa pessoa) {
        // Agora, verificamos se este é o registro que procuramos
        int correspondeu = 0;
        
        if (strcmp(nomeCampo, "idadePessoa") == 0) {
            if (strcmp(valorCampo, "NULO") == 0) {
                if (pessoa.idadePessoa == -1) {
                    correspondeu = 1;
                }
            } else {
                if (pessoa.idadePessoa == atoi(valorCampo)) {
                    correspondeu = 1;
                }
            }
        } 
        else if (strcmp(nomeCampo, "nomePessoa") == 0) {
            if (strcmp(valorCampo, "NULO") == 0) {
                if (pessoa.tamanhoNomePessoa == 0) {
                    correspondeu = 1;
                }
            } else {
                if (strcmp(pessoa.nomePessoa, valorCampo) == 0) {
                    correspondeu = 1;
                }
            }
        } 
        else if (strcmp(nomeCampo, "nomeUsuario") == 0) {
            if (strcmp(valorCampo, "NULO") == 0) {
                if (pessoa.tamanhoNomeUsuario == 0) {
                    correspondeu = 1;
                } 
            } else {
                if (strcmp(pessoa.nomeUsuario, valorCampo) == 0) {
                    correspondeu = 1;
                } 
            }
        }

	// Se correspondeu, retorna 1
	if (correspondeu == 1) {
		return 1;
	}
	// Se não correspondeu, retorna 0
	else {
		return 0;
	}

}

// retorna a quantidade de bytes uteis (bytes de dado) de um registro de pessoa
int tamanhoUtilRegistro (RegistroPessoa pessoa) {
    // 4 * 4: 4 ints que possuem 4 bytes cada
    return 4 * 4 + pessoa.tamanhoNomePessoa + pessoa.tamanhoNomeUsuario;
}

// Para o QuickSort
int compararIndices(const void *a, const void *b) {
    RegistroIndice *regA = (RegistroIndice *)a;
    RegistroIndice *regB = (RegistroIndice *)b;
    return (regA->id - regB->id);
}

// Atualiza o status de removido, fazendo a remoção lógica 
void removerRegistroPessoa(FILE* fpPessoa) {
  fwrite("1", sizeof(char), 1, fpPessoa);
}

// SuperFunção de buscar pessoas
int buscarPessoas(FILE *fpPessoa, FILE *fpIndice, CabecalhoPessoa *headerPessoa, char *nomeCampo, char *valorCampo, long long *vetorOffsets) {
    
    int numEncontrados = 0;

    // --- Busca por índice  ---
    if (strcmp(nomeCampo, "idPessoa") == 0) {
        
        long long offset = buscaByteOffsetPorID(fpIndice, atoi(valorCampo));

        if (offset != -1) {
            // Encontrou no índice, agora verificar se não está removido no arq. de dados
            fseek(fpPessoa, offset, SEEK_SET);
            char removido;
            fread(&removido, sizeof(char), 1, fpPessoa);

            if (removido == '0') {
                vetorOffsets[0] = offset; // Armazena no vetor
                numEncontrados = 1;
            }
        }
    }
    // --- Busca sequencial ---
    else {
        // Pula o cabeçalho do arquivo de dados
        fseek(fpPessoa, TAMANHO_CABECALHO_PESSOA, SEEK_SET);

        RegistroPessoa pessoa;
        long long posAtual;

        // Loop principal da busca sequencial
        while ((posAtual = ftell(fpPessoa)) < headerPessoa->proxByteOffSet) {
            
            int removido = lerRegistroPessoa(fpPessoa, &pessoa);

            // Se o registro é válido (não removido)
            if (removido == 0) {
                // Verifica se corresponde ao critério de busca (usando sua função que trata NULO)
                if (correspondeRegistro(nomeCampo, valorCampo, pessoa) == 1) {
                    // Adiciona o offset ao vetor e incrementa o contador
                    vetorOffsets[numEncontrados] = posAtual;
                    numEncontrados++;
                }
            }
        }
    }

    return numEncontrados; // Retorna o número de Registros encontrados
}


// Função auxiliar para escrever um registro novo, já verificado o registro logicamente removido e que há espaço suficiente
// tamanhoDisponivel não contabiliza os 5 primeiros bytes (1 byte para removido e 4 bytes para tamanhoRegistro)
// essa função já preenche o fim do registro, caso haja espaço sobrando
void escreverRegistroPessoa(FILE *fpPessoa, RegistroPessoa pessoa, int tamanhoDisponivel) {
    // removido == 0, registro não removido
    fwrite("0", sizeof(char), 1, fpPessoa);
    // Tamanho do registro
    fwrite(&tamanhoDisponivel, sizeof(int), 1, fpPessoa);
    // idPessoa
    fwrite(&pessoa.idPessoa, sizeof(int), 1, fpPessoa);
    // idadePessoa
    fwrite(&pessoa.idadePessoa, sizeof(int), 1, fpPessoa);
    // tamanhoNomePessoa
    fwrite(&pessoa.tamanhoNomePessoa, sizeof(int), 1, fpPessoa);
    // nomePessoa[MAX_STRING_TAMANHO]
    fwrite(pessoa.nomePessoa, sizeof(char), pessoa.tamanhoNomePessoa, fpPessoa);
    // tamanhoNomeUsuario
    fwrite(&pessoa.tamanhoNomeUsuario, sizeof(int), 1, fpPessoa);
    // nomeUsuario[MAX_STRING_TAMANHO]
    fwrite(pessoa.nomeUsuario, sizeof(char), pessoa.tamanhoNomeUsuario, fpPessoa);

    // Preencher o restante de bytes do registro com lixo ($)
    int tamanhoNecessario = tamanhoUtilRegistro(pessoa);
    int tamanhoLixo = tamanhoDisponivel - tamanhoNecessario;
    for (int i = 0; i < tamanhoLixo; i++) {
        fwrite("$", sizeof(char), 1, fpPessoa);
    }

}


// Função que busca o primeiro registro removido que caiba o novo registro e insere o novoRegistro
// Caso não encontre um registro removido que caiba, o novoRegistro é inserido no final do arquivo
// A verificação se cabe é feita usando apenas os campos posteriores ao "removido" e "tamanhoRegistro"
// Retorna o byteOffset do registro para atualizar o arquivo de índice
long long inserirPessoa (FILE* fpPessoa, RegistroPessoa novoRegistro, CabecalhoPessoa *headerPessoa) {
    int tamanhoNecessario = tamanhoUtilRegistro(novoRegistro);
    long long posAtual = ftell(fpPessoa);

    // inserir no final
    long long byteOffset = headerPessoa->proxByteOffSet;
    fseek(fpPessoa, byteOffset, SEEK_SET);

    // Atualizar o byteOffset e mover o cursor para ele
    byteOffset = headerPessoa->proxByteOffSet;
    fseek(fpPessoa, byteOffset, SEEK_SET);

    // No fim, o tamanho disponível é exatamente o necessário
    escreverRegistroPessoa(fpPessoa, novoRegistro, tamanhoNecessario); 

    // Atualizar o cabeçalho na memória
    headerPessoa->qtdPessoas++;
    headerPessoa->proxByteOffSet += (sizeof(char) + sizeof(int) + tamanhoNecessario);

    return byteOffset;
}


// Função que vai inserir os novos registros e ordenar o arquivo de índice
void atualizarEOrdenarArquivoIndice (FILE *fpIndice, char *nameFileIndice, RegistroIndice *novosRegistros, int numRegistrosNovos) {
	// Descobrir o número de registros antigos
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoBytesArquivo = ftell(fpIndice);
    int numRegistrosAntigos = (tamanhoBytesArquivo - TAMANHO_CABECALHO_INDICE) / TAMANHO_REGISTRO_INDICE;

	// Número total de registros após inserção
    int numeroRegistros = numRegistrosAntigos + numRegistrosNovos;

    // Alocar um vetor de ponteiros da struct de registro de índice
    // para fazer o sorting na memória e depois a escrita
    RegistroIndice *registrosIndiceMemoria = (RegistroIndice*)malloc(numeroRegistros * sizeof(RegistroIndice));

    // Ler o arquivo de índice na memória (registros antigos)
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET); // pular cabecalho
    for (int i = 0; i < numRegistrosAntigos; i++) { // Ler todos os registros antigos
        lerRegistroIndice(fpIndice, &registrosIndiceMemoria[i]);
    }

    // Adicionar os novos registros no vetor de índices
    for (int i = 0; i < numRegistrosNovos; i++) {
        registrosIndiceMemoria[i + numRegistrosAntigos] = novosRegistros[i];
    }

    // Ordenar o vetor usando QuickSort
    qsort(registrosIndiceMemoria, numeroRegistros, sizeof(RegistroIndice), compararIndices);

	// Reabrir o arquivo em "wb" e reescrever tudo
    fclose(fpIndice);
    fpIndice = fopen(nameFileIndice, "wb");
    if (verificaArquivo(fpIndice) == 0) {
        return;
    } 

    // Escreve o cabeçalho do arquivo de índice já consistente para fechar
    CabecalhoIndice headerIndice = {'1', "$$$$$$$$$$$"};
    fwrite(&headerIndice.status, sizeof(char), 1, fpIndice);
    fwrite(headerIndice.lixo, sizeof(char), 11, fpIndice);

    // Escrever o vetor ordenado no arquivo (já aberto para escrita e leitura)
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET); // Pular cabeçalho
    for (int i = 0; i < numeroRegistros; i++) {
        escreveRegistroIndice(fpIndice, registrosIndiceMemoria[i]);
    }

    // Liberar memória
    free(registrosIndiceMemoria);

	// fechar o arquivo e reabrir em "r+b" para o resto do programa
    fclose(fpIndice);
    fpIndice = fopen(nameFileIndice, "r+b");
    if (verificaArquivo(fpIndice) == 0) {
		return; // abortar
	}

}

// Função auxiliar para verificar (binariamente) se um ID existe em um vetor
int idEstaNaLista(int idBuscado, int *listaIds, int tamanhoLista) {
	int inicio = 0, meio, fim = tamanhoLista - 1;
	int encontrou = 0;

    while (inicio <= fim) {
        // Calcula o índice do registro do meio
        meio = inicio + (fim - inicio) / 2;

        // Comparar se é o id buscado
        if (listaIds[meio] == idBuscado) {
            encontrou = 1;
            break; // Encontrou. Sai do loop
        }
        else if (listaIds[meio] < idBuscado) {
            // O ID que lemos é menor, então o que buscamos está na metade direita
            inicio = meio + 1;
        }
        else { // listaIds[meio] > idBuscado
            // O ID que lemos é maior, então o que buscamos está na metade esquerda
            fim = meio - 1;
        }
    }

	if (encontrou == 0) {
		return 0;
	}
	else {
		return 1;
	}

}

int compararIndicesPeloId(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Função auxiliar para a func5, para atualizar o arquivo de índice após remoção
void reescreverIndiceComRemocoes(FILE *fpIndice, char *nomeArquivoIndice, CabecalhoIndice headerIndice, int *idsParaRemover, int numRemovidos) {
    // Se não houver remoções, não fazer nada
    if (numRemovidos == 0) {
		return; // Abortar
	}

    // Descobrir o número de registros antigos
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoBytesArquivo = ftell(fpIndice);
    int numRegistrosAntigos = (tamanhoBytesArquivo - TAMANHO_CABECALHO_INDICE) / TAMANHO_REGISTRO_INDICE;

    // Alocar vetor para ler os registros antigos
    RegistroIndice *vetorAntigo = (RegistroIndice*)malloc(numRegistrosAntigos * sizeof(RegistroIndice));
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET);
    for (int i = 0; i < numRegistrosAntigos; i++) {
        lerRegistroIndice(fpIndice, &vetorAntigo[i]);
    }

    // Alocar vetor para os novos registros
    RegistroIndice *vetorNovo = (RegistroIndice*)malloc(numRegistrosAntigos * sizeof(RegistroIndice));
    int contagemVetorNovo = 0;

    // ordenar para busca binária
    qsort(idsParaRemover, numRemovidos, sizeof(int), compararIndicesPeloId);

    // Fazer um filtro, ou seja, copiar do antigo para o novo se não tiver na lista de remoção
    for (int i = 0; i < numRegistrosAntigos; i++) {
        // Usando busca binária para isso
        if (idEstaNaLista(vetorAntigo[i].id, idsParaRemover, numRemovidos) == 0) {
            // ID não está na lista de remoção, então fazer a cópia
            vetorNovo[contagemVetorNovo] = vetorAntigo[i];
            contagemVetorNovo++;
        }
    }

    // Reabrir o arquivo no modo "wb" e reescrever tudo
    fclose(fpIndice); // Fecha o ponteiro antigo
    fpIndice = fopen(nomeArquivoIndice, "wb"); // Cria um novo ponteiro
    if (verificaArquivo(fpIndice) == 0) {
        // Libera memória antes de sair em caso de erro
        free(vetorAntigo);
        free(vetorNovo);
        return; // Retorna para indicar falha
    }

    // Atualizar consistência
    headerIndice.status = '1';

	// Cabeçalho para o novo arquivo de índice, já consistente
    escreverCabecalhoIndice(fpIndice, headerIndice);

    // Escreve os registros (já estão ordenados)
    for (int i = 0; i < contagemVetorNovo; i++) {
        escreveRegistroIndice(fpIndice, vetorNovo[i]);
    }

	// liberar memória
    free(vetorAntigo);
    free(vetorNovo);
    
// Fechar e reabrir o arquivo de índica no modo "r+b"
    fclose(fpIndice);
    fpIndice = fopen(nomeArquivoIndice, "r+b"); // Cria o ponteiro final
    if (verificaArquivo(fpIndice) == 0) {
        return;// Retorna para indicar falha
    } 

    return;
}


// Função auxiliar para a func7, para atualizar o arquivo de índica após as modificações
void reescreverIndiceComAtualizacoes(FILE *fpIndice, char *nomeArquivoIndice, 
                                     int *idsParaRemover, int numRemovidos, 
                                     RegistroIndice* registrosParaAdicionar, int numAdicionados) {
	// se não houver modificações, não fazer nada
    if (numRemovidos == 0 && numAdicionados == 0) {
		return; // abortar
	}

    // Ler o número de registos antes das modificações
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoBytesArquivo = ftell(fpIndice);
    int numRegistrosAntigos = (tamanhoBytesArquivo - TAMANHO_CABECALHO_INDICE) / TAMANHO_REGISTRO_INDICE;

	// Alocar um vetor de RegistroIndice para os registros de índice antes da modificação
    RegistroIndice *vetorAntigo = (RegistroIndice*)malloc(numRegistrosAntigos * sizeof(RegistroIndice));
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET);
    for (int i = 0; i < numRegistrosAntigos; i++) {
        lerRegistroIndice(fpIndice, &vetorAntigo[i]);
    }

    // Alocar um vetor de RegistroIndice para os registros após as modificações
    int numRegistrosNovo = numRegistrosAntigos - numRemovidos + numAdicionados;
    RegistroIndice *vetorNovo = (RegistroIndice*)malloc(numRegistrosNovo * sizeof(RegistroIndice));
    int contagemVetorNovo = 0;

    // ordenar para busca binária
    qsort(idsParaRemover, numRemovidos, sizeof(int), compararIndicesPeloId);

    // Fazer um filtro, copiando do antigo para o novo se não tiver removido
    for (int i = 0; i < numRegistrosAntigos; i++) {
        if (idEstaNaLista(vetorAntigo[i].id, idsParaRemover, numRemovidos) == 0) {
            vetorNovo[contagemVetorNovo] = vetorAntigo[i];
            contagemVetorNovo++;
        }
    }

    // Adicionar os novos registros no final do vetor filtrado
    for (int i = 0; i < numAdicionados; i++) {
        vetorNovo[contagemVetorNovo] = registrosParaAdicionar[i];
        contagemVetorNovo++;
    }

    // Ordenar o vetor combinado (filtrado + adicionados), usando QuickSort
    qsort(vetorNovo, contagemVetorNovo, sizeof(RegistroIndice), compararIndices);

    // Reabrir o arquivo no modo "wb" e reescrever tudo
    fclose(fpIndice);
    fpIndice = fopen(nomeArquivoIndice, "wb");
    if (verificaArquivo(fpIndice) == 0) { // verificar arquivo
        return;
    } 

	// Cabeçaho para o novo arquivo de índice
    CabecalhoIndice headerIndice = {'0', "$$$$$$$$$$$"};
    fwrite(&headerIndice.status, sizeof(char), 1, fpIndice);
    fwrite(headerIndice.lixo, sizeof(char), 11, fpIndice);
    
    // Escreve os registros ordenados
    for (int i = 0; i < contagemVetorNovo; i++) {
        escreveRegistroIndice(fpIndice, vetorNovo[i]);
    }

    free(vetorAntigo);
    free(vetorNovo);
    
    // Fechar e reabrir em "r+b"
    fclose(fpIndice);
    fpIndice = fopen(nomeArquivoIndice, "r+b");
    if (verificaArquivo(fpIndice) == 0) {
        return;
    } 
}
