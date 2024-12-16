#include "../headers/execute.h"

int execute_program(int fds[], char *output, StrArray *args, int *pid)
{
    if (pipe(fds) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    *pid = fork();

    if (*pid == -1)
    {
        perror("fork failed");
        return 2;
    }

    if (!(*pid))
    {
        close(fds[0]);

        if (dup2(fds[1], 1) == -1)
        {
            perror("dup2 failed");
            return 3;
        }

        if (dup2(fds[1], 2) == -1)
        {
            perror("dup2 failed");
            return 3;
        }

        if (inputFile)
        {
            int input = open(inputFile, O_RDONLY);

            if (input == -1)
            {
                perror("open failed");
                return 9;
            }

            if (dup2(input, 0) == -1)
            {
                perror("dup2 failed");
                return 3;
            }
        }
        else
        {
            close(0);
        }

        if (update_string(args, 0, output) == -1)
        {
            return 12;
        }

        execv(output, args->elements);

        perror("execv failed");
        return 8;
    }

    close(fds[1]);

    return 0;
}

int update_elapsed(int *elapsed, int *status, int pid)
{
    while (*elapsed < EXEC_TIMEOUT)
    {
        int error = waitpid(pid, status, WNOHANG);
        if (error == -1)
        {
            perror("waitpid failed");
            return 7;
        }
        else if (error == 0)
        {
            sleep(1);
            *elapsed++;
            continue;
        }
        else
        {
            break;
        }
    }

    return 0;
}