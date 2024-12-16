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

StrArray *new_str_array(int initialLength);

int push_string(StrArray *array, const char *str);

int update_string(StrArray *array, const size_t index, const char *str);

int null_terminate_array(StrArray *arary);

void free_array(StrArray *array);

#endif