/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */
#include "memory.h"
#include "process.h"
#include "main.h"
#include "synchronization.h"
#include "configuration.h"
#include "log.h"
#include "stats.h"
#include "apsignal.h"
#include "aptime.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

//-----------------------Variaveis globais------------------------//
struct main_data *data;
struct comm_buffers *buffers;
struct semaphores *sems;
FILE *logF;
//---------------------------------------------------------------//

int main(int argc, char *argv[])
{
    pid_t main_id = getpid();

    data = create_dynamic_memory(sizeof(struct main_data));
    // init data structures
    buffers = create_dynamic_memory(sizeof(struct comm_buffers));
    buffers->main_client = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->client_interm = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->interm_enterp = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    // init semaphore data structure
    sems = create_dynamic_memory(sizeof(struct semaphores));
    sems->main_client = create_dynamic_memory(sizeof(struct prodcons));
    sems->client_interm = create_dynamic_memory(sizeof(struct prodcons));
    sems->interm_enterp = create_dynamic_memory(sizeof(struct prodcons));
    // execute main code
    main_args(argc, argv, data);
    create_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, buffers);
    create_semaphores(data, sems);
    launch_processes(buffers, data, sems);
    //-------------Set signals---------------//
    setAlarm();
    setStopSignal(main_id);
    //--------------------------------------//
    user_interaction(buffers, data, sems);
    // release memory before terminating
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

/* Função que lê os argumentos da aplicação, nomeadamente o número
 * máximo de operações, o tamanho dos buffers de memória partilhada
 * usados para comunicação, e o número de clientes, de intermediários e de
 * empresas. Guarda esta informação nos campos apropriados da
 * estrutura main_data.
 */
void main_args(int argc, char *argv[], struct main_data *data)
{
    if (argc == 2)
    {
        FILE *input = openFile(argv[1]);
        readInputFromFile(input, data);
        closeFile(input);
    }
    else
    {
        perror("Invalid args");
    }
}

/* Função que reserva a memória dinâmica necessária para a execução
 * do AdmPor, nomeadamente para os arrays *_pids e *_stats da estrutura
 * main_data. Para tal, pode ser usada a função create_dynamic_memory.
 */
void create_dynamic_memory_buffers(struct main_data *data)
{
    data->client_pids = create_dynamic_memory(data->n_clients * sizeof(int));
    data->client_stats = create_dynamic_memory(data->n_clients * sizeof(int));
    data->enterprise_pids = create_dynamic_memory(data->n_enterprises * sizeof(int));
    data->enterprise_stats = create_dynamic_memory(data->n_enterprises * sizeof(int));
    data->intermediary_pids = create_dynamic_memory(data->n_intermediaries * sizeof(int));
    data->intermediary_stats = create_dynamic_memory(data->n_intermediaries * sizeof(int));
}

/* Função que reserva a memória partilhada necessária para a execução do
 * AdmPor. É necessário reservar memória partilhada para todos os buffers da
 * estrutura comm_buffers, incluindo os buffers em si e respetivos
 * pointers, assim como para o array data->results e variável data->terminate.
 * Para tal, pode ser usada a função create_shared_memory. O array data->results
 * deve ser limitado pela constante MAX_RESULTS.
 */
void create_shared_memory_buffers(struct main_data *data, struct comm_buffers *buffers)
{
    buffers->client_interm->buffer = create_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, sizeof(struct operation) * data->buffers_size);
    buffers->interm_enterp->buffer = create_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, sizeof(struct operation) * data->buffers_size);
    buffers->main_client->buffer = create_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, sizeof(struct operation) * data->buffers_size);

    buffers->main_client->ptrs = create_shared_memory(STR_SHM_MAIN_CLIENT_PTR, sizeof(int) * data->buffers_size);

    buffers->client_interm->ptrs = create_shared_memory(STR_SHM_CLIENT_INTERM_PTR, sizeof(struct pointers));

    buffers->interm_enterp->ptrs = create_shared_memory(STR_SHM_INTERM_ENTERP_PTR, sizeof(int) * data->buffers_size);

    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));

    data->results = create_shared_memory(STR_SHM_RESULTS, sizeof(struct operation) * MAX_RESULTS);
    for (int i = 0; i < MAX_RESULTS; i++)
    {
        data->results[i].id = -1;
    }
}

/* Função que inicia os processos dos clientes, intermediários e
 * empresas. Para tal, pode usar as funções launch_*,
 * guardando os pids resultantes nos arrays respetivos
 * da estrutura data.
 */
void launch_processes(struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    for (int i = 0; i < data->n_clients; i++)
    {
        pid_t pid = launch_client(i, buffers, data, sems);
        data->client_pids[i] = pid;
    }

    for (int j = 0; j < data->n_enterprises; j++)
    {
        pid_t pid = launch_enterp(j, buffers, data, sems);
        data->enterprise_pids[j] = pid;
    }

    for (int k = 0; k < data->n_intermediaries; k++)
    {
        pid_t pid = launch_interm(k, buffers, data, sems);
        data->intermediary_pids[k] = pid;
    }
}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
 * op - cria uma nova operação, através da função create_request
 * status - verifica o estado de uma operação através da função read_status
 * stop - termina o execução do AdmPor através da função stop_execution
 * help - imprime informação sobre os comandos disponiveis
 */
