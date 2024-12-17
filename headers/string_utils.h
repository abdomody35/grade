#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

char *duplicate_string(char const *str);

char *trim(char *str);

void to_lowercase(char *str);

#endif
