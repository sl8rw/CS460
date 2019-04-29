#include "type.h"
#include "string.c"

PROC proc[NPROC];      // NPROC PROCs
PROC *freeList;        // freeList of PROCs 
PROC *readyQueue;      // priority queue of READY procs
PROC *running;         // current running proc pointer

PROC *sleepList;       // list of SLEEP procs
int procsize = sizeof(PROC);

#define printf kprintf
#define gets kgets

#include "kbd.c"
#include "vid.c"
#include "exceptions.c"
#include "pipe.c"

#include "queue.c"
#include "wait.c"      // include wait.c file

/*******************************************************
  kfork() creates a child process; returns child pid.
  When scheduled to run, child PROC resumes to body();
********************************************************/
int body(), tswitch(), do_sleep(), do_wakeup(), do_exit(), do_switch();
int do_kfork();
int scheduler();

int kprintf(char *fmt, ...);

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    if (vicstatus & 0x80000000){ // SIC interrupts=bit_31=>KBD at bit 3 
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

// initialize the MT system; create P0 as initial running process
int init() 
{
  int i;
  PROC *p;
  for (i=0; i<NPROC; i++){ // initialize PROCs
    p = &proc[i];
    p->pid = i;            // PID = 0 to NPROC-1  
    p->status = FREE;
    p->priority = 0;      
    p->next = p+1;
  }
  proc[NPROC-1].next = 0;  
  freeList = &proc[0];     // all PROCs in freeList     
  readyQueue = 0;          // readyQueue = empty

  sleepList = 0;           // sleepList = empty
  
  // create P0 as the initial running process
  p = running = dequeue(&freeList); // use proc[0] 
  p->status = READY;
  p->priority = 0;
  p->ppid = 0;             // P0 is its own parent
  
  printList("freeList", freeList);
  printf("init complete: P0 running\n"); 
}

int INIT()
{
  int pid, status;
  PIPE *p = &pipe;
  printf("P1 running: create pipe and writer reader processes\n");
  kpipe();
  kfork(pipe_writer);
  kfork(pipe_reader);
  printf("P1 waits for ZOMBIE child\n");
  while(1){
    pid = kwait(&status);
    if (pid < 0){
      printf("no more child, P1 loops\n");
      while(1);
    }
    printf("P1 buried a ZOMBIE child %d\n", pid);
  }
}

int menu()
{
  printf("**********************************\n");
  printf(" ps fork switch  wait exit sleep wakeup \n");
  printf("**********************************\n");
}

char *status[ ] = {"FREE", "READY", "SLEEP", "ZOMBIE"};

int kwait(int *status)
  {
      PROC *p; int i, hasChild=0;
      while(1) //whole PROCs for a child
      {
        for (int i=1;i<NPROC;i++) //skip P0
        {
          p=&proc[i];
          if(p->status!=FREE && p->ppid==running->pid)
          {
            hasChild=1;
            if(p->status==ZOMBIE)
            {
              *status=p->exitCode;
              p->status=FREE;
              enqueue(&freeList,p);
              
              return(p->pid);
            }
          }
        }
        if(!hasChild) return -1; //if there is no child, we ERROR
        ksleep(running); //still has children
      }
  }



  

int do_ps()
{
  int i;
  PROC *p;
  printf("PID  PPID  status\n");
  printf("---  ----  ------\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    printf(" %d    %d    ", p->pid, p->ppid);
    if (p == running)
      printf("RUNNING\n");
    else
      printf("%s\n", status[p->status]);
  }
}
    
// int body()   // process body function
// {
//   int c;
//   char cmd[64];
//   printf("proc %d starts from body()\n", running->pid);
//   while(1){
//     printf("***************************************\n");
//     printf("proc %d running: parent=%d\n", running->pid,running->ppid);
//     printList("readyQueue", readyQueue);
//     printSleepList(sleepList);
//     menu();
//     printf("enter a command : ");
//     gets(cmd);
    
//       if (strcmp(cmd, "ps")==0)
//       do_ps();
//     // if (strcmp(cmd, "fork")==0)
//     //   do_kfork();
//     if (strcmp(cmd, "switch")==0)
//       do_switch();
//     if (strcmp(cmd, "exit")==0)
//       do_exit();
//     if (strcmp(cmd, "jesus")==0)
//       // do_jesus();
//    if (strcmp(cmd, "sleep")==0)
//       do_sleep();
//    if (strcmp(cmd, "wakeup")==0)
//       do_wakeup();
//     if (strcmp(cmd, "wait")==0)
//       kwait(running->pid);
//   }
// }

int kfork(int func)
{
  int  i;
  PIPE *pp = &pipe;
  PROC *p = dequeue(&freeList);
  if (!p){
     printf("no more proc\n");
     return(-1);
  }
  /* initialize the new proc and its stack */
  p->status = READY;
  p->priority = 1;       // ALL PROCs priority=1,except P0
  p->ppid = running->pid;
  p->parent = running;
  p->child = 0;
  p->sibling = 0;
  //Process Tree
  if(p->parent->child==0) //if theres no children
  {
    p->parent->child=p;
    printf("%d\n",p);
  }
  else
  {
    PROC *pTemp=p->parent->child; //there is a child
    while(pTemp)
    {
    pTemp=pTemp->sibling; //going to keep progressing through the list
    }
    pTemp->sibling=pTemp;
  }

  // enter_child(p);
  
  /************ new task initial stack contents ************
   kstack contains: |pid|exit|retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
                     -1   -2   -3   -4  -5  -6  -7  -8  -9  -10  -11
  **********************************************************/
  for (i=1; i<15; i++)                 // zero out kstack cells
      p->kstack[SSIZE - i] = 0;

  p->kstack[SSIZE-1] = (int)func;      // retPC -> func()
  p->ksp = &(p->kstack[SSIZE - 14]);   // PROC.ksp -> saved eflag 
  enqueue(&readyQueue, p);             // enter p into readyQueue
  
  return p->pid;
}

/*int do_kfork()
{
   int child = kfork();
   if (child < 0)
      printf("kfork failed\n");
   else{
      printf("proc %d kforked a child = %d\n", running->pid, child); 
      printList("readyQueue", readyQueue);
   }
   return child;
}*/

int do_switch()
{
   tswitch();
}

int do_exit()
{
  kexit(running->pid);  // exit with own PID value 
}

int do_sleep()
{
  
  int event;
  printf("enter an event value to sleep on : ");
  event = geti();
  ksleep(event);
}

int do_wakeup()
{
  int event;
  printf("enter an event value to wakeup with : ");
  event = geti();
  kwakeup(event);
}

int main()
{ 
   int i; 
   char line[128]; 
   u8 kbdstatus, key, scode;
   KBD *kp = &kbd;
   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kprintf("Welcome to Wanix in ARM\n");
   kbd_init();
   
   /* enable SIC interrupts */
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
   /* enable KBD IRQ */
   SIC_INTENABLE = (1<<3); // KBD int=bit3 on SIC
   SIC_ENSET = (1<<3);  // KBD int=3 on SIC
   *(kp->base+KCNTL) = 0x12;

   int pid, status;
   init();

  //  printQ(readyQueue);
   kfork(INIT);

   printList("readyQueue", readyQueue);
   while(1){
     printf("P0 switch process\n");
     while(!readyQueue);
     tswitch();
   }
}



/*********** scheduler *************/
int scheduler()
{ 
  printf("proc %d in scheduler()\n", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  printList("readyQueue", readyQueue);
  running = dequeue(&readyQueue);
  printf("next running = %d\n", running->pid);  
}


