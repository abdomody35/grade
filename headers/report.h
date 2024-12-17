#ifndef REPORT_H
#define REPORT_H

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include "arguments.h"
#include "error.h"
#include "write.h"

#define BUFFER_SIZE 4096
#define IDENTIFIER_WIDTH 10
#define COMPILATION_WIDTH 24
#define RETURN_WIDTH 3
#define RETURN_MESSAGE_WIDTH 14
#define SIG_KILL 9

extern int EXEC_TIMEOUT;

ERROR create_report(char *output, int length);

ERROR initialize_report_field(int report, char *output);

ERROR write_compile_result(int fd, int status, int report);

ERROR write_execute_result(int fd, int *status, int elapsed, int pid, int report);

ERROR write_timeout(int fd, int report);

ERROR write_crash(int fd, int report);

ERROR write_return_code(int fd, int report, int *status);

void read_output(char *buffer, char *actualOutput);

ERROR write_output_result(int fd, int report, char *actualOutput);

ERROR write_output(int fd, char *buffer);

#endif