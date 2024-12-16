#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <fcntl.h>
#include <unistd.h>
#include "string_array.h"

#define BUFFER_SIZE 4096
#define MAX_ARG_COUNT 8

extern char *program;
extern char *inputFile;
extern char *argFile;
extern int EXEC_TIMEOUT;

int handle_arguments(int argc, char **argv, StrArray *args);

void print_usage();

int update_config(const char *flag, const char *arg);

int update_arguments(StrArray *args);

#endif