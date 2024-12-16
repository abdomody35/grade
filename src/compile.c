#include "../headers/compile.h"

int compile_file(char *filename, char *output, int fds[], int *status)
{
    if (pipe(fds) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    int pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return 2;
    }

    if (!pid)
    {
        close(fds[0]);

        if (dup2(fds[1], 2) == -1)
        {
            perror("dup2 failed");
            close(fds[1]);
            return 3;
        }

        execlp("gcc", "gcc", "-o", output, filename, NULL);

        perror("Compiler failed");
        close(fds[1]);
        return 5;
    }

    close(fds[1]);

    wait(status);

    return 0;
}