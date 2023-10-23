/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */
#include "memory.h"
#include "main.h"
#include "intermediary.h"
#include "synchronization.h"
#include "aptime.h"
#include "apsignal.h"

/* Função principal de um Intermediário. Deve executar um ciclo infinito onde em
 * cada iteração lê uma operação dos clientes e se a mesma tiver id
 * diferente de -1 e se data->terminate ainda for igual a 0, processa-a e
 * envia a mesma para as empresas. Operações com id igual a -1 são
 * ignoradas (op inválida) e se data->terminate for igual a 1 é porque foi
 * dada ordem de terminação do programa, portanto deve-se fazer return do
 * número de operações processadas. Para efetuar estes passos, pode usar os
 * outros métodos auxiliares definidos em intermediary.h.
 */
int execute_intermediary(int interm_id, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    int count = 0;
    struct operation atual;
    setStopSignal();
    while (*data->terminate != 1)
    {
        intermediary_receive_operation(&atual, buffers, data, sems);
        if (atual.id != -1)
        {
            intermediary_process_operation(&atual, interm_id, data, &count, sems);
            intermediary_send_answer(&atual, buffers, data, sems);
        }
    }
    return count;
}

/* Função que lê uma operação do buffer de memória partilhada entre clientes e intermediários.
 * Antes de tentar ler a operação, deve verificar se data->terminate tem valor 1.
 * Em caso afirmativo, retorna imediatamente da função.
 */
void intermediary_receive_operation(struct operation *op, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    if (*data->terminate == 1)
    {
        return;
    }
    else
    {
        consume_begin(sems->client_interm);
        read_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
        consume_end(sems->client_interm);
    }
}

/* Função que processa uma operação, alterando o seu campo receiving_intermediary para o id
 * passado como argumento, alterando o estado da mesma para 'I' (intermediary), e
 * incrementando o contador de operações. Atualiza também a operação na estrutura data.
 */
void intermediary_process_operation(struct operation *op, int interm_id, struct main_data *data, int *counter, struct semaphores *sems)
{
    op->receiving_interm = interm_id;
    op->status = 'I';
    (*counter)++;

    registerTime(&op->intermed_time);

    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);
}

/* Função que escreve uma operação no buffer de memória partilhada entre
 * intermediários e empresas.
 */
void intermediary_send_answer(struct operation *op, struct comm_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    produce_begin(sems->interm_enterp);
    write_interm_enterp_buffer(buffers->interm_enterp, data->buffers_size, op);
    produce_end(sems->interm_enterp);
}
