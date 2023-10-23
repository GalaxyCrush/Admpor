/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "configuration.h"

/**
 * Função que abre um ficheiro para ser lido.
 * Devolve um ponteiro para o mesmo.
 */
FILE *openFile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("OpenFile");
        exit(-1);
    }
    return file;
}

/**
 * Função que fecha um ficheiro.
 */
void closeFile(FILE *file)
{
    fclose(file);
}

/**
 * Função que lê os argumentos de um ficheiro, guardando-os na struct main_data e extraAtr.
 */
void readInputFromFile(FILE *file, struct main_data *data)
{
    int max_lines = 8;
    int max_len = 20;

    int linha = 0;

    char inputs[max_lines][max_len]; //[N_linha][Conteudo_linha]

    while (!feof(file) && !ferror(file))
    {
        if (fgets(inputs[linha], max_len, file) != NULL)
        {
            linha++;
        }
    }

    sscanf(inputs[0], "%d", &data->max_ops);
    sscanf(inputs[1], "%d", &data->buffers_size);
    sscanf(inputs[2], "%d", &data->n_clients);
    sscanf(inputs[3], "%d", &data->n_intermediaries);
    sscanf(inputs[4], "%d", &data->n_enterprises);
    sscanf(inputs[5], "%s", data->log_filename);
    sscanf(inputs[6], "%s", data->statistics_filename);
    sscanf(inputs[7], "%d", &data->alarm_time);
}
