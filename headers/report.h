#ifndef REPORT_H
#define REPORT_H

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "write.h"

#define BUFFER_SIZE 4096

extern int EXEC_TIMEOUT;

int create_report(char *output, int length);

int write_compile_result(int fd, int status);

int write_execute_result(int fd, int *status, int elapsed, int pid);

#endif