/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */
#include <malloc.h>
#include <fcntl.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "main.h"

/* Função que reserva uma zona de memória partilhada com tamanho indicado
 * por size e nome name, preenche essa zona de memória com o valor 0, e
 * retorna um apontador para a mesma. Pode concatenar o resultado da função
 * getuid() a name, para tornar o nome único para o processo.
 */
void *create_shared_memory(char *name, int size)
{
    int shm_d;
    void *ptr;

    // Concatenar o uid com o nome dado
    int id = getuid();
    char idname[100];
    idname[0] = 0;
    sprintf(idname, "%d", id);
    strcat(idname, name);
    //-------------------------

    shm_d = shm_open(idname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_d == -1)
    {
        perror("open");
        exit(1);
    }
    int guard = ftruncate(shm_d, size);
    if (guard == -1)
    {
        perror("ftruncate");
        exit(2);
    }
    ptr = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED, shm_d, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(3);
    }
    memset(ptr, 0, size);
    return ptr;
}

/* Função que reserva uma zona de memória dinâmica com tamanho indicado
 * por size, preenche essa zona de memória com o valor 0, e retorna um
 * apontador para a mesma.
 */
void *create_dynamic_memory(int size)
{
    void *ptr;
    ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

/* Função que liberta uma zona de memória dinâmica previamente reservada.
 */
void destroy_shared_memory(char *name, void *ptr, int size)
{

    int id = getuid();
    char idname[100];
    idname[0] = 0;

    sprintf(idname, "%d", id);
    strcat(idname, name);

    int guard = munmap(ptr, size);
    if (guard == -1)
    {
        perror("munmap");
        exit(1);
    }
    int guard2 = shm_unlink(idname);
    if (guard2 == -1)
    {
        perror("unlink");
        exit(2);
    }
}

/* Função que liberta uma zona de memória partilhada previamente reservada.
 */
void destroy_dynamic_memory(void *ptr)
{
    free(ptr);
}

/* Função que escreve uma operação no buffer de memória partilhada entre a Main
 * e os clientes. A operação deve ser escrita numa posição livre do buffer,
 * tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
 * Se não houver nenhuma posição livre, não escreve nada.
 */
void write_main_client_buffer(struct rnd_access_buffer *buffer, int buffer_size, struct operation *op)
{
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer->ptrs[i] == 0)
        {
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = 1;
            return;
        }
    }
}

/* Função que escreve uma operação no buffer de memória partilhada entre os clientes
 * e intermediários. A operação deve ser escrita numa posição livre do buffer,
 * tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
 * Se não houver nenhuma posição livre, não escreve nada.
 */
void write_client_interm_buffer(struct circular_buffer *buffer, int buffer_size, struct operation *op)
{
    int ptrWrt = buffer->ptrs->in;
    int ptrRd = buffer->ptrs->out;

    if (((ptrWrt + 1) % buffer_size) == ptrRd) // Se true então buffer está cheio
    {
        return;
    }

    buffer->buffer[ptrWrt] = *op;
    buffer->ptrs->in = (ptrWrt + 1) % buffer_size;
}

/* Função que escreve uma operação no buffer de memória partilhada entre os intermediários
 * e as empresas. A operação deve ser escrita numa posição livre do buffer,
 * tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
 * Se não houver nenhuma posição livre, não escreve nada.
 */
void write_interm_enterp_buffer(struct rnd_access_buffer *buffer, int buffer_size, struct operation *op)
{
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer->ptrs[i] == 0)
        {
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = 1;
            return;
        }
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre a Main
 * e os clientes, se houver alguma disponível para ler que seja direcionada ao cliente especificado.
 * A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
 * Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
 */
void read_main_client_buffer(struct rnd_access_buffer *buffer, int client_id, int buffer_size, struct operation *op)
{
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer->ptrs[i] == 1)
        {
            if (buffer->buffer[i].requesting_client == client_id)
            {
                *op = buffer->buffer[i];
                buffer->ptrs[i] = 0;
                return;
            }
        }
    }
    op->id = -1;
}

/* Função que lê uma operação do buffer de memória partilhada entre os clientes e intermediários,
 * se houver alguma disponível para ler (qualquer intermediário pode ler qualquer operação).
 * A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
 * Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
 */
void read_client_interm_buffer(struct circular_buffer *buffer, int buffer_size, struct operation *op)
{
    int ptrWrt = buffer->ptrs->in;
    int ptrRd = buffer->ptrs->out;

    if (ptrWrt == ptrRd) // Buffer vazio
    {
        op->id = -1;
        return;
    }

    *op = buffer->buffer[ptrRd];
    buffer->ptrs->out = (ptrRd + 1) % buffer_size;
}

/* Função que lê uma operação do buffer de memória partilhada entre os intermediários e as empresas,
 * se houver alguma disponível para ler dirijida à empresa especificada. A leitura deve
 * ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
 * nenhuma operação disponível, afeta op->id com o valor -1.
 */
void read_interm_enterp_buffer(struct rnd_access_buffer *buffer, int enterp_id, int buffer_size, struct operation *op)
{
    for (int i = 0; i < buffer_size; i++)
    {
        if (buffer->ptrs[i] == 1)
        {
            if (buffer->buffer[i].requested_enterp == enterp_id)
            {
                *op = buffer->buffer[i];
                buffer->ptrs[i] = 0;
                return;
            }
        }
    }
    op->id = -1;
}
