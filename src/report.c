#include "../headers/report.h"

int create_report(char *output, int length)
{
    char log[length];

    snprintf(log, length + 4, "%s%s", output, strchr(output, '.') ? "" : ".txt");

    int fd = creat(log, 0644);

    return fd;
}

ERROR initialize_report_field(int report, char *output)
{
    if (!report)
    {
        return SUCCESS;
    }

    char str[IDENTIFIER_WIDTH + 4];

    snprintf(str, sizeof(str), "%-*s : ", IDENTIFIER_WIDTH, output);

    return write_all(report, str);
}

ERROR write_compile_result(int fd, int status, int report)
{
    bool warnings = false;

    if (WEXITSTATUS(status))
    {
        ERROR error = write_to_report(fd, report, "Compilation Failed.\n\nError :\n\n", "Compilation Failed\n");

        if (error)
        {
            return error;
        }

        error = read_and_write(STDIN_FILENO, fd);

        if (error)
        {
            return error;
        }

        return CONTINUE_FLAG;
    }

    ERROR error = write_all(fd, "Compiled Successfully.\n\n");

    if (error)
    {
        return error;
    }

    int size;

    char buffer[BUFFER_SIZE];

    while ((size = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[size] = '\0';

        if (!warnings)
        {
            error = write_all(fd, "Warnings:\n\n");

            if (error)
            {
                return error;
            }

            warnings = true;
        }

        error = write_all(fd, buffer);

        if (error)
        {
            return error;
        }
    }

    if (warnings)
    {
        error = write_all(fd, "\n\n");

        if (error)
        {
            return error;
        }
    }

    if (report)
    {
        char str[COMPILATION_WIDTH + 1];

        snprintf(str, sizeof(str), "%-*s", COMPILATION_WIDTH, (warnings ? "Compiled With Warnings" : "Compiled With No Issues"));

        error = write_all(report, str);

        if (error)
        {
            return error;
        }
    }

    return SUCCESS;
}

ERROR write_execute_result(int fd, int *status, int elapsed, int pid, int report)
{
    if (elapsed >= EXEC_TIMEOUT)
    {
        kill(pid, SIG_KILL);

        waitpid(pid, status, 0);

        return write_timeout(fd, report);
    }

    if (!WIFEXITED(*status))
    {
        return write_crash(fd, report);
    }

    ERROR error = write_return_code(fd, report, status);

    if (error)
    {
        return error;
    }

    char buffer[BUFFER_SIZE] = {};
    char actualOutput[BUFFER_SIZE] = {};

    read_output(buffer, actualOutput);

    error = write_output_result(fd, report, actualOutput);

    if (error)
    {
        return error;
    }

    error = write_output(fd, buffer);

    if (error)
    {
        return error;
    }

    return SUCCESS;
}

ERROR write_timeout(int fd, int report)
{
    return write_to_report(fd, report, "Program Timed out.\n", " | Program Timed out\n");
}

ERROR write_crash(int fd, int report)
{
    return write_to_report(fd, report, "Program Crashed.\n", " | Program Crashed\n");
}

ERROR write_return_code(int fd, int report, int *status)
{
    int returnCode = WEXITSTATUS(*status);

    char str1[RETURN_WIDTH + RETURN_MESSAGE_WIDTH], str2[RETURN_WIDTH + RETURN_MESSAGE_WIDTH];

    snprintf(str1, sizeof(str1), "Return code: %d", returnCode);

    snprintf(str2, sizeof(str2), " | Returned %-*d", RETURN_WIDTH, returnCode);

    return write_to_report(fd, report, str1, str2);
}

void read_output(char *buffer, char *actualOutput)
{
    int size, actualOutputSize = 0;

    while ((size = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1)) > 0)
    {
        buffer[size] = '\0';

        strncpy(actualOutput + actualOutputSize, buffer, size);

        actualOutputSize += size;
    }

    actualOutput[actualOutputSize] = '\0';
}

ERROR write_output_result(int fd, int report, char *actualOutput)
{
    if (!outputFile)
    {
        return write_to_report(fd, report, "", "\n");
    }

    char expectedOutput[BUFFER_SIZE];
    int expectedOutputSize = 0;

    int expectedFd = open(outputFile, O_RDONLY);

    if (expectedFd == -1)
    {
        perror("open failed");
        return OPEN_FAIL;
    }

    int size;

    while ((size = read(expectedFd, expectedOutput + expectedOutputSize, BUFFER_SIZE - 1 - expectedOutputSize)) > 0)
    {
        expectedOutputSize += size;
    }

    expectedOutput[expectedOutputSize] = '\0';

    close(expectedFd);

    char *cleanActualOutput = trim(actualOutput);
    to_lowercase(cleanActualOutput);

    char *cleanExpectedOutput = trim(expectedOutput);
    to_lowercase(cleanExpectedOutput);

    bool outputMatches = !strcmp(cleanActualOutput, cleanExpectedOutput);

    return write_to_report(fd, report,
                           outputMatches ? "\n\nOutput Matches The Expected Output." : "\n\nOutput Doesn't Match The Expected Output.",
                           outputMatches ? " | Correct Output\n" : " | Wrong Output\n");
}

ERROR write_output(int fd, char *buffer)
{
    ERROR error = write_all(fd, "\n\nOutput: \n\n");

    if (error)
    {
        return error;
    }

    error = write_all(fd, buffer);

    if (error)
    {
        return error;
    }

    return SUCCESS;
}
