/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */

#include "synchronization.h"
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

/* Função que cria um novo semáforo com nome name e valor inicial igual a
 * value. Pode concatenar o resultado da função getuid() a name, para tornar
 * o nome único para o processo.
 */
sem_t *semaphore_create(char *name, int value)
{
    sem_t *sem;

    int id = getuid();
    char idname[100];
    idname[0] = 0;
    sprintf(idname, "%d", id);
    strcat(idname, name);

    sem = sem_open(idname, O_CREAT, 0xFFFFFFFF, value);
    if (sem == SEM_FAILED)
    {
        perror("full");
        exit(1);
    }
    return sem;
}

/*
 * Função que destroi o semáforo passado em argumento.
 */
void semaphore_destroy(char *name, sem_t *semaphore)
{
    int id = getuid();
    char idname[100];
    idname[0] = 0;

    sprintf(idname, "%d", id);
    strcat(idname, name);

    int guard1 = sem_close(semaphore);
    if (guard1 == -1)
    {
        perror("sem_close");
    }

    int guard2 = sem_unlink(idname);
    if (guard2 == -1)
    {
        perror("sem_unlink");
    }
}

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
 * corretos da estrutura passada em argumento.
 */
void produce_begin(struct prodcons *pc)
{
    int guard = sem_wait(pc->empty);
    if (guard == -1)
    {
        perror("sem_wait_prod_begin");
    }
    int guard2 = sem_wait(pc->mutex);
    if (guard2 == -1)
    {
        perror("sem_wait_prod_begin");
    }
}

/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
 * corretos da estrutura passada em argumento.
 */
void produce_end(struct prodcons *pc)
{
    int guard = sem_post(pc->mutex);
    if (guard == -1)
    {
        perror("sem_post_prod_end");
    }
    int guard2 = sem_post(pc->full);
    if (guard2 == -1)
    {
        perror("sem_post_prod_end");
    }
}

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
 * corretos da estrutura passada em argumento.
 */
void consume_begin(struct prodcons *pc)
{
    int guard = sem_wait(pc->full);
    if (guard == -1)
    {
        perror("sem_wait_cons_begin");
    }
    int guard2 = sem_wait(pc->mutex);
    if (guard2 == -1)
    {
        perror("sem_wait_cons_begin");
    }
}

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
 * corretos da estrutura passada em argumento.
 */
void consume_end(struct prodcons *pc)
{
    int guard = sem_post(pc->mutex);
    if (guard == -1)
    {
        perror("sem_post_cons_end");
    }
    int guard2 = sem_post(pc->empty);
    if (guard2 == -1)
    {
        perror("sem_post_cons_end");
    }
}

/* Função que faz wait a um semáforo.
 */
void semaphore_mutex_lock(sem_t *sem)
{
    int guard = sem_wait(sem);
    if (guard == -1)
    {
        perror("semaphore_create_lock");
    }
}

/* Função que faz post a um semáforo.
 */
void semaphore_mutex_unlock(sem_t *sem)
{
    int guard = sem_post(sem);
    if (guard == -1)
    {
        perror("semaphore_create_unlock");
    }
}