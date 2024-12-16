#ifndef STR_ARRAY_H
#define STR_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "string_utils.h"

typedef struct
{
    char **elements;
    int length;
} StrArray;

StrArray *new_str_array(int initialLength);

ERROR push_string(StrArray *array, const char *str);

ERROR update_string(StrArray *array, const size_t index, const char *str);

ERROR null_terminate_array(StrArray *arary);

void free_array(StrArray *array);

#endif