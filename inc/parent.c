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

int main(void)
{

    FileMapping *shared = fileMappingCreate("file");

    char *line = NULL;
    size_t capacity = 0; // размер выделенного буфера
    ssize_t length;

    size_t buffer_capacity = 4096;
    char buffer[buffer_capacity];
    ssize_t received = 0;

    while ((length = getline(&line, &capacity, stdin)) != -1)
    {
        received += length;
        if (received > buffer_capacity)
        {
            while (received > buffer_capacity)
                buffer_capacity *= 2;

            char tmp[buffer_capacity];
            strcpy(tmp, buffer);

            *buffer = tmp;
        }
        for (int i = received - length; i < received; ++i)
        {
            buffer[i] = line[i - (received - length)];
        }
    }

    // fileMappingWrite(shared, line, (size_t)length);

    pid_t child1_pid = fork();
    // Отныне код выполняется двумя процессами: родительский и дочерний
    // child1_pid = PID > 0 (в родительском)
    // child1_pid = 0 в дочернем и выполняется блок ниже:
    if (child1_pid == 0)
    { // если мы внутри дочернего процесса
        char *args[] = {"./child1", NULL};
        execv("./child1", args); // Далее код выполняется только родителем
    }

    pid_t child2_pid = fork();
    // Снова код выполняется родителем и дочерним процессом
    if (child2_pid == 0)
    {
        char *args[] = {"./child2", NULL};
        execv("./child2", args);
    }

    buffer[4096];

    // Цикл нужен для случая, когда child2 пишет в pipe[2] больше, чем 4096 символов
    while ((received = read(pipes[2][0], buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[received] = '\0';
        printf("%s", buffer);
    }

    free(line);

    int status;
    waitpid(child1_pid, &status, 0);
    waitpid(child2_pid, &status, 0);

    return 0;
}

// cmake