/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */
#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include <stdlib.h>
#include <stdio.h>
#include "main.h"

/**
 * Função que abre um ficheiro para ser lido.
 * Devolve um ponteiro para o mesmo.
 */
FILE *logOpen(char filename[]);

/**
 * Função que vai fechar o ficheiro de log
 */
void closeLogFile(FILE *logF);

/**
 * Função que vai escrever o log do caso da opção "op"
 */
void writeFileOp(FILE *logF, int cliente, int opId);

/**
 * Função que vai escrever o log do caso da opção "help"
 */
void writeFileHelp(FILE *logF);

/**
 * Função que vai escrever o log do caso da opção "Status"
 */
void writeFileStatus(FILE *logF, int opId);

/**
 * Função que vai escrever o log do caso da opção "Stop"
 */
void writeFileStop(FILE *logF);

/**
 * Função que vai retornar a data atual
 */
char *getDate();

#endif
