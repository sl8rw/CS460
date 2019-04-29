#include "ucode.c"

/*
 * Set string filename to op if an IO redirect exists in cmdline.
 * Return IO_REDIRECT code.
 * Returns ERROR_CODE if no IO redirects.
 */
int get_io_redirect_code(char *cmdline, char *filename)
{
    int i = 0;
    char cpy[64];
    strcpy(cpy, cmdline);

    while (cmdline[i])
    {
        if (cmdline[i] == '>')
        {
            if (cmdline[i + 1] == '>')
            {
                // cmd >> filename
                //     i
                //      +1
                //        +3
                strcpy(filename, &cmdline[i + 3]);

                // ensure rest of cmd line ignored
                cmdline[i - 1] = 0;
                return APPEND;
            }
            else
            {
                // cmd > filename
                //     i
                //       +2
                strcpy(filename, &cmdline[i + 2]);

                // ensure rest of cmd line ignored
                cmdline[i - 1] = 0;
                return WRITE;
            }
        }
        else if (cmdline[i] == '<')
        {
            // cmd < filename
            //     i
            //       +2
            strcpy(filename, &cmdline[i + 2]);

            // ensure rest of cmd line ignored
            cmdline[i - 1] = 0;

            // remove '<'; shift string over by 2
            // strcpy(&cmdline[i], filename);
            return READ;
        }
        ++i;
    }
    return ERROR_CODE;
}

void io_redirect(int code, char *filename)
{
    switch (code)
    {
    case WRITE:
        close(STDOUT);
        open(filename, O_WRONLY | O_CREAT | O_TRUNC);
        break;
    case READ:
        close(STDIN);
        open(filename, O_RDONLY);
        break;
    case APPEND:
        close(STDOUT);
        open(filename, O_WRONLY | O_APPEND);
        break;
    case ERROR_CODE:
        break;
    }
}

int pipe_exists(char *cmdline)
{
    int i = 0;
    while (cmdline[i] != '\0')
    {
        if (cmdline[i] == '|')
        {
            return 1;
        }
        i++;
    }
    return 0;
}

// Does IO redirect if it exists; o.w. nothing
do_io_redirect(char *cmdline)
{
    char filename[64];
    int io_redirect_code = get_io_redirect_code(cmdline, filename);

    if (io_redirect_code != ERROR_CODE)
    {
        /* Handle the IO redirection. */
        //printf("filename for ioredir: |%s|\n", filename);
        io_redirect(io_redirect_code, filename);
    }
}

do_linux_pipe(char *cmdline)
{
    char head[64], tail[64];
    int i = 0, pid, status, pd[2];
    strcpy(head, cmdline);
    while (cmdline[i] != '\0')
    {
        if (cmdline[i] == '|')
        {
            // cmd1 | cmd2
            //      i
            //    i-1 is space before pipe
            //        i+2 is start of cmd2
            // from cmd[0] to cmd[i-1] will be our pipe reader / parent
            // from cmd[i+2] onwards will be pipe writer / child
            head[i - 1] = 0;

            strcpy(tail, &cmdline[i + 2]);
            break;
        }
        i++;
    }
    // pd[0] reader, pd[1] writer
    // if (pipe(pd) == 0) printf("creating pipe! :-)\n");
    //printf("head: |%s|\n", head);
    //printf("tail: |%s|\n", tail);
    pipe(pd);

    // fork proc to handle pipe
    pid = fork();

    // parent execute tail as ==> pipe reader
    if (pid)
    {
        //pid = wait(&status);
        close(STDIN);       // close stdin
        dup2(pd[0], STDIN); // replace stdin with pd[0] -- pipe reader
        close(pd[1]);       // reader must close pipe writer

        if (pipe_exists(tail))
        {
            do_linux_pipe(tail);
        }
        else
        {
            do_io_redirect(tail);
            exec(tail);
            exit(0);
        }
    }
    // child run head as ==> pipe writer
    else
    {
        close(STDOUT);       // close stdout
        dup2(pd[1], STDOUT); // replace stdout with pd[1] -- pipe writer
        close(pd[0]);        // writer must close pipe reader

        do_io_redirect(head);
        exec(head);
        exit(0);
    }

    /* Before exiting, make sure to reset fd[] to stdin & stdout. ORDER MATTERS! */
    //    close(pd[0]);

    //    close(pd[1]);
}

main(int argc, char *argv[])
{
    int pid, status, pd[2];
    char cmdline[64];
    int pipe_detected = 0;
    char head[64], tail[64];
    int wait_flag;

    while (1)
    {
        // TODO > Display executable commands of /bin/ directory

        // Prompt for a command line cmdline = "cmd arg1 arg2 ... argn"
        printf("sh %d# ", getpid());
        gets(cmdline);

        // extract cmd from cmdline
        char cmd[64];
        strcpy(cmd, cmdline);

        int i = 0;
        while (cmd[i] != '\0' && cmd[i] != ' ')
        {
            if (cmd[i] == ' ')
            {
                cmd[i] = 0;
                break;
            }
            i++;
        }

        if (strcmp(cmd, "exit") == 0)
            exit(0);
        if (strcmp(cmd, "logout") == 0)
        {
            exit(0);
        }
        // if(strcmp(cmd,"cd")==0)
        // {
        //     // chdir("/");
        //     // print2f("changed to root\n\r");
        //     // print2f(cmd[0]);
        //     // print2f(cmd[1]);
        //     if(cmd[1]==0)
        //     {
        //         print2f("in if\n");
        //         chdir("/");
        //     }
        //     else
        //     {
        //         chdir(cmd[1]);
        //     }
        //     continue;

        // }
        if (strcmp(cmd, "pwd") == 0)
        {
            pwd();
            continue;
        }

        // general sh on p. 228 of Real-Time OS System
        if (cmdline[0] == 0 || cmdline[0] == "\r" || cmdline[0] == "\n")
        {
            continue;
        }
        pid = fork(); //forking a child

        // parent sh waits for child to die
        if (pid)
        {

            pid = wait(&status);
            printf("sh.c -- child proc %d died -- exit code = %d\n", pid, status);
            //continue;
        }
        // child exec command line
        else
        {
            // Detect if pipe is present in cmdline
            // if so, run pipe code
            // else exec normally
            printf("sh.c -- %d forked me; I am child %d\n", getppid(), getpid());

            // HANDLE PIPES :-)
            if (pipe_exists(cmdline))
            {
                do_linux_pipe(cmdline);
            }
            // NO PIPES :-)
            else
            {

                do_io_redirect(cmdline);
                exec(cmdline);
                exit(0);
            }
        }
    }
}
