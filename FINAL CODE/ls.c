//no time fields are needed per directions
#include "ucode.c"

//ls_file
//ls_directory
//Page 298-301 in embedded real time operating systems
//refer to chapter 8.6.7 in sys programming book
//thank u so much KC for giving us the code in your book!!!!!!!

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
STAT mystat, *sp;
int fd, n;
DIR *dp;
char *cp;


char buf[BLKSIZE];


int ls_file(char *fname)
{

   
    char sbuf[4096];

    int r = lstat(fname, sp);

    if (S_ISDIR(sp->st_mode))
    {
        printf("%c", 'd');
    }
    if (S_ISREG(sp->st_mode))
    {
        printf("%c", '-');
    }
    if (S_ISLNK(sp->st_mode))
    {
        printf("%c", 'l');
    }
    for (int i=8;i>=0;i--)
    {
        if(sp->st_mode && (1<<i))
        {
            printf("%c", t1[i]); //prints as r w x from embedded rtos
        }
        else
        {
            printf("%c",t2[i]);
        }
        
    }
    printf("%4d ", sp->st_nlink);
    printf("%4d ",sp->st_uid);
    printf("%8d ",sp->st_size);
    //no time needed
    printf("%s", basename(fname));
    if(S_ISLNK(sp->st_mode))
    {
        r=readlink(fname,sbuf);
        printf(" -> %s", sbuf);
    }
    printf("\n");
}

int ls_dir(char *dname)
{
char name[256];
DIR *dp;
struct dirent *ep;
//opening DIR to read names,page 300 embedded rtos
dp=opendir(dname);
while(ep=readdir(dp))
{
    strcpy(name,ep->d_name);
    if(!strcmp(name,".") || !strcmp(name, ".."))
    {
        continue; //skip dot and dot dot
    }
    strcpy(name,dname);
    strcat(name,"/");
    strcat(name,ep->d_name);
    ls_file(name); //call to ls
}
}

int main(int argc, char *argv[])
{
    int r;
    char *s;
    char filename[BLKSIZE];
    char cwd[BLKSIZE];

    s=argv[1]; //ls the filename
    if (argc==1)
    {
        s="./"; //cwd
    }
    sp=&mystat;
    if((r=stat(s,sp))<0)
    {
        printf("ERROR: ls failed\n");
        exit(1);
    }
    strcpy(filename,s);
    if(s[0]!='/')
    {
        getcwd(cwd);
        print2f("cwd line passed 109\n");
        strcat(filename, cwd);
        strcat(filename,"/");
        strcat(filename,s);
        if(S_ISDIR(sp->st_mode))
        {
            ls_dir(filename);
        }
        else
        {
            //we have a file
            ls_file(filename);
        }
        
        
    }
}