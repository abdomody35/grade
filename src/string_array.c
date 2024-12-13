#include "../headers/string_array.h"

StrArray newStrArray(int initialLength)
{
    StrArray array;

    array.length = initialLength;

    array.elements = malloc(initialLength * sizeof(char *));

    return array;
}

int pushString(StrArray *array, const char *str)
{
    if (!array || !str)
        return -1;

    char **newElements = realloc(array->elements, (array->length + 1) * sizeof(char *));

    if (!newElements)
    {
        perror("realloc failed");
        return -1;
    }

    char *newString = duplicate_string(str);

    if (!newString)
    {
        perror("strdup failed");
        free(newElements);
        return -1;
    }

    array->elements = newElements;
    
    array->elements[array->length++] = newString;

    return 0;
}

int updateString(StrArray *array, const size_t index, const char *str)
{
    if (!array || index >= array->length || !str)
    {
        return -1;
    }

    free(array->elements[index]);

    array->elements[index] = duplicate_string(str);

    if (!array->elements[index])
    {
        perror("strdup failed");
        return -1;
    }

    return 0;
}

int nullTerminateArray(StrArray *array)
{
    if (!array)
    {
        return -1;
    }

    array->elements[array->length] = NULL;

    return 0;
}

void freeArray(StrArray *array)
{
    for (int i = 0; i < array->length; i++)
    {
        if (array->elements[i])
        {
            free(array->elements[i]);
        }
    }

    free(array->elements);

    array->length = 0;
}