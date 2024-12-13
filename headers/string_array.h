#ifndef STR_ARRAY_H
#define STR_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "string.h"

typedef struct
{
    char **elements;
    int length;
} StrArray;

StrArray *newStrArray(int initialLength);

int pushString(StrArray *array, const char *str);

int updateString(StrArray *array, const size_t index, const char *str);

int nullTerminateArray(StrArray *arary);

void freeArray(StrArray *array);

#endif