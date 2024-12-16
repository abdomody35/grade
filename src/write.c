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