#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <fcntl.h>
#include <unistd.h>
#include "error.h"
#include "string_array.h"

#define BUFFER_SIZE 4096
#define MAX_ARG_COUNT 12

extern char *program;
extern char *inputFile;
extern char *argFile;
extern char *reportFile;
extern char *outputFile;
extern int EXEC_TIMEOUT;

ERROR parse_arguments(int argc, char **argv, StrArray *args);

void print_usage();

ERROR update_config(const char *flag, const char *arg);

ERROR update_arguments(StrArray *args);

#endif