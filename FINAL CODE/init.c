/* init.c -- initial user mode image of P1 */
/****** Details from p. 321-322 of book ******/
#include "ucode.c"
//taken from embedded and rtos book
// int parent() // P1's code {
//   int pid, status;
//   while(1){
// printf("INIT : wait for ZOMBIE child\n");
// pid = wait(&status);
// if (pid==console){ // if console login process died
// printf("INIT: forks a new console login\n"); console = fork(); // fork another one
// if (console)
//            continue;
//        else
// exec("login /dev/tty0"); // new console login process
// }
// printf("INIT: I just buried an orphan child proc %d\n", pid); }
// }
// main()
// {
 
// 322 8
// General Purpose Embedded Operating Systems
//  int in, out; // file descriptors for terminal I/O
// in = open("/dev/tty0", O_RDONLY); // file descriptor 0
// out = open("/dev/tty0", O_WRONLY); // for display to console printf("INIT : fork a login proc on console\n");
// console = fork();
// if (console) // parent
// parent();
// else // child: exec to login on tty0
//       exec("login /dev/tty0");
// }


int console;
int serial0, serial1;

int parent()
{
    int pid, status;
    while (1)
    {
        printf("init.c -- wait for ZOMBIE child\n");
        pid = wait(&status);

        // if console login process died
        if (pid == console)
        {
            printf("init.c -- forks a new console login\n");
            console = fork();
            if (console)
                continue;
            else
                exec("login /dev/tty0"); // new console login process
        }

        if (pid == serial0) //multi user login thank u kc for providing outline
        {
            serial0 = fork();
            if (serial0)
            {
                continue;
            }
            else
            {
                exec("login /dev/ttyS0");
            }
        }
        if (pid == serial1)
        {
            serial1 = fork();
            if (serial1)
            {
                continue;
            }
            else
            {
                exec("login /dev/ttyS1");
            }
        }
        printf("init.c -- P1: I just buried an orphan %d\n", pid);
    }
}

main(int argc, char *argv[])
{
    int in, out;                       // file descriptors for terminal I/O
    in = open("/dev/tty0", O_RDONLY);  // file descriptor 0
    out = open("/dev/tty0", O_WRONLY); // for display to console
    printf("proc %d in Umode ", getpid());
    printf("argc: %d argv[0]: %s argv[1]: %s\n", argc, argv[0], argv[1]);
    printf("init.c -- fork a login PROC on console\n");
    console = fork();

    // if console is non-zero, then it's the parent PROC
    if (console)
    {
        serial0 = fork();
        if (serial0)
        {
            serial1 = fork();
            if (serial1)
            {
                parent();
            }
            else
            {
                exec("login /dev/ttyS1");
            }
        }
        else
        {
            exec("login /dev/ttyS0");
        }
    }
    // otherwise, it's the child PROC; login on tty0
    else
    {
        exec("login /dev/tty0");
    }
}
