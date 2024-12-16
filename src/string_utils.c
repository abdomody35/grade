#include "../headers/string_utils.h"

char *duplicate_string(char const *str)
{
    int len = strlen(str) + 1;

    char *new_string = (char *)malloc(len);

    if (!new_string)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(MEMORY_ALLOCATION_FAIL);
    }

    strcpy(new_string, str);

    return new_string;
}