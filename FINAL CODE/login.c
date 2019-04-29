/****** login.c ******/
/****** Details from p. 322 of book ******/
#include "ucode.c"

int in, out, err;
int gid, uid;
int password_file;
char l_name[64], password[64], terminal[128];
char *pname[8];
char *s;
char buf[BLKSIZE];
char l_username[128], l_password[128];
char *cp;
char *cq;
int n, i;
char line2[64];
char tokenized_file[128];
//refer to page 322 in the book as stated above
//argv[0]=login and argv[1]=/dev/ttyX -->322

int tokenize_password(char *password)
{
    int count = 1;
    int j = 0;
    strcat(password, ":");
    cp = password;
    while (*cp)
    { //tokenize username:password:gid:uid:fullname:HOMEDIR:program
        if (*cp == ':' || *cp == 0)
        {
            *cp++ = 0; //need to start over
            tokenized_file[j] = cp - count;
            count = 1; //reset
            j++;
        }
        else
        {
            cp++;
            count++;
        }
    }
}

int read_password_file(char *s1, int fd)
{
    //remember int fstat(int filedes, struct stat *buf) from 360 systems programming
    STAT stat1;
    fstat(fd, &stat1);
    char ch;
    cp = s1;
    int flag = 0;
    if (read(fd, &ch, 1) != 0)
    {
        while (ch != EOF && ch != '\n' && ch != '\r')
        {
            *cp++ = ch; //we begin reading
            if (read(fd, &ch, 1) == 0)
            {
                flag = 0;
                return flag;
            }
        }
    }
    else if ((read(fd, &ch, 1) == 0) || (ch == EOF))
    {
        flag = 0;
        return flag;
    }
    //refer to page 270 in systems programming
    if (S_ISCHR(stat1.st_mode))
    {
        *cp++ = '\n';
    }
    *cp++ = ch;
    *cp = 0;
    return 1;
}

int main(int argc, char *argv[])
{
    //printf("in mainfunction of shw\n");
    /* 1. Close file descriptors 0, 1 inherited from INIT. */
    char mybuf[256];
    close(0);
    close(1);

    /* 2. Open argv[1] 3 times as in(0), out(1), err(2). */
    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_WRONLY); //this may be 1?

    /* 3. Set tty name string in PROC.tty so PROC knows where its reading/writing to. */
    fixtty(argv[1]); //sets tty in PROC
    gettty(terminal);

    printf("terminal: %s\n", terminal); //this just tells us the terminal

    while (1)
    {
        //opened password file for reading
        password_file = open("/etc/passwd", 0);
        if (password_file < 0)
        {
            printf("No password file\n");
            exit(1);
        }

        /* 5. Prompt user for username/password */
        printf("Login Name: ");
        gets(l_username);
        //printf("name: %s\n", l_name);
        printf("Password: ");
        gets(l_password); // change password to getline after it works

        while (read_password_file(password_file, mybuf) != 0)
        {
            tokenize_password(mybuf);
            //this will return an array, first index is l_username, second is l_password
            if (!strcmp(l_username, tokenized_file[0] && !strcmp(l_password, tokenized_file[1])))
            {
                printf("Hello!!!!!\n");
                printf("You have logged in %s\n", l_username);
                //now we need to chuid,cwd,close file
                chuid(atoi(tokenized_file[3],atoi(tokenized_file[2]));
                chdir(tokenized_file[5]);
                close(password_file);
                exec("sh");
            }
            else if (strcmp(l_username, tokenized_file[0] || strcmp(l_password, tokenized_file[1])))
            {
                printf("ERROR: Incorrect username or password\n");
            }
        }
        lseek(password_file, 0, 0);
    }
}
