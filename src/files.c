#include "../headers/files.h"

int get_files(int fds[])
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

        if (dup2(fds[1], 1) == -1)
        {
            perror("dup2 failed");
            close(fds[1]);
            return 3;
        }

        if (dup2(fds[1], 2) == -1)
        {
            perror("dup2 failed");
            close(fds[1]);
            return 3;
        }

        execlp("ls", "ls", NULL);

        perror("execlp failed");
        close(fds[1]);
        return 4;
    }

    close(fds[1]);

    wait(NULL);

    return 0;
}