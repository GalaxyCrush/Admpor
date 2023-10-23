/**
 * @author João Pereira n58189
 * @author Daniel Nunes n58257
 * @author André Reis n58192
 * Grupo: SO-002
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "log.h"

/**
 * Função que abre um ficheiro para ser lido.
 * Devolve um ponteiro para o mesmo.
 */
FILE *logOpen(char filename[])
{
    FILE *log_file;
    if ((log_file = fopen(filename, "w")) == NULL)
    {
        perror("OpenFile");
        exit(-1);
    }
    return log_file;
}

/**
 * Função que vai fechar o ficheiro de log
 */
void closeLogFile(FILE *logF)
{
    fclose(logF);
}

/**
 * Função que vai escrever o log do caso da opção "op"
 */
void writeFileOp(FILE *logF, int cliente, int empresa)
{
    char *date = getDate();
    char userInter[10];
    char buffer[100];
    memset(buffer, 0, 100);
    snprintf(userInter, sizeof(buffer), " op %d %d\n", cliente, empresa);
    strcat(buffer, date);
    strcat(buffer, userInter);

    for (int i = 0; i < sizeof(buffer); i++)
    {
        char aux = buffer[i];
        if (aux == '\n')
        {
            fputc(aux, logF);
            break;
        }
        fputc(aux, logF);
    }
    free(date);
}

/**
 * Função que vai escrever o log do caso da opção "help"
 */
void writeFileHelp(FILE *logF)
{
    char *date = getDate();
    char buffer[100];
    memset(buffer, 0, 100);
    printf("%s\n ", buffer);
    strcat(buffer, date);
    strcat(buffer, " help\n");
    for (int i = 0; i < sizeof(buffer); i++)
    {
        char aux = buffer[i];
        if (aux == '\n')
        {
            fputc(aux, logF);
            break;
        }
        fputc(aux, logF);
    }
    free(date);
}

/**
 * Função que vai escrever o log do caso da opção "Status"
 */
void writeFileStatus(FILE *logF, int opId)
{
    char *date = getDate();
    char userInter[13];
    char buffer[100];
    memset(buffer, 0, 100);
    snprintf(userInter, sizeof(buffer), " status %d\n", opId);

    strcat(buffer, date);
    strcat(buffer, userInter);

    for (int i = 0; i < sizeof(buffer); i++)
    {
        char aux = buffer[i];
        if (aux == '\n')
        {
            fputc(aux, logF);
            break;
        }
        fputc(aux, logF);
    }
    free(date);
}

/**
 * Função que vai escrever o log do caso da opção "Stop"
 */
void writeFileStop(FILE *logF)
{
    char *date = getDate();
    char buffer[100];
    memset(buffer, 0, 100);
    strcat(buffer, date);
    strcat(buffer, " stop\n");

    for (int i = 0; i < sizeof(buffer); i++)
    {
        char aux = buffer[i];
        if (aux == '\n')
        {
            fputc(aux, logF);

            break;
        }
        fputc(aux, logF);
    }
    free(date);
}

/**
 * Função que vai retornar a data atual
 */
char *getDate()
{
    struct timespec actualT;
    clock_gettime(CLOCK_REALTIME, &actualT);
    time_t s = actualT.tv_sec;
    long ms = ceil(actualT.tv_nsec / 1.0e6);
    if (ms > 999)
    {
        s++;
        ms = 0;
    }
    time_t t = time(NULL);
    int hour = localtime(&t)->tm_hour;
    int min = localtime(&t)->tm_min;
    int sec = localtime(&t)->tm_sec;
    int day = localtime(&t)->tm_mday;
    int month = localtime(&t)->tm_mon + 1;
    int year = localtime(&t)->tm_year + 1900;
    char *buffer = malloc(sizeof(char) * 100);
    snprintf(buffer, 100, "%d-%d-%d %02d:%02d:%02d.%03ld", year, month, day, hour, min, sec, ms);
    return buffer;
}