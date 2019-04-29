#include "ucode.c"

//refer to 360 project help cat section
/*
==========================  HOW TO cat ======================================
cat filename:
   char mybuf[1024], dummy = 0;  // a null char at end of mybuf[ ]
   int n;
1. int fd = open filename for READ;
2. while( n = read(fd, mybuf[1024], 1024)){
       mybuf[n] = 0;             // as a null terminated string
       // printf("%s", mybuf);   <=== THIS works but not good
       spit out chars from mybuf[ ] but handle \n properly;
   } 
3. close(fd);
*/

int main(int argc, char *argv[])
{
    int n, i;
    char ch, mybuf[BLKSIZE], temp[128], lastch = '\0';
    int fd = 0;
    char *cp;
    char ret = '\r';
    char nline = '\n';
    int count;

    /* Read and write 1 char at a time bc can't get it to work o.w. */
    i = 0;
    if (argc == 1) //just gonna do it on term
    {

        fd=1; //stdin
    }
    else if (argc > 1)
    {
        fd = open(argv[1], O_RDONLY);
        if (fd < 0)
        {
            printf("ERROR: Cat failed\n");
            exit(0);
        }
    }
    //refer to page 260 and page 261 in Systems Programming
    if(fd!=1)
    {
    while (n = read(fd, mybuf, BLKSIZE))
    {
        mybuf[n] = 0;
        cp = mybuf;
        if (fd)
        {
            //i = 0;
            for (i = 0; i < n; i++)
            {
                write(STDOUT, &mybuf[i], 1);
                if (mybuf[i] == '\n')
                {
                    write(STDERR, &ret, 1);
                }
            }
        }

        else
        {
            cp = mybuf;
            if (*cp == '\r') //because we are at a new line
            {
                write(STDERR, &nline, 1);
            }
            write(STDOUT, cp, 1);
        }
    }
    }
    else
    {
        count=0;
        while(n=read(STDIN,&ch,1)) 
        {
            count++; //gonna count
            if(ch != nline || ch != ret)
            {
                write(STDOUT,&ch,1);
            }
            if (ch==ret)
            {
                if(lastch != ret || lastch!=nline)
                {
                    write(STDOUT,&nline,1);
                    write(2,&ret,1);
                }
                temp[i]=0;
                write(2,temp,128);
                i=0;
                write(2,&nline,1);
                write(2,&ret,1);
            }
            else
            {
                temp[i]=ch;
                i++;
            }
            lastch=ch;
        }

        if(count >=1024)
        {
            //now we have read more thna 1024
            while(n=read(fd,mybuf,1024))
            {
                //0 out buf
                mybuf[n]=0;
                for(i=0;i<n;i++)
                {
                    ch=mybuf[i];
                }
                if(ch!=nline && ch!=ret)
                {
                    write(STDOUT,&ch,1);
                }
                if(ch==ret)
                {
                    if(lastch!=nline || lastch!=ret)
                    {
                        write(1,&nline,1);
                        write(2,&ret,1);
                    }
                    //set the lastch
                    lastch=ch;
                }
            }
        }
    }
    



    close(fd);
    exit(0);
}
