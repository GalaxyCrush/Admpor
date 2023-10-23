/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */
#include "memory.h"
#include "process.h"
#include "main.h"
#include "client.h"
#include "enterprise.h"
#include "intermediary.h"
#include "synchronization.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Função que inicia um novo processo cliente através da função fork do SO. O novo
 * processo irá executar a função execute_client respetiva, fazendo exit do retorno.
 * O processo pai devolve o pid do processo criado.
 */
int launch_client(int client_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("error: process.launch_client");
        exit(-1);
    }
    else if (pid == 0)
    {
        int ret = execute_client(client_id, buffers, data, sems);
        exit(ret);
    }
    else
    {
        return pid;
    }
}

/* Função que inicia um novo processo intermediário através da função fork do SO. O novo
 * processo irá executar a função execute_intermediary, fazendo exit do retorno.
 * O processo pai devolve o pid do processo criado.
 */
int launch_interm(int interm_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("error: process.launch_intern");
        exit(-1);
    }
    else if (pid == 0)
    {
        int ret = execute_intermediary(interm_id, buffers, data, sems);
        exit(ret);
    }
    else
    {
        return pid;
    }
}

/* Função que inicia um novo processo empresa através da função fork do SO. O novo
 * processo irá executar a função execute_enterprise, fazendo exit do retorno.
 * O processo pai devolve o pid do processo criado.
 */
int launch_enterp(int enterp_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("error: process.launch_enterp");
        exit(-1);
    }
    else if (pid == 0)
    {
        int ret = execute_enterprise(enterp_id, buffers, data, sems);
        exit(ret);
    }
    else
    {
        return pid;
    }
}

/* Função que espera que um processo termine através da função waitpid.
 * Devolve o retorno do processo, se este tiver terminado normalmente.
 */
int wait_process(int process_id)
{
    int status = 0;
    waitpid(process_id, &status, 0);

    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }
    return -1;
}
