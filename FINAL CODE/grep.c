#include "ucode.c"
//refer to page 323 for some grep examples, 8.16.2 command processing -> Embedded sys
char *cp;
char tty[32]; //refer to page 270
char uline[2048], buf[1024];
char nline='\n';
char ret='\r';
//grep pattern filename

//int grep_helper(char *s1, char *s2);
int grep_helper(char *s1, char *s2)
{
    int i;
    int result = 0;
    int amount_to_read=strlen(s1)-strlen(s2); //pattern starts at s1-s2-1
    for (i = 0; i < amount_to_read; i++)
    {
        if (strstr(s1,s2)) //checks for a substring
        {
            result = 1;
            return result;
        }
    }
    return result;
}
int main(int argc, char *argv[])
{
    int fd;
    int n, n2;
    int count;
    int redirect_flag, r1;
    STAT st0;
    STAT sttty;
    gettty(tty); //terminal
    fstat(0, &st0);
    stat(tty, &sttty);
    redirect_flag = 1;
    if (st0.st_ino == sttty.st_ino && st0.st_dev == sttty.st_dev) //device and inode number on that device pg 299
    {
        redirect_flag = 0;
    }
    r1 = 1;
    
    if(argc<2)
    {
        printf("What are you grepping\n?");
        exit(1);
    }

    else if (argc == 2) //pattern
    {
        //two args provided, necessary for the grep command
        if (redirect_flag) //checks redirection flag
        {
            //printf("line in redirect_flag: ");
            n2 = 1;
            while (n2)
            {
                n2 = getline(uline);
                if (grep_helper(uline, argv[1]))
                {
                    printf("%s", uline);
                }
            }
        }
        else
        {
            //printf("line on 69: ");
            //gets(uline);
            //printf("uline: %s\n", uline);
            while (gets(uline))
            {
                //printf("uline: %s", uline);
                if (grep_helper(uline, argv[1]))
                {
                    printf("%s", uline);
                }
            }
        }
    }

    
    else //now pattern and filename
    {
        fd = open(argv[2], O_RDONLY);
        if (fd < 0)
        {
            print2f("ERROR: file failed to be opened\n");
            exit(1);
        }

        printf("%s has been opened for reading\n", argv[2]); //file
        count = 0;

        while (n = read(fd, buf, BLKSIZE))
        {
            buf[n] = 0;
            uline[0] = 0;
            int j = 0;
            for (int i = 0; i < n; i++)
            {
                if (buf[i] == '\n')
                {
                    break;
                }

                if (buf[i] == '\r')
                {
                    break;
                }
                uline[j++] = buf[i];  
                count++;
            }
                uline[j] = 0; //dont forget to clear
                count++;

                if (grep_helper(uline, argv[1])) //pattern
                {
                    write(1,&nline,1);
                    write(2,&ret,1);
                }
                //refer to page 259 (chapter 8.7.3) and 228/236 in systems programming
                //lseek(fd, (long)(n*512), SEEK_SET); //->long type cast
                lseek(fd, (long)count, 0);
            
        }
    }
}
