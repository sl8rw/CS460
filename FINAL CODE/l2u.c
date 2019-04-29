#include "ucode.c"

//this program converts lower case characters to upper case characters


void convert2upper(char c)
{
    c=c-32;
}

int main(int argc, char *argv[])
{
    int fd;
    int gd;
    int n;
    int ch;
    char buf[1024];
    char *cp;
    
  
    if (argc == 1) //just gonna do it on term using stdin
    {
        while ((ch = getc()) != EOF)
        {
            //while we are reading
            ch &= 0x7F; //interested in first 128 chars
            //printf("%d\n", ch);
            //refer to page 287 in MTX book for conversion upper to lower (reverse logic)
            if (ch >= 'a' && ch <= 'z') //check for lower case
            {
                convert2upper(ch);
                mputc(ch);
            }
            else
            {
                mputc(ch);
            }
            if (ch == '\r')
            {
                mputc('\n'); //if theres a return just make a new line
            }
        }
        exit(0);
    }
    //l2u f1 f2
    if (argc < 3)
    {
        printf("ERROR: 2 file names are needed\n");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);

    if (fd < 0)
    {
        printf("ERROR: File not opened\n");
        exit(1);
    }

    gd = open(argv[2], O_WRONLY | O_CREAT);
    while (n = read(fd, buf, BLKSIZE))
    {
        cp = buf;
        while (cp < buf + n)
        {
            if (*cp >= 'a' && *cp <= 'z')
            {
                convert2upper(*cp);
            }
            cp++;
        }
        write(gd, buf, n);
    }
}
