/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "aptime.h"

/**
 * Função que regista o tempo de quando um pedido foi criado
 */
void registerTime(struct timespec *time)
{
    if (clock_gettime(CLOCK_REALTIME, time) == -1)
    {
        perror("RegisterTime");
        exit(-1);
    }
}
