#ifndef WRITE_H
#define WRITE_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "error.h"

#define BUFFER_SIZE 4096
#define DO_NOT_WRITE 0

ERROR write_all(int fd, const char *buf);

ERROR write_to_report(int fd, int report, char *str1, char *str2);

ERROR read_and_write(int source, int destination);

#endif