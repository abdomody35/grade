#ifndef EXECUTE_H
#define EXECUTE_H

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <unistd.h>
#include "error.h"
#include "string_array.h"

extern char *inputFile;
extern int EXEC_TIMEOUT;

ERROR execute_program(int fds[], char *output, StrArray *args, int *pid);

ERROR update_elapsed(int *elapsed, int *status, int pid);

#endif