/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */
#ifndef APSIGNAL_H_GUARD
#define APSIGNAL_H_GUARD

#include "main.h"
#include "synchronization.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

/**
 * Função que vai dar set a um alarm de alarm_time em alarm_time.
 * É usado a função setitimer porque a função alarm() está obsuleta.
 */
void setAlarm();

/**
 * Função que vai fazer mudar o comportamento do programa quando
 * na presença de um sinal SIGINT (CtrlC). Fai fazer com que o programa
 * termine corretamente.
 */
void setStopSignal();

/**
 * Printa o estado atual das operções quando recebe o SIGALRM
 */
void handleTimerSignal();

/**
 * Printa uma operação
 */
void printOp(struct operation op);

/**
 * Função que para a execução
 */
void callStopExec();

/**
 * Liberta a memória alocada pela main
 */
void destroyBuffers();

/**
 * Funão que faz com que nada aconteça na presença de um SIGINT(CtrlC)
 */
void defaultExec();
#endif