#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int comeco, fim;
    float *vetor1, *vetor2;
}
argumentosThread;

void *produto(void *args);

int main(int argc, char** argv)
{
    int tamanho, numeroThreads, quantidadePorThread, resto, quantoFalta;
    float *vetor1, *vetor2;
    double produtoInternoSequencial = 0, produtoInternoConcorrente = 0, *retornoThreads, variacao;
    FILE *descritorArquivo;
    pthread_t *idThreads;
    argumentosThread *arg;

    /* Verifica se o programa foi chamado com o mínimo de argumentos necessários */
    if(argc < 3)
    {
        printf("Faltou argumentos: %s <QUANTIDADE_DE_THREADS> <NOME_DO_ARQUIVO>\n", argv[0]);
        return 1;
    }

    /* Abre o arquivo em binário para leitura e verifica se não ocorreu erros */
    descritorArquivo = fopen(argv[2], "rb");
    if(fread(&tamanho, sizeof(int), 1, descritorArquivo) != 1)
    {
        printf("O arquivo %s não pode ser lido\n", argv[2]);
        return 2;
    }

    /* Aloca espaço na memória para os dois vetores que terão seu produto interno calculado e passa os valores que estão no arquivo para eles */
    vetor1 = malloc(sizeof(float) * tamanho);
    vetor2 = malloc(sizeof(float) * tamanho);
    if(!(vetor1 || vetor2))
    {
        puts("Não foi possível alocar memória para os vetores.");
        return 3;
    }
    if(fread(vetor1, sizeof(float), tamanho, descritorArquivo) != tamanho)
    {
        printf("O vetor1 do arquivo %s não pode ser lido.\n", argv[2]);
        return 4;
    }
    if(fread(vetor2, sizeof(float), tamanho, descritorArquivo) != tamanho)
    {
        printf("O vetor2 do arquivo %s não pode ser lido.\n", argv[2]);
        return 5;
    }
    if(fread(&produtoInternoSequencial, sizeof(double), 1, descritorArquivo) != 1) // Armazena o valor do produto interno do programa sequencial
    {
        printf("Não foi possível ler o produto interno do arquivo %s.\n", argv[2]);
        return 6;
    }
    fclose(descritorArquivo); // Fecha o arquivo binário

    numeroThreads = atoi(argv[1]);
    if(numeroThreads < 1) 
    {
        puts("É necessário pelo menos uma thread.");
        return 1;
    }
    if(numeroThreads > tamanho) numeroThreads = tamanho; // Garante que não terá mais threads do que espaços no vetor
    idThreads = malloc(sizeof(pthread_t) * numeroThreads);
    if(!idThreads)
    {
        puts("Erro ao alocar espaço para o vetor de IDs de threads.");
        return 1;
    }
    quantidadePorThread = tamanho / numeroThreads; // Tamanho do bloco do vetor alocado a cada thread
    resto = tamanho % numeroThreads;
    quantoFalta = resto; // Número imutável de espaços extras a serem considerados

    for(int i = 0; i < numeroThreads; i++)
    {
        arg = malloc(sizeof(argumentosThread));
        if(!arg)
        {
            printf("Erro ao alocar espaço para os argumentos da thread %d.\n", i);
            return 1;
        }
        arg->vetor1 = vetor1;
        arg->vetor2 = vetor2;
        arg->comeco = quantidadePorThread * i + quantoFalta - resto; // Representa o índice do vetor em que esta thread deve começar
        if(resto > 0) resto--; // Resto é diminuído até chegar a 0
        arg->fim = quantidadePorThread * (i+1) + quantoFalta - resto - 1; // Representa o índice do vetor em que esta thread deve terminar
        if(pthread_create(&idThreads[i], NULL, produto, (void*) arg))
        {
            printf("Erro na criação da thread %d\n", i);
            return 3;
        }
    }

    /* Pega o retorno das threads e soma tudo para se obter o produto interno */
    for(int i = 0; i < numeroThreads; i++)
    {
        if(pthread_join(idThreads[i], (void*) &retornoThreads))
        {
            puts("Erro na função pthread_join().");
            return 1;
        }
        produtoInternoConcorrente += *retornoThreads;
        free(retornoThreads);
    }

    /* Variação calculada como pedido no item 4 da atividade 3 */
    variacao = (produtoInternoSequencial - produtoInternoSequencial) / produtoInternoSequencial;

    printf("Produto interno sequencial: %lf\n", produtoInternoSequencial);
    printf("Produto interno concorrente: %lf\n", produtoInternoConcorrente);
    printf("Variação relativa: %lf\n", variacao);
    free(idThreads);
    free(vetor1);
    free(vetor2);
    return 0;
}

void *produto(void *args)
{
    double parcial = 0, *retorno;
    argumentosThread *arg = (argumentosThread*) args;
    for(int i = arg->comeco; i <= arg->fim; i++)
    {
        parcial += arg->vetor1[i] * arg->vetor2[i];
    }
    retorno = malloc(sizeof(double));
    if(!retorno) puts("Erro na função malloc() em alguma das threads.");
    else *retorno = parcial;
    free(arg);
    pthread_exit((void*)retorno);
}
