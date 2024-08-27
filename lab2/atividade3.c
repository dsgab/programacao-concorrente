#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char** argv)
{
    int tamanho, quantidadeThreads;
    float *vetor1, *vetor2;
    double produtoInternoSequencial = 0, produtoInternoConcorrente = 0;
    FILE *descritorArquivo;
    pthread_t *idThreads;
    if(argc < 3)
    {
        printf("Faltou argumentos: %s <QUANTIDADE_DE_THREADS> <NOME_DO_ARQUIVO>\n", argv[0]);
        return 1;
    }
    descritorArquivo = fopen(argv[2], "rb");
    if(fread(&tamanho, sizeof(int), 1, descritorArquivo) != 1)
    {
        printf("O arquivo %s não pode ser lido\n", argv[2]);
        return 2;
    }
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
    if(fread(&produtoInternoSequencial, sizeof(double), 1, descritorArquivo) != 1)
    {
        printf("Não foi possível ler o produto interno do arquivo %s.\n", argv[2]);
        return 6;
    }
    printf("Quantidade de threads: %d\n", quantidadeThreads);
    printf("N: %d\n", tamanho);
    for(int i = 0; i < tamanho - 1; i++)
    {
        printf("%.4f, ", vetor1[i]);
    }
    printf("%.4f\n", vetor1[tamanho-1]);
    for(int i = 0; i < tamanho - 1; i++)
    {
        printf("%.4f, ", vetor2[i]);
    }
    printf("%.4f\n", vetor2[tamanho-1]);
    printf("Produto interno: %lf\n", produtoInternoSequencial);

    quantidadeThreads = atoi(argv[1]);
    if(quantidadeThreads < 1)
    {
        puts("É necessário pelo menos uma thread.");
        return 1;
    }
    if(quantidadeThreads > tamanho) quantidadeThreads = tamanho;
    idThreads = malloc(sizeof(pthread_t) * quantidadeThreads);
    if(!idThreads)
    {
        puts("Erro ao alocar espaço para o vetor de IDs de threads.");
        return 1;
    }



    free(vetor1);
    free(vetor2);
    return 0;
}