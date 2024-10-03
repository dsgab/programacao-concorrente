#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list_int.h"
#include "timer.h"

#define QTDE_OPS 100
#define QTDE_INI 100
#define MAX_VALUE 100


struct list_node_s* head = NULL;
int nThreads, leitores = 0, escritores = 0, querEscrever = 0;
pthread_mutex_t mutex;
pthread_cond_t condicaoEscrita, condicaoLeitura;

void entraLeitura()
{
    pthread_mutex_lock(&mutex);
    while(escritores > 0 || querEscrever > 0)
    {
        pthread_cond_wait(&condicaoLeitura, &mutex);
    }
    leitores++;
    pthread_mutex_unlock(&mutex);
}

void saiLeitura()
{
    pthread_mutex_lock(&mutex);
    leitores--;
    if(leitores == 0) pthread_cond_signal(&condicaoEscrita);
    pthread_mutex_unlock(&mutex);
}

void entraEscrita()
{
    pthread_mutex_lock(&mutex);
    if(escritores > 0 || leitores > 0) querEscrever++;
    while(escritores > 0 || leitores > 0)
    {
        pthread_cond_wait(&condicaoEscrita, &mutex);
    }
    escritores++;
    pthread_mutex_unlock(&mutex);
}

void saiEscrita()
{
    pthread_mutex_lock(&mutex);
    escritores--;
    querEscrever--;
    pthread_cond_broadcast(&condicaoEscrita);
    pthread_cond_broadcast(&condicaoLeitura);
    pthread_mutex_unlock(&mutex);
}

void* tarefa(void* arg)
{
    long int id = (long int) arg;
    int op;
    int in, out, read; 
    in=out=read = 0; 

    //realiza operacoes de consulta (98%), insercao (1%) e remocao (1%)
    for(long int i=id; i<QTDE_OPS; i+=nThreads) {
        op = rand() % 100;
        if(op<98)
        {
            printf("Quer ler %ld na thread %ld.\n", i, id);
	        entraLeitura();
            printf("Lendo %ld na thread %ld.\n", i, id);
            Member(i%MAX_VALUE, head);   /* Ignore return value */
	        saiLeitura();
	        read++;
        }
        if(op<99)
        {
            printf("Quer escrever %ld na thread %ld.\n", i, id);
            entraEscrita();
            printf("Escrevendo %ld na thread %ld.\n", i, id);
            Insert(i%MAX_VALUE, &head);  /* Ignore return value */
            saiEscrita();
	        in++;
        }
        if(99<=op)
        {
            printf("Quer escrever %ld na thread %ld.\n", i, id);
            entraEscrita();
            printf("Escrevendo %ld na thread %ld.\n", i, id);
            Delete(i%MAX_VALUE, &head);  /* Ignore return value */
            saiEscrita();
	        out++;
        }
    }
    //registra a qtde de operacoes realizadas por tipo
    printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    pthread_t* threads;
    double inicio, fim, delta;

    if(argc < 2)
    {
        printf("Execute: %s <NÚMERO_THREADS>\n", argv[0]);
        return 1;
    }
    nThreads = atoi(argv[1]);

    for(int i = 0; i < QTDE_INI; i++) Insert(rand() % MAX_VALUE, &head);

    threads = malloc(sizeof(pthread_t)*nThreads);
    if(threads == NULL)
    {
        puts("Erro no malloc.");
        return 2;
    }

    pthread_mutex_init(&mutex, NULL);

    GET_TIME(inicio);
    for(long int i=0; i < nThreads; i++) {
        if(pthread_create(threads+i, NULL, tarefa, (void*) i))
        {
            puts("Erro no pthread_create().");
            return 3;
        }
    }
    for(int i = 0; i < nThreads; i++)
    {
        if(pthread_join(threads[i], NULL))
        {
            puts("Erro no pthread_join().");
            return 4;
        }
    }
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo: %lf\n", delta);

    pthread_mutex_destroy(&mutex);
    free(threads);
    Free_list(&head);
    return 0;
}
