/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "stats.h"
#include <time.h>

/**
 * Função que escreve as estatisticas finais do programa
 */
void writeStatsFile(struct main_data *data)
{
    FILE *file;
    if ((file = fopen(data->statistics_filename, "w")) == NULL)
    {
        perror("OpenFile");
        exit(-1);
    }

    fprintf(file, "Process Statistics: \n");

    for (int i = 0; i < data->n_clients; i++)
    {
        if (data->client_stats[i] != 0)
        {
            fprintf(file, "     Client: %d received %d operation (s)! \n", i, data->client_stats[i]);
        }
    }
    for (int i = 0; i < data->n_intermediaries; i++)
    {
        if (data->intermediary_stats[i] != 0)
        {
            fprintf(file, "     Intermediary: %d received %d operation (s)! \n", i, data->intermediary_stats[i]);
        }
    }
    for (int i = 0; i < data->n_enterprises; i++)
    {
        if (data->enterprise_stats[i] != 0)
        {
            fprintf(file, "     Enterprise: %d received %d operation (s)! \n", i, data->enterprise_stats[i]);
        }
    }
    fprintf(file, "Request Statistics: \n");

    for (int i = 0; i < MAX_RESULTS; i++)
    {
        struct operation aux = data->results[i];
        if (aux.id != -1)
        {
            long msStart = ceil(aux.start_time.tv_nsec / 1.0e6);
            long msInterm = ceil(aux.intermed_time.tv_nsec / 1.0e6);
            long msEnterp = ceil(aux.enterp_time.tv_nsec / 1.0e6);

            fprintf(file, "Request: %d\n", aux.id);
            fprintf(file, "Status: %c\n", aux.status);
            fprintf(file, "Intermediary id: %d\n", aux.receiving_interm);
            fprintf(file, "Enterprise id: %d\n", aux.receiving_enterp);
            fprintf(file, "Start time: %d-%d-%d %d:%d:%d.%3ld\n",
                    localtime(&aux.start_time.tv_sec)->tm_mday,
                    localtime(&aux.start_time.tv_sec)->tm_mon + 1,
                    localtime(&aux.start_time.tv_sec)->tm_year + 1900,
                    localtime(&aux.start_time.tv_sec)->tm_hour,
                    localtime(&aux.start_time.tv_sec)->tm_min,
                    localtime(&aux.start_time.tv_sec)->tm_sec,
                    msStart);
            fprintf(file, "Intermediary time: %d-%d-%d %d:%d:%d.%3ld\n",
                    localtime(&aux.intermed_time.tv_sec)->tm_mday,
                    localtime(&aux.intermed_time.tv_sec)->tm_mon + 1,
                    localtime(&aux.intermed_time.tv_sec)->tm_year + 1900,
                    localtime(&aux.intermed_time.tv_sec)->tm_hour,
                    localtime(&aux.intermed_time.tv_sec)->tm_min,
                    localtime(&aux.intermed_time.tv_sec)->tm_sec,
                    msInterm);
            fprintf(file, "Enterp time: %d-%d-%d %d:%d:%d.%3ld\n",
                    localtime(&aux.enterp_time.tv_sec)->tm_mday,
                    localtime(&aux.enterp_time.tv_sec)->tm_mon + 1,
                    localtime(&aux.enterp_time.tv_sec)->tm_year + 1900,
                    localtime(&aux.enterp_time.tv_sec)->tm_hour,
                    localtime(&aux.enterp_time.tv_sec)->tm_min,
                    localtime(&aux.enterp_time.tv_sec)->tm_sec,
                    msInterm);
            double totalTime = difftime(aux.enterp_time.tv_sec, aux.start_time.tv_sec);
            fprintf(file, "Total time: %.3f\n ", totalTime);
        }
    }
    fclose(file);
}
