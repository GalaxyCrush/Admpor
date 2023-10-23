#ifndef CONFIGURATIONL_H_GUARD
#define CONFIGURATION_H_GUARD

#include "main.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Função que abre um ficheiro para ser lido.
 * Devolve um ponteiro para o mesmo.
 */
FILE *openFile(char filename[]);

/**
 * Função que fecha um ficheiro.
 */
void closeFile(FILE *file);

/**
 * Função que lê os argumentos de um ficheiro, guardando-os na struct main_data e extraAtr.
 */
void readInputFromFile(FILE *file, struct main_data *data);

#endif
