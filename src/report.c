#include "../headers/report.h"

int create_report(char *output, int length)
{
    char log[length];

    strcpy(log, output);
    strcat(log, ".txt");

    int fd = creat(log, 0644);

    if (fd == -1)
    {
        perror("open failed");
    }

    return fd;
}

int write_compile_result(int fd, int status)
{
    int warningAdded = 0, size;

    char buffer[BUFFER_SIZE];

    if (WEXITSTATUS(status))
    {
        if (write_all(fd, "Compilation Failed.\n\nError :\n\n") == -1)
        {
            perror("write failed");
            close(fd);
            return 7;
        }

        while ((size = read(0, buffer, BUFFER_SIZE - 1)) > 0)
        {
            buffer[size] = '\0';

            if (write_all(fd, buffer) == -1)
            {
                perror("write failed");
                close(fd);
                return 7;
            }
        }

        return -1;
    }

    if (write_all(fd, "Compiled Successfully.\n\n") == -1)
    {
        perror("write failed");
        close(fd);
        return 7;
    }

    while ((size = read(0, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[size] = '\0';

        if (!warningAdded)
        {
            if (write_all(fd, "Warnings:\n\n") == -1)
            {
                perror("write failed");
                close(fd);
                return 7;
            }
            warningAdded = 1;
        }

        if (write_all(fd, buffer) == -1)
        {
            perror("write failed");
            close(fd);
            return 7;
        }
    }

    if (warningAdded)
    {
        if (write_all(fd, "\n\n") == -1)
        {
            perror("write failed");
            close(fd);
            return 7;
        }
    }

    return 0;
}

int write_execute_result(int fd, int *status, int elapsed, int pid)
{
    if (elapsed >= EXEC_TIMEOUT)
    {
        kill(pid, 9);

        waitpid(pid, status, 0);

        if (write_all(fd, "Program Timed out.\n") == -1)
        {
            perror("write failed");
            return 7;
        }
    }
    else if (!WIFEXITED(*status))
    {
        if (write_all(fd, "Program Crashed.\n") == -1)
        {
            perror("write failed");
            return 7;
        }
    }
    else
    {
        if (write_all(fd, "Return code: ") == -1)
        {
            perror("write failed");
            return 7;
        }

        int returnCode = WEXITSTATUS(*status);

        char code[4];

        sprintf(code, "%d", returnCode);

        if (write_all(fd, code) == -1)
        {
            perror("write failed");
            return 7;
        }

        if (write_all(fd, "\n\nOutput: \n\n") == -1)
        {
            perror("write failed");
            return 7;
        }

        int size;
        char buffer[BUFFER_SIZE];

        while ((size = read(0, buffer, BUFFER_SIZE - 1)) > 0)
        {
            buffer[size] = '\0';

            if (write_all(fd, buffer) == -1)
            {
                perror("write failed");
                return 7;
            }
        }
    }

    return 0;
}
