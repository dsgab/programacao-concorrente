#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10000

int main(int argc, char* argv[])
{
    int tamanho;
    float *vetor1, *vetor2;
    if(argc < 2)
    {
        printf("Faltou o tamanho do vetor: %s <TAMANHO>\n", argv[0]);
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
    return 0;
}
