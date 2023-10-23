/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */

#include "apsignal.h"
#include "main.h"
#include "synchronization.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

extern struct main_data *data;
extern struct comm_buffers *buffers;
extern struct semaphores *sems;

/**
 * Função que vai dar set a um alarm de alarm_time em alarm_time.
 * É usado a função setitimer porque a função alarm() está obsuleta.
 */
void setAlarm()
{
    signal(SIGALRM, handleTimerSignal);
    struct itimerval val;
    val.it_interval.tv_sec = data->alarm_time; // Tempo do timer atual
    val.it_interval.tv_usec = 0;
    val.it_value.tv_sec = data->alarm_time; // Proximo tempo quando o timer acabar
    val.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &val, 0);
}

/**
 * Função que vai fazer mudar o comportamento do programa quando
 * na presença de um sinal SIGINT (CtrlC). Vai fazer com que o programa
 * termine corretamente.
 */
void setStopSignal(int main_id)
{
    if (getpid() == main_id) // A main vai terminar o processo de forma correta
    {
        signal(SIGINT, callStopExec);
    }
    else
    {
        // Os processos filhos têm de acabar antes da main para haver a finalização dos processos filhos
        signal(SIGINT, defaultExec);
    }
}

/**
 * Função que faz com que nada aconteça na presença de um SIGINT(CtrlC)
 */
void defaultExec()
{
    signal(SIGINT, defaultExec);
}

/**
 * Função que para a execução
 */
void callStopExec()
{
    stop_execution(data, buffers, sems);
    destroyBuffers();
    exit(0);
}

/**
 * Printa o estado atual das operções quando recebe o SIGALRM
 */
void handleTimerSignal()
{
    for (int i = 0; i < MAX_RESULTS; i++)
    {
        struct operation opP = data->results[i];
        if (opP.id != -1)
        {
            printOp(opP);
        }
    }
    printf("\n");
    signal(SIGALRM, handleTimerSignal);
}

/**
 * Printa uma operação
 */
void printOp(struct operation op)
{

    if (op.status == 'M')
    {
        printf("op:%d | status:%c | start_time:%ld \n",
               op.id,
               op.status,
               op.start_time.tv_sec);
    }
    else if (op.status == 'C')
    {
        printf("op:%d | status:%c | start_time:%ld | client:%d | client_time:%ld\n",
               op.id,
               op.status,
               op.start_time.tv_sec,
               op.requesting_client,
               op.client_time.tv_sec);
    }
    else if (op.status == 'I')
    {
        printf("op:%d | status:%c | start_time:%ld | client:%d | client_time:%ld | intermediary:%d | intermediary_time:%ld\n",
               op.id,
               op.status,
               op.start_time.tv_sec,
               op.requesting_client,
               op.client_time.tv_sec,
               op.receiving_interm,
               op.intermed_time.tv_sec);
    }
    else if (op.status == 'A')
    {
        printf("op:%d | status:%c | start_time:%ld | client:%d | client_time:%ld | intermediary:%d | intermediary_time:%ld | enterprise:%d | enterprise_time:%ld\n",
               op.id,
               op.status,
               op.start_time.tv_sec,
               op.receiving_client,
               op.client_time.tv_sec,
               op.receiving_interm,
               op.intermed_time.tv_sec,
               op.receiving_enterp,
               op.enterp_time.tv_sec);
    }
    else if (op.status == 'E')
    {
        printf("op:%d | status:%c | start_time:%ld | client:%d | client_time:%ld | intermediary:%d | intermediary_time:%ld | enterprise:%d | enterprise_time:%ld\n",
               op.id,
               op.status,
               op.start_time.tv_sec,
               op.receiving_client,
               op.client_time.tv_sec,
               op.receiving_interm,
               op.intermed_time.tv_sec,
               op.receiving_enterp,
               op.enterp_time.tv_sec);
    }
}

/**
 * Liberta a memória alocada pela main
 */
void destroyBuffers()
{
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_client);
    destroy_dynamic_memory(buffers->client_interm);
    destroy_dynamic_memory(buffers->interm_enterp);
    destroy_dynamic_memory(buffers);
    destroy_dynamic_memory(sems->main_client);
    destroy_dynamic_memory(sems->client_interm);
    destroy_dynamic_memory(sems->interm_enterp);
    destroy_dynamic_memory(sems);
}