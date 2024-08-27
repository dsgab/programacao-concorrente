#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10000

int main(int argc, char* argv[])
{
    int tamanho;
    float *vetor1, *vetor2;
    double produtoInterno = 0;
    FILE *descritorArquivo;
    if(argc < 3)
    {
        printf("Faltou argumentos: %s <TAMANHO> <NOME_DO_ARQUIVO>\n", argv[0]);
        return 1;
    }
    srand(time(NULL));
    tamanho = atoi(argv[1]);
    vetor1 = malloc(tamanho * sizeof(float));
    vetor2 = malloc(tamanho * sizeof(float));
    for(int i = 0; i < tamanho; i++)
    {
        vetor1[i] = ((float)rand() / (float)RAND_MAX) * 2 * MAX;
        vetor1[i] -= MAX;
        vetor2[i] = ((float)rand() / (float)RAND_MAX) * 2 * MAX;
        vetor2[i] -= MAX;
    }
    for(int i = 0; i < tamanho-1; i++) printf("%f, ", vetor1[i]);
    printf("%f\n", vetor1[tamanho-1]);
    for(int i = 0; i < tamanho-1; i++) printf("%f, ", vetor2[i]);
    printf("%f\n", vetor2[tamanho-1]);
    descritorArquivo = fopen(argv[2], "wb");
    if(!descritorArquivo)
    {
        puts("Erro ao abrir o arquivo.");
        return 2;
    }
    if(fwrite(&tamanho, sizeof(int), 1, descritorArquivo) < 1)
    {
        puts("Não foi possível escrever no arquivo o tamanho N.");
        return 3;
    }
    if(fwrite(vetor1, sizeof(float), tamanho, descritorArquivo) < tamanho)
    {
        puts("Não foi possível escrever no arquivo o vetor 1.");
        return 4;
    }
    if(fwrite(vetor2, sizeof(float), tamanho, descritorArquivo) < tamanho)
    {
        puts("Não foi possível escrever no arquivo o vetor 2.");
        return 4;
    }
    for(int i = 0; i < tamanho; i++)
    {
        produtoInterno += vetor1[i] * vetor2[i];
    }
    if(fwrite(&produtoInterno, sizeof(double), 1, descritorArquivo) < 1)
    {
        puts("Não foi possível escrever no arquivo o resultado do produto interno.");
        return 5;
    }
    printf("Produto interno: %lf\n", produtoInterno);
    fclose(descritorArquivo);
    return 0;
}
