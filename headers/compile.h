#ifndef COMPILE_H
#define COMPILE_H

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int compile_file(char *filename, char *output, int fds[], int *status);

#endif