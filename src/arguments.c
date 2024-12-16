#include "../headers/arguments.h"

char *program = NULL;
char *inputFile = NULL;
char *argFile = NULL;
int EXEC_TIMEOUT = 5;

int handle_arguments(int argc, char **argv, StrArray *args)
{
    program = argv[0];

    if (!(argc == 1 || (argc % 2 && argc < MAX_ARG_COUNT)))
    {
        print_usage();
        return 1;
    }

    for (int i = 1; i < argc; i += 2)
    {
        if (update_config(argv[i], argv[i + 1]))
        {
            print_usage();
            return 5;
        }
    }

    if (!args->elements)
    {
        perror("malloc failed");
        return 11;
    }

    if (argFile)
    {
        int error = update_arguments(args);

        if (error)
        {
            return error;
        }
    }

    if (null_terminate_array(args) == -1)
    {
        perror("null_terminate_array failed");
        return 13;
    }

    return 0;
}

void print_usage()
{
    fprintf(stderr, "Usage: %s [-i inputsfile] [-a argumetnsfile] [-t timeout].\nNote: time out is in seconds (Default 5).\n", program);
}

int update_config(const char *flag, const char *arg)
{
    if (!arg)
    {
        fprintf(stderr, "An argument must be specified after the flag.\n");
        return 8;
    }

    if (!strcmp(flag, "-i"))
    {
        inputFile = (char *)arg;
        return 0;
    }

    if (!strcmp(flag, "-a"))
    {
        argFile = (char *)arg;
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

int update_arguments(StrArray *args)
{
    int arguments = open(argFile, O_RDONLY);

    if (arguments == -1)
    {
        perror("open failed");
        return 6;
    }

    char buffer[BUFFER_SIZE];

    int size;

    while ((size = read(arguments, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[size] = '\0';

        char *token = strtok(buffer, " \n");

        while (token)
        {
            if (push_string(args, token) == -1)
            {
                perror("pushString failed");
                return 12;
            }

            token = strtok(NULL, " \n");
        }
    }

    close(arguments);

    return 0;
}