#include "../headers/write.h"

ERROR write_all(int fd, const char *buf)
{
    int count = strlen(buf), written;

    while ((written = write(fd, buf, count)) > 0)
    {
        count -= written;
        buf += written;
    }

    if (written == -1)
    {
        perror("write failed");
        return WRITE_FAIL;
    }

    return SUCCESS;
}

ERROR write_to_report(int fd, int report, char *str1, char *str2)
{
    if (!str2)
    {
        str2 = str1;
    }

    ERROR error = write_all(fd, str1);

    if (error)
    {
        return error;
    }

    if (!report)
    {
        return SUCCESS;
    }

    return write_all(report, str2);
}

ERROR read_and_write(int source, int destination)
{
    int size;

    char buffer[BUFFER_SIZE];

    while ((size = read(source, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[size] = '\0';

        ERROR error = write_all(destination, buffer);

        if (error)
        {
            return error;
        }
    }

    return SUCCESS;
}