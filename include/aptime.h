/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */
#ifndef APTIME_H_GUARD
#define APTIME_H_GUARD

#include <time.h>
#include <stdio.h>

/**
 * Função que regista o tempo de quando um pedido foi criado
 */
void registerTime(struct timespec *time);

#endif