#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int argc_num;
    int xargc_argc;
    char buffer[512];
    char *xargv_argv[MAXARG];
    int start;
    int end;
    char arg_cur;

    // check argc
    if (argc < 2)
    {
        fprintf(2, "Usage: xarg <cmd>\n");
        exit(1);
    }
    else if (argc > MAXARG)
    {
        fprintf(2, "xargs: too many arguments\n");
        exit(1);
    }

    argc_num = argc - 1;

    for (int i = 0; i < argc_num; i++)
    {
        xargv_argv[i] = argv[i + 1];
    }

    // check -n argument
    if (xargv_argv[0][0] == '-')
    {
        argc_num = argc - 3;
        for (int i = 0; i < argc_num; i++)
        {
            xargv_argv[i] = xargv_argv[i + 2];
            xargv_argv[i + 1] = 0;
            xargv_argv[i + 2] = 0;
        }
    }

    xargc_argc = argc_num;

    start = end = 0;

    // read arguments
    while (read(0, &arg_cur, sizeof(char)))
    {
        switch (arg_cur)
        {
        case ' ':
        {
            buffer[end] = '\0';
            xargv_argv[xargc_argc++] = &buffer[start];
            end++;
            start = end;
            break;
        }
        case '\n':
        {
            buffer[end] = '\0';
            xargv_argv[xargc_argc++] = &buffer[start];

            // execute command
            if (fork() == 0)
            {
                exec(xargv_argv[0], xargv_argv);
            }
            else
            {
                wait(0);
            }
            xargc_argc = argc_num;
            end++;
            start = end;
            break;
        }
        default:
        {
            buffer[end++] = arg_cur;
            break;
        }
        }
    }

    wait(0);
    exit(0);
}