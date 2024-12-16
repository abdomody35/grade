#include "../headers/arguments.h"

char *program = NULL;
char *inputFile = NULL;
char *argFile = NULL;
char *reportFile = NULL;
int EXEC_TIMEOUT = 5;

ERROR parse_arguments(int argc, char **argv, StrArray *args)
{
    program = argv[0];

    ERROR error;

    if (!(argc == 1 || (argc % 2 && argc < MAX_ARG_COUNT)))
    {
        print_usage();
        return INVALID_ARGUMENT_COUNT;
    }

    for (int i = 1; i < argc; i += 2)
    {
        error = update_config(argv[i], argv[i + 1]);

        if (error)
        {
            print_usage();
            return error;
        }
    }

    if (!args->elements)
    {
        perror("malloc failed");
        return MEMORY_ALLOCATION_FAIL;
    }

    if (argFile)
    {
        error = update_arguments(args);

        if (error)
        {
            return error;
        }
    }

    error = null_terminate_array(args);

    if (error)
    {
        return error;
    }

    return SUCCESS;
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
        return MISSING_ARGUMENT;
    }

    if (!strcmp(flag, "-i"))
    {
        inputFile = (char *)arg;
        return SUCCESS;
    }

    if (!strcmp(flag, "-a"))
    {
        argFile = (char *)arg;
        return SUCCESS;
    }

    if (!strcmp(flag, "-t"))
    {
        EXEC_TIMEOUT = atoi(arg);
        return SUCCESS;
    }

    if (!strcmp(flag, "-r"))
    {
        reportFile = (char *)arg;
        return SUCCESS;
    }

    fprintf(stderr, "Invalid flag. You can only use the following :\n"
                    "\t-i : the name of the file containing inputs to be passed to the programs\n"
                    "\t-a : the name of the file containing arguments to be passed to the programs\n"
                    "\t-t : specifying the maximum number of seconds a program can take to run. Default is 5 seconds.\n\n");

    return INVALID_FLAG;
}

int update_arguments(StrArray *args)
{
    int arguments = open(argFile, O_RDONLY);

    if (arguments == -1)
    {
        perror("open failed");
        return OPEN_FAIL;
    }

    char buffer[BUFFER_SIZE];

    int size;

    while ((size = read(arguments, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[size] = '\0';

        char *token = strtok(buffer, " \n");

        while (token)
        {
            ERROR error = push_string(args, token);

            if (error)
            {
                return error;
            }

            token = strtok(NULL, " \n");
        }
    }

    close(arguments);

    return SUCCESS;
}