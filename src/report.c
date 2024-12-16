#include "../headers/report.h"

int create_report(char *output, int length)
{
    char log[length];

    strcpy(log, output);
    strcat(log, ".txt");

    int fd = creat(log, 0644);

    return fd;
}

ERROR write_compile_result(int fd, int status)
{
    bool warningAdded = false;

    int size;

    char buffer[BUFFER_SIZE];

    if (WEXITSTATUS(status))
    {
        if (write_all(fd, "Compilation Failed.\n\nError :\n\n") == -1)
        {
            perror("write failed");
            close(fd);
            return WRITE_FAIL;
        }

        while ((size = read(0, buffer, BUFFER_SIZE - 1)) > 0)
        {
            buffer[size] = '\0';

            if (write_all(fd, buffer) == WRITE_FAIL)
            {
                perror("write failed");
                close(fd);
                return WRITE_FAIL;
            }
        }

        return CONTINUE_FLAG;
    }

    if (write_all(fd, "Compiled Successfully.\n\n") == WRITE_FAIL)
    {
        perror("write failed");
        close(fd);
        return WRITE_FAIL;
    }

    while ((size = read(0, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[size] = '\0';

        if (!warningAdded)
        {
            if (write_all(fd, "Warnings:\n\n") == WRITE_FAIL)
            {
                perror("write failed");
                close(fd);
                return WRITE_FAIL;
            }
            warningAdded = true;
        }

        if (write_all(fd, buffer) == WRITE_FAIL)
        {
            perror("write failed");
            close(fd);
            return WRITE_FAIL;
        }
    }

    if (warningAdded)
    {
        if (write_all(fd, "\n\n") == WRITE_FAIL)
        {
            perror("write failed");
            close(fd);
            return WRITE_FAIL;
        }
    }

    return SUCCESS;
}

int write_execute_result(int fd, int *status, int elapsed, int pid)
{
    if (elapsed >= EXEC_TIMEOUT)
    {
        kill(pid, 9);

        waitpid(pid, status, 0);

        if (write_all(fd, "Program Timed out.\n") == WRITE_FAIL)
        {
            perror("write failed");
            return WRITE_FAIL;
        }
    }
    else if (!WIFEXITED(*status))
    {
        if (write_all(fd, "Program Crashed.\n") == WRITE_FAIL)
        {
            perror("write failed");
            return WRITE_FAIL;
        }
    }
    else
    {
        if (write_all(fd, "Return code: ") == WRITE_FAIL)
        {
            perror("write failed");
            return WRITE_FAIL;
        }

        int returnCode = WEXITSTATUS(*status);

        char code[4];

        sprintf(code, "%d", returnCode);

        if (write_all(fd, code) == WRITE_FAIL)
        {
            perror("write failed");
            return WRITE_FAIL;
        }

        if (write_all(fd, "\n\nOutput: \n\n") == WRITE_FAIL)
        {
            perror("write failed");
            return WRITE_FAIL;
        }

        int size;
        char buffer[BUFFER_SIZE];

        while ((size = read(0, buffer, BUFFER_SIZE - 1)) > 0)
        {
            buffer[size] = '\0';

            if (write_all(fd, buffer) == WRITE_FAIL)
            {
                perror("write failed");
                return WRITE_FAIL;
            }
        }
    }

    return SUCCESS;
}