void user_interaction(struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    char nome[10];
    int counter = 0;

    logF = logOpen(data->log_filename);
    while (1)
    {
        printf("Introduza a operação desejada \n");

        scanf("%s", nome);
        if (strcmp(nome, "op") == 0)
        {
            create_request(&counter, buffers, data, sems);
        }
        else if (strcmp(nome, "status") == 0)
        {
            read_status(data, sems);
        }
        else if (strcmp(nome, "stop") == 0)
        {
            writeFileStop(logF);
            stop_execution(data, buffers, sems);
            return;
        }
        else if (strcmp(nome, "help") == 0)
        {
            writeFileHelp(logF);
            printf("op - cria uma nova operação, através da função create_request \n status - verifica o estado de uma operação através da função read_status \n stop - termina o execução do AdmPor através da função stop_execution \n help - imprime informação sobre os comandos disponiveis \n");
        }
    }
}

/* Cria uma nova operação identificada pelo valor atual de op_counter e com os
 * dados introduzidos pelo utilizador na linha de comandos, escrevendo a mesma
 * no buffer de memória partilhada entre main e clientes. Imprime o id da
 * operação e incrementa o contador de operações op_counter. Não deve criar
 * mais operações para além do tamanho do array data->results.
 */
void create_request(int *op_counter, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    if (*op_counter >= MAX_RESULTS)
    {
        printf("Numero máximo de operações excedido.");
    }
    else
    {
        struct operation nOp;
        int cliente;
        int empresa;

        printf("Insira o id de cliente \n");
        scanf("%d", &cliente);
        printf("Insira o id de empresa \n");
        scanf("%d", &empresa);

        writeFileOp(logF, cliente, empresa);

        nOp.id = *op_counter;
        nOp.requesting_client = cliente;
        nOp.requested_enterp = empresa;
        nOp.status = 'M';

        registerTime(&nOp.start_time);
        printf("Id da operação criada: %d \n", *op_counter);
        (*op_counter)++;

        produce_begin(sems->main_client);
        write_main_client_buffer(buffers->main_client, data->buffers_size, &nOp);
        produce_end(sems->main_client);
    }
}

/* Função que lê um id de operação do utilizador e verifica se a mesma é valida.
 * Em caso afirmativo imprime informação da mesma, nomeadamente o seu estado, o
 * id do cliente que fez o pedido, o id da empresa requisitada, e os ids do cliente,
 * intermediário, e empresa que a receberam e processaram.
 */
void read_status(struct main_data *data, struct semaphores *sems)
{
    int id;
    printf("Insira o id da op pretendida \n");
    scanf("%d", &id);
    writeFileStatus(logF, id);
    semaphore_mutex_lock(sems->results_mutex);
    if (id != -1 && id < MAX_RESULTS && data->results[id].id != -1)
    {
        printf("Operation status: %c \n", data->results[id].status);
        printf("Requesting client: %i \n", data->results[id].requesting_client);
        printf("Requested enterp: %i \n", data->results[id].requested_enterp);
        printf("Receiving client: %i \n", data->results[id].receiving_client);
        printf("Receiving interm: %i \n", data->results[id].receiving_interm);
        printf("Receveing enterp: %i \n", data->results[id].receiving_enterp);
    }
    else
    {
        printf("No such operation \n");
    }
    semaphore_mutex_unlock(sems->results_mutex);
}

/* Função que termina a execução do programa AdmPor. Deve começar por
 * afetar a flag data->terminate com o valor 1. De seguida, e por esta
 * ordem, deve esperar que os processos filho terminem, deve escrever as
 * estatisticas finais do programa, e por fim libertar
 * as zonas de memória partilhada e dinâmica previamente
 * reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
 */
void stop_execution(struct main_data *data, struct comm_buffers *buffers, struct semaphores *sems)
{
    *data->terminate = 1;
    closeLogFile(logF);
    wakeup_processes(data, sems);
    wait_processes(data);
    write_statistics(data);
    destroy_memory_buffers(data, buffers);
    destroy_semaphores(sems);
}

/* Função que espera que todos os processos previamente iniciados terminem,
 * incluindo clientes, intermediários e empresas. Para tal, pode usar a função
 * wait_process do process.h.
 */
void wait_processes(struct main_data *data)
{
    for (int i = 0; i < data->n_clients; i++)
    {
        data->client_stats[i] = wait_process(data->client_pids[i]);
    }
    for (int k = 0; k < data->n_intermediaries; k++)
    {
        data->intermediary_stats[k] = wait_process(data->intermediary_pids[k]);
    }
    for (int j = 0; j < data->n_enterprises; j++)
    {
        data->enterprise_stats[j] = wait_process(data->enterprise_pids[j]);
    }
}

