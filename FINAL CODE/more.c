#include "ucode.c"

//more usage is more filename with filename optional, if no filename us stdin

#define DISPLAYED_LINES 20
char buf[1024], myline[256];
int linecount = 0;
int index = 0;
char tty[64];
char temp;
char ret = '\r';
char nline = '\n';
char spacebar = ' ';

// void print_lines(int fd, int n)
// {
//     char ch;
//     char lastch;
//     int n_read=0;
//     int count=0;
//     while(read(fd,&ch,1))
//     {
//         line[index]=ch;
//         if(ch==nline)
//         {
//             if(lastch!=nline && lastch!=ret)
//             {
//                 linecount++;
//                 index=0;
//                 printf("%s",line);
//                 for(int i=0;i<256;i++)
//                 {
//                     line[i]=0; //0s out string
//                 }
//             }
//         }

//         }
//     }
// }
int main(int argc, char *argv[])
{
    int fd = 0;
    int gd;
    int i, n;
    char lastch;
    //int count;
    char ch;
    char keypress;

    if (argc == 1) //term
    {
        gettty(tty);
        fd = open(tty, O_RDONLY);
        while (getline(myline)) //get us a string from terminal easy to measruee
        {
            printf("%s\r", myline); //printing to the terminal window
            linecount++;            //tracking number of lines
            if (linecount > DISPLAYED_LINES)
            {
                n = read(fd, &ch, 1);
                if (ch == ret || ch == nline) //display ONLY 1 LINE
                {
                    linecount--;
                }
                if (ch == spacebar)
                {
                    linecount = 0; //display 20 more lines
                }
            }
        }
    }
    else
    {
        i = 0;
        while (argc > i)
        {

            fd = open(argv[i++], O_RDONLY);
            while (read, &ch, 1)
            {
                line[index] = ch;
                if (ch == nline)
                {
                    if (lastch != nline && lastch != ret)
                    {
                        linecount++;
                        index = 0;
                        printf("%s", line);
                        for (i = 0; i < 256; i++)
                        {
                            line[i] = 0; //0s out string
                        }
                    }
                }
                if (linecount == DISPLAYED_LINES)
                {
                    keypress = getc(); //receives char
                    if (keypress == spacebar)
                    {
                        linecount = 0;
                        break;
                    }
                    if (keypress == nline || keypress == ret)
                    {
                        linecount--;
                    }
                }
                lastch = ch;
            }
        }
    }
}
