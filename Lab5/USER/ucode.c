typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#include "string.c"
#include "uio.c"

int ubody(char *name)
{
  int pid, ppid;
  char line[64];
  u32 mode,  *up;

  mode = getcsr();
  mode = mode & 0x1F;
  printf("CPU mode=%x\n", mode);
  pid = getpid();
  ppid = getppid();

  while(1){
    printf("This is process #%d in Umode at %x parent=%d\n", pid, getPA(),ppid);
    
    umenu();
    printf("\nthis is from %s body\n", name);
    printf("Process parent: %d\n", ppid);
    printf("Process id: %d\n", pid);
    printf("input a command: ");
    ugetline(line); 
    uprintf("\n"); 
 
    if (strcmp(line, "getpid")==0)
       ugetpid();
    if (strcmp(line, "getppid")==0)
       ugetppid();
    if (strcmp(line, "ps")==0)
       ups();
    if (strcmp(line, "chname")==0)
       uchname();
    if (strcmp(line, "switch")==0)
       uswitch();
    if (strcmp(line, "wait")==0)
      uwait();
    if (strcmp(line,"wakeup")==0)
      uwakeup();
    if (strcmp(line, "sleep")==0)
      usleep();
    if (strcmp(line, "fork")==0)
       ufork();
    if (strcmp(line, "kfork")==0)
      ukfork(name);
    if (strcmp(line, "exec")==0)
       uexec();
    if (strcmp(line, "exit")==0)
       uexit();

  }
}

int umenu()
{
  uprintf("-------------------------------------------------\n");
  uprintf("getpid getppid ps chname switch wait wakeup sleep exec exit\n");
  uprintf("-------------------------------------------------\n");
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}    

int getppid()
{ 
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}

int uchname()
{
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  printf("\n");
  return syscall(3,s,0,0);
}

int uswitch()
{
  return syscall(4,0,0,0);
}

int uwait()
{
  return syscall(5,0,0,0);

}

int uwakeup()
{
  return syscall(6,0,0,0);
}

int usleep()
{
  return syscall(7,0,0,0);
}

int uexec()
{
  char *cmd;
  kprintf("What command?\n");
  ugetline(cmd);
  return syscall(93,cmd,0,0);
}
int uexit()
{
  return syscall(8,0,0,0);
}

int ufork()
{
  return syscall(9,0,0,0); //binding, parameters.......
}

int ukfork(char *name)
{
  return syscall(10,name,0,0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}

int getPA()
{
  return syscall(92,0,0,0);
}
