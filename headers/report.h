#ifndef REPORT_H
#define REPORT_H

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include "error.h"
#include "write.h"

#define BUFFER_SIZE 4096

extern int EXEC_TIMEOUT;

ERROR create_report(char *output, int length);

ERROR write_compile_result(int fd, int status);

ERROR write_execute_result(int fd, int *status, int elapsed, int pid);

#endif