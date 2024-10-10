#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

char *buffer1, *buffer2;
unsigned int n = 0, tamanho, limite = 1, indiceBuffer1 = 0;
FILE* arquivoEntrada, arquivoSaida;
sem_t buffer1Cheio, buffer1Vazio, buffer2Cheio, buffer2Vazio;

void *thread1(void *arg)
{
    while(!feof(arquivoEntrada))
    {
        sem_wait(&buffer1Vazio);
        fread(buffer1, sizeof(char), tamanho, arquivoEntrada);
        sem_post(&buffer1Cheio);
    }
    return NULL;
}

void *thread2(void *arg)
{
    sem_wait(&buffer1Cheio);
    for(int i = 0; i < tamanho; i++)
    {
        if(contador < limite)
        {
            buffer2[i] = buffer1[indiceBuffer1];
            indiceBuffer1++;
            n++;
        }
        else if(limite < 21)
        {
            limite += 2;
            contador = 0;
        }
        else
        {
            buffer2[i] = '\n';
            contador = 0;
        }
    }
}

void *thread3(void *arg)
{

}

int main(int argc, char *argv[])
{

}
