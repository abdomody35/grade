#ifndef WRITE_H
#define WRITE_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "error.h"

ERROR write_all(int fd, const char *buf);

#endif