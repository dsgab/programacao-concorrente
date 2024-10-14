#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define LIMITE 10
#define NUMERO_THREADS 3
//#define DEBUG /* Descomentar o #define para printar o log de execução do programa. */

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif

/* Criado para diferenciar qual dos dois buffers será utilizado. */
typedef enum _tipoBuffer
{
    BUFFER1,
    BUFFER2
}
TipoBuffer;

void *thread1(void *arg);
void *thread2(void *arg);
void *thread3(void *arg);
void insereBuffer(char caractere, TipoBuffer tipo);
char retiraBuffer(TipoBuffer tipo);

char *buffer1, *buffer2;
unsigned int n = 0, tamanho, inBuffer1 = 0, outBuffer1 = 0, inBuffer2 = 0, outBuffer2 = 0;
FILE* arquivoEntrada;
sem_t slotsCheios1, slotsVazios1, slotsCheios2, slotsVazios2;

int main(int argc, char *argv[])
{
    pthread_t *tid;
    char *nomeArquivo;

    /* Verificando se a quantidade de argumentos é a mínima necessária para a execução correta do programa. */
    if(argc < 3)
    {
        printf("Digite: %s <TAMANHO_BUFFER> <NOME_ARQUIVO_ENTRADA>\n", argv[0]);
        return 1;
    }
    tamanho = atoi(argv[1]);

    /* Alocar o vetor de threads e verificar se pode ser alocado corretamente. */
    tid = malloc(sizeof(pthread_t) * NUMERO_THREADS);
    if(!tid)
    {
        puts("Não foi possível alocar espaço.");
        return 2;
    }

    /* Verificando se o nome do arquivo não é nulo. */
    nomeArquivo = argv[2];
    if(nomeArquivo == NULL)
    {
        puts("Não foi possível ler o nome do arquivo.");
        return 3;
    }

    /* Abertura do arquivo para leitura. */
    arquivoEntrada = fopen(nomeArquivo, "r");
    if(arquivoEntrada == NULL)
    {
        puts("Não foi possível abrir o arquivo para leitura.");
        return 4;
    }

    /* Alocar e verificar se foram alocados ambos os buffers necessários. */
    buffer1 = malloc(sizeof(char) * tamanho);
    buffer2 = malloc(sizeof(char) * tamanho);
    if(buffer1 == NULL || buffer2 == NULL)
    {
        puts("Não foi possível abrir o arquivo para leitura.");
        return 5;
    }

    /* Inicialização dos semáforos. */
    sem_init(&slotsCheios1, 0, 0);
    sem_init(&slotsCheios2, 0, 0);
    sem_init(&slotsVazios1, 0, tamanho);
    sem_init(&slotsVazios2, 0, tamanho);

    /* Criação de cada uma das 3 threads individualmente. */
    if(pthread_create(&tid[0], NULL, thread1, NULL))
    {
        puts("Erro no pthread_create.");
        return 6;
    }
    if(pthread_create(&tid[1], NULL, thread2, NULL))
    {
        puts("Erro no pthread_create.");
        return 7;
    }
    if(pthread_create(&tid[2], NULL, thread3, NULL))
    {
        puts("Erro no pthread_create.");
        return 8;
    }

    /* Join nas threads e verificar se houve erro na função join. */
    for(int i = 0; i < NUMERO_THREADS; i++)
    {
        if(pthread_join(tid[i], NULL))
        {
            puts("Erro no pthread_join.");
            return 9;
        }
    }

    /* Finalizar o programa liberando a memória do que foi utilizado. */
    sem_destroy(&slotsCheios1);
    sem_destroy(&slotsCheios2);
    sem_destroy(&slotsVazios1);
    sem_destroy(&slotsVazios2);
    free(tid);
    free(buffer1);
    free(buffer2);
    return 0;
}

void *thread1(void *arg)
{
    char caractere;
    DEBUG_PRINT(("Thread 1 inicializada.\n"));
    while((caractere = fgetc(arquivoEntrada)) != EOF)
        insereBuffer(caractere, BUFFER1);
    insereBuffer('\0', BUFFER1); // Adiciona '\0' no final do buffer 1 para sinalizar onde acaba o texto original.
    fclose(arquivoEntrada);
    DEBUG_PRINT(("\n\n\nThread 1 finaliada.\n\n\n"));
    return NULL;
}

