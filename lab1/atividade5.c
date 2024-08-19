#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct
{
    int *vetor, comeco, fim;
}
argumentosThread;

void *incrementar(void *argumentos);
void inicializarVetor(int *vetor, int tamanho);
int verificarResultado(int *vetorOriginal, int *vetorAlterado, int tamanho);

int main(int argc, char* argv[])
{
    int numeroThreads, tamanhoVetor, quantidadePorThread, resto, quantoFalta;
    if(argc < 3)
    {
        printf("É necessário saber a quantidade de threads: %s numeroThreads tamanhoVetor\n", argv[0]);
        return 1;
    }

    srand(time(NULL));
    numeroThreads = atoi(argv[1]);
    tamanhoVetor = atoi(argv[2]);
    quantidadePorThread = tamanhoVetor / numeroThreads;
    resto = tamanhoVetor % numeroThreads;
    quantoFalta = resto;

    pthread_t idThreadsSistema[numeroThreads];
    int vetorOriginal[tamanhoVetor], vetorAlterado[tamanhoVetor];
    argumentosThread *args;

    inicializarVetor(vetorOriginal, tamanhoVetor);
    for(int i = 0; i < tamanhoVetor; i++) vetorAlterado[i] = vetorOriginal[i];

    for(int i = 0; i < numeroThreads; i++)
    {
        args = malloc(sizeof(argumentosThread));
        if(args == NULL)
        {
            puts("Erro na função malloc().");
            return 2;
        }
        args->vetor = vetorAlterado;
        args->comeco = quantidadePorThread * i + quantoFalta - resto;
        if(resto > 0) resto--;
        args->fim = quantidadePorThread * (i+1) + quantoFalta - resto - 1;
        if(pthread_create(&idThreadsSistema[i], NULL, incrementar, (void*) args))
        {
            printf("Erro na criação da thread %d\n", i);
            return 3;
        }
    }
    for(int i = 0; i < numeroThreads; i++)
    {
        if(pthread_join(idThreadsSistema[i], NULL))
            printf("Erro em pthread_join() da thread %d\n", i);
    }
    if(verificarResultado(vetorOriginal, vetorAlterado, tamanhoVetor))
    {
        puts("Não foi obtido o resultado esperado.");
        return 4;
    }
    return 0;
}

void *incrementar(void *argumentos)
{
    argumentosThread args = *(argumentosThread*) argumentos;
    for(int i = args.comeco; i <= args.fim; i++)
    {
        args.vetor[i]++;
    }
    free(argumentos);
    pthread_exit(NULL);
}

void inicializarVetor(int *vetor, int tamanho)
{
    for(int i = 0; i < tamanho; i++)
    {
        vetor[i] = rand() % 100;
    }
}

int verificarResultado(int *vetorOriginal, int *vetorAlterado, int tamanho)
{
    for(int i = 0; i < tamanho; i++)
    {
        if(vetorOriginal[i] + 1 != vetorAlterado[i]) return 1;
    }
    return 0;
}