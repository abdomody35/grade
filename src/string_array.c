#include "../headers/string_array.h"

StrArray *new_str_array(int initialLength)
{
    StrArray *array = malloc(sizeof(StrArray));

    if (!array)
    {
        perror("malloc failed");
        return NULL;
    }

    array->length = initialLength;

    array->elements = malloc(initialLength * sizeof(char *));

    if (!array->elements)
    {
        perror("malloc failed");
        free(array);
        return NULL;
    }

    for (int i = 0; i < initialLength; i++)
    {
        array->elements[i] = NULL;
    }

    return array;
}

ERROR push_string(StrArray *array, const char *str)
{
    if (!array || !str)
        return NULL_POINTER_FAIL;

    char **newElements = realloc(array->elements, (array->length + 1) * sizeof(char *));

    if (!newElements)
    {
        perror("realloc failed");
        return MEMORY_ALLOCATION_FAIL;
    }

    array->elements = newElements;

    char *newString = duplicate_string(str);

    if (!newString)
    {
        perror("strdup failed");
        return STRING_DUPLICATE_FAIL;
    }

    array->elements[array->length++] = newString;

    return SUCCESS;
}

ERROR update_string(StrArray *array, const size_t index, const char *str)
{
    if (!array || index >= array->length || !str)
        return NULL_POINTER_FAIL;

    free(array->elements[index]);

    array->elements[index] = duplicate_string(str);

    if (!array->elements[index])
    {
        perror("strdup failed");
        return STRING_DUPLICATE_FAIL;
    }

    return SUCCESS;
}

int null_terminate_array(StrArray *array)
{
    if (!array)
        return NULL_POINTER_FAIL;

    char **newElements = realloc(array->elements, (array->length + 1) * sizeof(char *));

    if (!newElements)
    {
        perror("realloc failed");
        return MEMORY_ALLOCATION_FAIL;
    }

    array->elements = newElements;

    array->elements[array->length] = NULL;

    return SUCCESS;
}

void free_array(StrArray *array)
{
    if (!array)
        return;

    for (int i = 0; i < array->length; i++)
    {
        free(array->elements[i]);
    }

    free(array->elements);

    free(array);
}
