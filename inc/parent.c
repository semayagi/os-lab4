/*
stdin → parent → pipe[0] → child1 → pipe[1] → child2 → pipe[2] → parent → stdout
        читает              UPPER              пробел→_
        строки              CASE


*/
#define _POSIX_C_SOURCE 200809L // Portable Operating System Interface

#include "fileMapping.h"

#include <stdio.h>     // getline, printf, stdin
#include <stdlib.h>    // free
#include <unistd.h>    // write, read, fork, dup2, close, execv
#include <sys/types.h> // pid_t, ssize_t
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main(void)
{
    FILE *out = fopen("file", "wb");
    if (out == NULL)
    {
        printf("main - fopen failed, %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    char *line = NULL;
    size_t capacity = 0;
    ssize_t length;

    size_t total_length = 0;
    while ((length = getline(&line, &capacity, stdin)) != -1)
    {
        total_length += length;
        if (fwrite(line, 1, (size_t)length, out) != (size_t)length)
        {
            printf("main - fwrite failed, %s\n", strerror(errno));
            free(line);
            fclose(out);
            return EXIT_FAILURE;
        }
    }
    free(line);
    fclose(out);
    if (total_length == 0)
    {
        return 0;
    }

    FileMapping *shared = fileMappingCreate("file");
    if (shared == NULL)
    {

        return EXIT_FAILURE;
    }

    pid_t child1_pid = fork();
    if (child1_pid == -1)
    {
        printf("main - fork for child1 failed, %s\n", strerror(errno));
        fileMappingClose(shared);
        return EXIT_FAILURE;
    }
    // Отныне код выполняется двумя процессами: родительский и дочерний
    // child1_pid = PID > 0 (в родительском)
    // child1_pid = 0 в дочернем и выполняется блок ниже:
    if (child1_pid == 0)
    { // если мы внутри дочернего процесса
        char *args[] = {"./child1", "file", NULL};
        int result = execv("./child1", args); // Далее код выполняется только родителем
        if (result != 0)
            return EXIT_FAILURE;
    }

    int status;
    pid_t ret = waitpid(child1_pid, &status, 0);
    if (ret == -1)
    {
        printf("main - waitpid for child1 failed, %s\n", strerror(errno));
        fileMappingClose(shared);
        return EXIT_FAILURE;
    }

    pid_t child2_pid = fork();
    if (child2_pid == -1)
    {
        printf("main - fork for child2 failed, %s\n", strerror(errno));
        fileMappingClose(shared);
        return EXIT_FAILURE;
    }
    // Снова код выполняется родителем и дочерним процессом
    if (child2_pid == 0)
    {
        char *args[] = {"./child2", "file", NULL};
        int result = execv("./child2", args);
        if (result != 0)
            return EXIT_FAILURE;
    }

    ret = waitpid(child2_pid, &status, 0);
    if (ret == -1)
    {
        printf("main - waitpid for child2 failed, %s\n", strerror(errno));
        fileMappingClose(shared);
        return EXIT_FAILURE;
    }

    unsigned char *data = fileMappingGetPointer(shared);
    size_t size = fileMappingGetSize(shared);
    printf("\n");
    for (size_t i = 0; i < size; ++i)
    {
        printf("%c", data[i]);
    }
    printf("\n");
    fileMappingClose(shared);

    return 0;
}