#include "../headers/files.h"

ERROR get_files(int fds[])
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

        if (dup2(fds[1], 1) == -1)
        {
            perror("dup2 failed");
            close(fds[1]);
            return DUP2_FAIL;
        }

        if (dup2(fds[1], 2) == -1)
        {
            perror("dup2 failed");
            close(fds[1]);
            return DUP2_FAIL;
        }

        execlp("ls", "ls", NULL);

        perror("execlp failed");
        close(fds[1]);
        return EXECLP_FAIL;
    }

    close(fds[1]);

    wait(NULL);

    return SUCCESS;
}