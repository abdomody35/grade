#ifndef EXECUTE_H
#define EXECUTE_H

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <unistd.h>
#include "string_array.h"

extern char *inputFile;
extern int EXEC_TIMEOUT;

int execute_program(int fds[], char *output, StrArray *args, int *pid);

int update_elapsed(int *elapsed, int *status, int pid);

#endif