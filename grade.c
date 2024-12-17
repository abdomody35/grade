#include "headers/arguments.h"
#include "headers/compile.h"
#include "headers/error.h"
#include "headers/execute.h"
#include "headers/files.h"
#include "headers/report.h"
#include "headers/string_array.h"
#include "headers/write.h"

int main(int argc, char **argv)
{
    StrArray *args = new_str_array(1);

    int fds[2];

    ERROR error;

    error = parse_arguments(argc, argv, args);

    if (error)
    {
        free_array(args);
        return error;
    }

    error = get_files(fds);

    if (error)
    {
        free_array(args);
        return error;
    }

    int report = DO_NOT_WRITE;

    if (reportFile)
    {
        report = create_report(reportFile, strlen(reportFile));

        if (report == -1)
        {
            perror("creat failed");
            free_array(args);
            return CREAT_FAIL;
        }
    }

    if (dup2(fds[0], STDIN_FILENO) == -1)
    {
        perror("dup2 failed");
        free_array(args);
        close(fds[0]);
        return DUP2_FAIL;
    }

    while (1)
    {
        char filename[255] = {};

        scanf("%s", filename);

        int length = strlen(filename);

        if (!length)
        {
            break;
        }

        if (!(filename[length - 2] == '.' && filename[length - 1] == 'c'))
        {
            continue;
        }

        char output[length];

        strcpy(output, filename);
        output[length - 2] = '\0';

        if (strstr(program, output))
        {
            continue;
        }

        error = initialize_report_field(report, output);

        if (error)
        {
            free_array(args);
            return error;
        }

        printf("grading %s...\n", filename);

        int status;

        error = compile_file(filename, output, fds, &status);

        if (error)
        {
            free_array(args);
            return error;
        }

        if (dup2(fds[0], STDIN_FILENO) == -1)
        {
            perror("dup2 failed");
            close(fds[0]);
            free_array(args);
            return DUP2_FAIL;
        }

        int fd = create_report(output, length);

        if (fd == -1)
        {
            perror("creat failed");
            free_array(args);
            return CREAT_FAIL;
        }

        if (WIFEXITED(status))
        {
            error = write_compile_result(fd, status, report);

            if (error)
            {
                if (error == CONTINUE_FLAG)
                {
                    continue;
                }

                free_array(args);
                return error;
            }
        }

        int pid;

        error = execute_program(fds, output, args, &pid);

        if (error)
        {
            close(fd);
            free_array(args);
            return error;
        }

        if (dup2(fds[0], STDIN_FILENO) == -1)
        {
            perror("dup2 failed");
            close(fds[0]);
            free_array(args);
            return DUP2_FAIL;
        }

        int elapsed = 0;

        error = update_elapsed(&elapsed, &status, pid);

        if (error)
        {
            close(fd);
            free_array(args);
            return error;
        }

        error = write_execute_result(fd, &status, elapsed, pid, report);

        if (error)
        {
            close(fd);
            free_array(args);
            return error;
        }

        close(fds[0]);
        close(fd);
    }

    free_array(args);
    close(fds[0]);
    return 0;
}