void *thread2(void *arg)
{
    unsigned int caracteresLidos = 0;
    char retirado;
    DEBUG_PRINT(("Thread 2 inicializada.\n"));
    while((retirado = retiraBuffer(BUFFER1)) != '\0') // Verifica se o texto original acabou caso o caractere lido seja '\0'
    {
        insereBuffer(retirado, BUFFER2);
        caracteresLidos++;
        if(n < 11)
        {
            if(caracteresLidos >= (2 * n) + 1)
            {
                insereBuffer('\n', BUFFER2);
                caracteresLidos = 0;
                n++;
            }
        }
        else
        {
            if(caracteresLidos >= LIMITE)
            {
                insereBuffer('\n', BUFFER2); // Adiciona '\0' no final do buffer 2 para sinalizar onde acaba o texto original.
                caracteresLidos = 0;
            }
        }
    }
    insereBuffer('\0', BUFFER2);
    DEBUG_PRINT(("\n\n\nThread 2 finaliada.\n\n\n"));
    return NULL;
}

void *thread3(void *arg)
{
    char retirado;
    DEBUG_PRINT(("Thread 3 inicializada.\n"));
    while((retirado = retiraBuffer(BUFFER2)) != '\0') // Verifica se o texto original acabou caso o caractere lido seja '\0'
        printf("%c", retirado);
    DEBUG_PRINT(("Thread 3 finalizada.\n"));
    return NULL;
}

/* Função chamada pelo produtor. */
void insereBuffer(char caractere, TipoBuffer tipo)
{
    switch(tipo)
    {
        case BUFFER1:
            DEBUG_PRINT(("Thread 1 quer inserir no buffer 1.\n"));
            sem_wait(&slotsVazios1);
            DEBUG_PRINT(("Thread 1 vai inserir no buffer 1, posição: %d.\n", inBuffer1));
            buffer1[inBuffer1] = caractere;
            inBuffer1 = (inBuffer1 + 1) % tamanho;
            sem_post(&slotsCheios1);
            DEBUG_PRINT(("Caractere %c inserido no buffer 1.\n", caractere));
            break;
        case BUFFER2:
            DEBUG_PRINT(("Thread 2 quer inserir no buffer 2.\n"));
            sem_wait(&slotsVazios2);
            DEBUG_PRINT(("Thread 2 vai inserir no buffer 2, posição: %d.\n", inBuffer2));
            buffer2[inBuffer2] = caractere;
            inBuffer2 = (inBuffer2 + 1) % tamanho;
            sem_post(&slotsCheios2);
            DEBUG_PRINT(("Caractere %c inserido no buffer 2.\n", caractere));
            break;
        default:
            puts("Tipo não reconhecido passado como argumento da função 'insereBuffer'.");
            exit(10);
    }
}

/* Função chamada pelo consumidor. 
    @return Caractere consumido pelo consumidor.
*/
char retiraBuffer(TipoBuffer tipo)
{
    char retorno;
    switch(tipo)
    {
        case BUFFER1:
            DEBUG_PRINT(("Thread 2 quer retirar do buffer 1.\n"));
            sem_wait(&slotsCheios1);
            DEBUG_PRINT(("Thread 2 vai retirar do buffer 1, posição: %d.\n", outBuffer1));
            retorno = buffer1[outBuffer1];
            outBuffer1 = (outBuffer1 + 1) % tamanho;
            sem_post(&slotsVazios1);
            DEBUG_PRINT(("Caractere %c retirado do buffer 1.\n", retorno));
            break;
        case BUFFER2:
            DEBUG_PRINT(("Thread 3 quer retirar do buffer 2.\n"));
            sem_wait(&slotsCheios2);
            DEBUG_PRINT(("Thread 3 vai retirar do buffer 2, posição: %d.\n", outBuffer2));
            retorno = buffer2[outBuffer2];
            outBuffer2 = (outBuffer2 + 1) % tamanho;
            sem_post(&slotsVazios2);
            DEBUG_PRINT(("Caractere %c retirado do buffer 2.\n", retorno));
            break;
        default:
            puts("Tipo não reconhecido passado como argumento da função 'retiraBuffer'.");
            exit(11);
    }
    return retorno;
}