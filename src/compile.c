#include "../headers/compile.h"

ERROR compile_file(char *filename, char *output, int fds[], int *status)
{
    if (pipe(fds) == -1)
    {
        perror("pipe failed");
        return PIPE_FAIL;
    }

    int pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return FORK_FAIL;
    }

    if (!pid)
    {
        close(fds[0]);

        if (dup2(fds[1], 2) == -1)
        {
            perror("dup2 failed");
            close(fds[1]);
            return DUP2_FAIL;
        }

        execlp("gcc", "gcc", "-o", output, filename, NULL);

        perror("execlp failed");
        close(fds[1]);
        return EXECLP_FAIL;
    }

    close(fds[1]);

    wait(status);

    return SUCCESS;
}