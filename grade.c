#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>
#include "headers/write.h"

#define BUFFER_SIZE 4096

int EXEC_TIMEOUT = 5;

char *program, *inputFile, *argFile;

void print_usage()
{
    fprintf(stderr, "Usage: %s [-i inputsfile] [-a argumetnsfile] [-t timeout].\nNote: time out is in seconds (Default 5).\n", program);
}

int update_Config(const char *flag, const char *arg)
{
    if (!arg)
    {
        fprintf(stderr, "An argument must be specified after the flag.\n");
        return 8;
    }

    if (!strcmp(flag, "-i"))
    {
        inputFile = arg;
        return 0;
    }

    if (!strcmp(flag, "-a"))
    {
        argFile = arg;
        return 0;
    }

    if (!strcmp(flag, "-t"))
    {
        EXEC_TIMEOUT = atoi(arg);
        return 0;
    }

    fprintf(stderr, "Invalid flag. You can only use the following :\n"
                    "\t-i : the name of the file containing inputs to be passed to the programs\n"
                    "\t-a : the name of the file containing arguments to be passed to the programs\n"
                    "\t-t : specifying the maximum number of seconds a program can take to run. Default is 5 seconds.\n\n");
    return 5;
}

char *duplicate_string(char const *str)
{
    int len = strlen(str) + 1;

    char *new_string = (char *)malloc(len);

    if (!new_string)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(11);
    }

    strcpy(new_string, str);

    return new_string;
}

int main(int argc, char **argv)
{
    program = argv[0];

    if (!(argc == 1 || (argc % 2 && argc < 8)))
    {
        print_usage();
        return 1;
    }

    for (int i = 1; i < argc; i += 2)
    {
        if (update_Config(argv[i], argv[i + 1]))
        {
            print_usage();
            return 5;
        }
    }

    int fds[2], size, pid, argCount = 1;

    char buffer[BUFFER_SIZE], **args = (char **)malloc(sizeof(char *));

    if (argFile)
    {
        int arguments = open(argFile, O_RDONLY);

        if (arguments == -1)
        {
            perror("open failed");
            return 6;
        }

        while ((size = read(arguments, buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[size] = '\0';

            char *token = strtok(buffer, " \n");

            while (token)
            {
                args = (char **)realloc(args, sizeof(char *) * (argCount + 1));

                if (!args)
                {
                    perror("Memory allocation failed");
                    close(arguments);
                    return 11;
                }

                args[argCount] = duplicate_string(token);

                if (!args[argCount])
                {
                    perror("Memory allocation failed");
                    close(arguments);
                    return 11;
                }

                argCount++;

                token = strtok(NULL, " \n");
            }
        }

        close(arguments);
    }

    args[argCount] = NULL;

    if (pipe(fds) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    pid = fork();

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

    if (dup2(fds[0], 0) == -1)
    {
        perror("dup2 failed");
        close(fds[0]);
        return 3;
    }

    while (1)
    {
        char filename[255] = {};

        scanf("%s", filename);

        int length = strlen(filename);

        if (!length)
        {
            break;
        }

        if (!(filename[length - 2] == '.' && filename[length - 1] == 'c'))
        {
            continue;
        }

        printf("grading %s...\n", filename);

        char output[length];

        strcpy(output, filename);
        output[length - 2] = '\0';

        if (strstr(argv[0], output))
        {
            continue;
        }

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

        if (dup2(fds[0], 0) == -1)
        {
            perror("dup2 failed");
            close(fds[0]);
            return 3;
        }

        int status;
        wait(&status);

        char log[length];

        strcpy(log, output);
        strcat(log, ".txt");

        int fd = creat(log, 0644);

        if (fd == -1)
        {
            perror("open failed");
            return 6;
        }

        if (WIFEXITED(status))
        {
            int warningAdded = 0;

            if (WEXITSTATUS(status))
            {
                if (write_all(fd, "Compilation Failed.\n\nError :\n\n") == -1)
                {
                    perror("write failed");
                    close(fd);
                    return 7;
                }

                while ((size = read(0, buffer, sizeof(buffer - 1))) > 0)
                {
                    buffer[size] = '\0';

                    if (write_all(fd, buffer) == -1)
                    {
                        perror("write failed");
                        close(fd);
                        return 7;
                    }
                }

                continue;
            }

            if (write_all(fd, "Compiled Successfully.\n\n") == -1)
            {
                perror("write failed");
                close(fd);
                return 7;
            }

            while ((size = read(0, buffer, sizeof(buffer - 1))) > 0)
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
        }

        if (pipe(fds) == -1)
        {
            perror("pipe failed");
            return 1;
        }

        pid = fork();

        if (pid == -1)
        {
            perror("fork failed");
            close(fd);
            return 2;
        }

        if (!pid)
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

            close(fd);

            if (inputFile)
            {
                int input = open(inputFile, O_RDONLY);

                if (input == -1)
                {
                    perror("open failed");
                    close(fd);
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

            args[0] = duplicate_string(buffer);

            execv(output, args);

            perror("execl failed");
            close(fd);
            return 8;
        }

        close(fds[1]);

        if (dup2(fds[0], 0) == -1)
        {
            perror("dup2 failed");
            close(fds[0]);
            return 3;
        }

        int elapsed = 0;

        while (elapsed < EXEC_TIMEOUT)
        {
            int result = waitpid(pid, &status, WNOHANG);
            if (result == -1)
            {
                perror("waitpid failed");
                close(fd);
                return 7;
            }
            else if (result == 0)
            {
                sleep(1);
                elapsed++;
                continue;
            }
            else
            {
                break;
            }
        }

        if (elapsed >= EXEC_TIMEOUT)
        {
            kill(pid, 9);

            waitpid(pid, &status, 0);

            if (write_all(fd, "Program Timed out.\n") == -1)
            {
                perror("write failed");
                close(fd);
                return 7;
            }
        }
        else if (!WIFEXITED(status))
        {
            if (write_all(fd, "Program Crashed.\n") == -1)
            {
                perror("write failed");
                close(fd);
                return 7;
            }
        }
        else
        {
            if (write_all(fd, "Return code: ") == -1)
            {
                perror("write failed");
                close(fd);
                return 7;
            }

            int returnCode = WEXITSTATUS(status);

            char code[4];

            sprintf(code, "%d", returnCode);

            if (write_all(fd, code) == -1)
            {
                perror("write failed");
                close(fd);
                return 7;
            }

            if (write_all(fd, "\n\nOutput: \n\n") == -1)
            {
                perror("write failed");
                close(fd);
                return 7;
            }

            while ((size = read(0, buffer, sizeof(buffer) - 1)) > 0)
            {
                buffer[size] = '\0';

                if (write_all(fd, buffer) == -1)
                {
                    perror("write failed");
                    close(fd);
                    return 7;
                }
            }
        }

        close(fds[0]);
        close(fd);
    }

    close(fds[0]);
    return 0;
}