/* Função que imprime as estatisticas finais do AdmPor, nomeadamente quantas
 * operações foram processadas por cada cliente, intermediário e empresa.
 */
void write_statistics(struct main_data *data)
{
    writeStatsFile(data);
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
 * reservados na estrutura data.
 */
void destroy_memory_buffers(struct main_data *data, struct comm_buffers *buffers)
{
    destroy_dynamic_memory(data->client_pids);
    destroy_dynamic_memory(data->client_stats);
    destroy_dynamic_memory(data->enterprise_pids);
    destroy_dynamic_memory(data->enterprise_stats);
    destroy_dynamic_memory(data->intermediary_pids);
    destroy_dynamic_memory(data->intermediary_stats);

    destroy_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, buffers->client_interm->buffer, data->buffers_size * sizeof(struct operation));
    destroy_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, buffers->interm_enterp->buffer, data->buffers_size * sizeof(struct operation));
    destroy_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, buffers->main_client->buffer, data->buffers_size * sizeof(struct operation));

    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));
    destroy_shared_memory(STR_SHM_RESULTS, data->results, sizeof(struct operation) * MAX_RESULTS);

    destroy_shared_memory(STR_SHM_MAIN_CLIENT_PTR, buffers->main_client->ptrs, sizeof(int) * data->buffers_size);
    destroy_shared_memory(STR_SHM_CLIENT_INTERM_PTR, buffers->client_interm->ptrs, sizeof(struct operation));
    destroy_shared_memory(STR_SHM_INTERM_ENTERP_PTR, buffers->interm_enterp->ptrs, sizeof(int) * data->buffers_size);
}

/* Função que inicializa os semáforos da estrutura semaphores. Semáforos
 * *_full devem ser inicializados com valor 0, semáforos *_empty com valor
 * igual ao tamanho dos buffers de memória partilhada, e os *_mutex com
 * valor igual a 1. Para tal pode ser usada a função semaphore_create.*/
void create_semaphores(struct main_data *data, struct semaphores *sems)
{
    sems->main_client->empty = semaphore_create(STR_SEM_MAIN_CLIENT_EMPTY, data->buffers_size);
    sems->main_client->full = semaphore_create(STR_SEM_MAIN_CLIENT_FULL, 0);
    sems->main_client->mutex = semaphore_create(STR_SEM_MAIN_CLIENT_MUTEX, 1);

    sems->client_interm->empty = semaphore_create(STR_SEM_CLIENT_INTERM_EMPTY, data->buffers_size);
    sems->client_interm->full = semaphore_create(STR_SEM_CLIENT_INTERM_FULL, 0);
    sems->client_interm->mutex = semaphore_create(STR_SEM_CLIENT_INTERM_MUTEX, 1);

    sems->interm_enterp->empty = semaphore_create(STR_SEM_INTERM_ENTERP_EMPTY, data->buffers_size);
    sems->interm_enterp->full = semaphore_create(STR_SEM_INTERM_ENTERP_FULL, 0);
    sems->interm_enterp->mutex = semaphore_create(STR_SEM_INTERM_ENTERP_MUTEX, 1);

    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

/* Função que acorda todos os processos adormecidos em semáforos, para que
 * estes percebam que foi dada ordem de terminação do programa. Para tal,
 * pode ser usada a função produce_end sobre todos os conjuntos de semáforos
 * onde possam estar processos adormecidos e um número de vezes igual ao
 * máximo de processos que possam lá estar.*/
void wakeup_processes(struct main_data *data, struct semaphores *sems)
{
    for (int i = 0; i < data->n_clients; i++)
    {
        produce_end(sems->main_client);
    }
    for (int i = 0; i < data->n_intermediaries; i++)
    {
        produce_end(sems->client_interm);
    }
    for (int i = 0; i < data->n_enterprises; i++)
    {
        produce_end(sems->interm_enterp);
    }
}

/* Função que liberta todos os semáforos da estrutura semaphores. */
void destroy_semaphores(struct semaphores *sems)
{
    semaphore_destroy(STR_SEM_MAIN_CLIENT_EMPTY, sems->main_client->empty);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_FULL, sems->main_client->full);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_MUTEX, sems->main_client->mutex);

    semaphore_destroy(STR_SEM_CLIENT_INTERM_EMPTY, sems->client_interm->empty);
    semaphore_destroy(STR_SEM_CLIENT_INTERM_FULL, sems->client_interm->full);
    semaphore_destroy(STR_SEM_CLIENT_INTERM_MUTEX, sems->client_interm->mutex);

    semaphore_destroy(STR_SEM_INTERM_ENTERP_EMPTY, sems->interm_enterp->empty);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_FULL, sems->interm_enterp->full);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_MUTEX, sems->interm_enterp->mutex);

    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
}
