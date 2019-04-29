#include "ucode.c"

//more usage is more filename with filename optional, if no filename us stdin

char buf[1024], uline[2048];
char line2;
char tty[64];
char temp;
int main(int argc, char *argv[])
{
    int fd = 0;
    int gd;
    int i, n;
    char ret = '\r';
    char nline = '\n';
    char w;

    line2 = 0;
    gettty(tty);
    gd = open(tty, 0);

    if (argv[1])
    {
        //stdin
        fd = open(argv[1], 0); //reading from stdin
        if (fd < 0)
        {
            printf("ERROR: issue with stdin\n");
            exit(1);
        }
    }
    if (fd)
    {
        n = read(fd, buf, 1024);
        while (n > 0)
        {
            for (i = 0; i < n; i++)
            {
                temp = buf[i];
                write(1, &temp, 1);
                if (temp == '\n')
                {
                    line2++;
                    write(2, &ret, 1);
                }
                if (line2 > 25)
                {
                    //more than 25 lines read
                    read(gd, &w, 1);
                    if (w == ret || w == nline)
                    {
                        line2--;
                    }
                    if ((w == ' '))
                    {
                        line2 = 0;
                    }
                }
            }
            n = read(fd, buf, 1024);
        }
    }
    else
    {
        while (getline(uline))
        {
            printf("%s\r", uline);
            line2++; //tracking number of lines
            if (line2 > 25)
            {
                n = read(gd, &w, 1);
                if (w == ret || w == nline)
                {
                    line2--;
                }
                if (w == ' ')
                {
                    line2 = 0;
                }
            }
        }
    }
}