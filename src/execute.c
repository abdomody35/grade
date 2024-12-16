#include "../headers/execute.h"

ERROR execute_program(int fds[], char *output, StrArray *args, int *pid)
{
    if (pipe(fds) == -1)
    {
        perror("pipe failed");
        return PIPE_FAIL;
    }

    *pid = fork();

    if (*pid == -1)
    {
        perror("fork failed");
        return FORK_FAIL;
    }

    if (!(*pid))
    {
        close(fds[0]);

        if (dup2(fds[1], 1) == -1)
        {
            perror("dup2 failed");
            return DUP2_FAIL;
        }

        if (dup2(fds[1], 2) == -1)
        {
            perror("dup2 failed");
            return DUP2_FAIL;
        }

        if (inputFile)
        {
            int input = open(inputFile, O_RDONLY);

            if (input == -1)
            {
                perror("open failed");
                return OPEN_FAIL;
            }

            if (dup2(input, 0) == -1)
            {
                perror("dup2 failed");
                return DUP2_FAIL;
            }
        }
        else
        {
            close(0);
        }

        ERROR error = update_string(args, 0, output);

        if (error)
        {
            return error;
        }

        execv(output, args->elements);

        perror("execv failed");
        return EXECV_FAIL;
    }

    close(fds[1]);

    return SUCCESS;
}

ERROR update_elapsed(int *elapsed, int *status, int pid)
{
    while (*elapsed < EXEC_TIMEOUT)
    {
        int error = waitpid(pid, status, WNOHANG);
        
        if (error == -1)
        {
            perror("waitpid failed");
            return WAITPID_FAIL;
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

    return SUCCESS;
}