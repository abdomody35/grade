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

char *trim(char *str)
{
    char *end;

    while (isspace((unsigned char)*str))
    {
        str++;
    }

    end = str + strlen(str) - 1;

    while (end > str && isspace((unsigned char)*end))
    {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}

void to_lowercase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower((unsigned char)str[i]);
    }
}