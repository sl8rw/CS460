/*********** t.c file of A Multitasking System *********/
#include <stdio.h>
#include "string.h"
#include "type.h"

PROC proc[NPROC];      // NPROC PROCs
PROC *freeList;        // freeList of PROCs 
PROC *readyQueue;      // priority queue of READY procs
PROC *running;         // current running proc pointer

PROC *sleepList;       // list of SLEEP procs

#include "queue.c"     // include queue.c file
#include "wait.c"      // include wait.c file

/*******************************************************
  kfork() creates a child process; returns child pid.
  When scheduled to run, child PROC resumes to body();
********************************************************/
int body(), tswitch(), do_sleep(), do_wakeup(), do_exit(), do_switch();
int do_kfork();

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

int INIT() //uppercase INIT -->
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
  printf("****************************************\n");
  printf(" ps fork switch exit jesus wait sleep wakeup \n");
  printf("****************************************\n");
}

char *status[ ] = {"FREE", "READY", "SLEEP", "ZOMBIE"};

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

int do_jesus()
{
  int i;
  PROC *p;
  printf("Jesus perfroms miracles here\n");
  for (i=1; i<NPROC; i++){
    p = &proc[i];
    if (p->status == ZOMBIE){
      p->status = READY;
      enqueue(&readyQueue, p);
      printf("raised a ZOMBIE %d to live again\n", p->pid);
    }
  }
  printList("readyQueue", readyQueue);
}
    
int body()   // process body function
{
  int c;
  char cmd[64];
  printf("proc %d starts from body()\n", running->pid);
  while(1){
    printf("***************************************\n");
    printf("proc %d running: parent=%d\n", running->pid,running->ppid);
    printList("readyQueue", readyQueue);
    printSleep("sleepList ", sleepList);
    
    menu();
    printf("enter a command : ");
    fgets(cmd, 64, stdin);
    cmd[strlen(cmd)-1] = 0;

    if (strcmp(cmd, "ps")==0)
      do_ps();
    if (strcmp(cmd, "fork")==0)
      do_kfork();
    if (strcmp(cmd, "switch")==0)
      do_switch();
    if (strcmp(cmd, "exit")==0)
      do_exit();
    if (strcmp(cmd, "jesus")==0)
      do_jesus();
   if (strcmp(cmd, "sleep")==0)
      do_sleep();
   if (strcmp(cmd, "wakeup")==0)
      do_wakeup();
    if (strcmp(cmd, "wait")==0)
      do_wait();
  }
}

int kfork()
{
  int  i;
  PROC *p = dequeue(&freeList);
  if (!p){
     printf("no more proc\n");
     return(-1);
  }
  /* initialize the new proc and its stack */
  p->status = READY;
  p->priority = 1;       // ALL PROCs priority=1, except P0
  p->ppid = running->pid;
  p->parent = running;

  //Process Tree
  if(p->parent->child==NULL) //if theres no children
  {
    p->parent->child=p;
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
}
  

  //implement kwait and kexit ---- PAGE 121 MTX for kwait, 
  int kwait(int *status)
  {
      PROC *p, int i, hasChild=0;
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
              put_proc(&freeList,p);
              nproc--;
              return(p->pid);
            }
          }
        }
        if(!hasChild) return -1; //if there is no child, we ERROR
        ksleep(running); //still has children
      }
  }


  int kexit(int exitValue) //page 452 in embedded systems
  {
    int i;
    PROC *p;
    if(running->pid==1)
    {
      return -1;
    }
    slock(&proclock);
    for (i=2;i<NPROC;i++)
    {
      p=&proc[i];
      if((p->status!=FREE) && (p->ppid==running->pid))
      {
        if(p->status==ZOMBIE)
        {
          p->status=FREE;
          putproc(&freeList,p); //releases zombie children
        }
        else
        {
          printf("send child %d to P1\n",p->pid);
          p->ppid=1;
          p->parent=&proc[1];
        }
      }
      sunlock(&proclock);
      running->exitCode=value;
      running->status=ZOMBIE;
      V(&running->parent->wchild); //senaphore thingy
      ttswitch();
    }
  }
  
  /************ new task initial stack contents ************
   kstack contains: |retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
                      -1   -2  -3  -4  -5  -6  -7  -8   -9
  **********************************************************/
  for (i=1; i<10; i++)               // zero out kstack cells
      p->kstack[SSIZE - i] = 0;
  p->kstack[SSIZE-1] = (int)body;    // retPC -> body()
  p->ksp = &(p->kstack[SSIZE - 9]);  // PROC.ksp -> saved eflag 
  enqueue(&readyQueue, p);           // enter p into readyQueue
  return p->pid;
}

int do_kfork()
{
   int child = kfork();
   if (child < 0)
      printf("kfork failed\n");
   else{
      printf("proc %d kforked a child = %d\n", running->pid, child); 
      printList("readyQueue", readyQueue);
   }
   return child;
}

int do_switch()
{
   tswitch();
}

int do_exit()
{
  kexit(running->pid);  // exit with own PID value 
}

int do_wait()
{
  kwait(0);
}

int do_sleep()
{
  int event;
  printf("enter an event value to sleep on : ");
  scanf("%d", &event); getchar();
  sleep(event);
}

int do_wakeup()
{
  int event;
  printf("enter an event value to wakeup with : ");
  scanf("%d", &event); getchar(); 
  wakeup(event);
}


/*************** main() function ***************/
int main()
{
   printf("Welcome to the MT Multitasking System\n");
   init();    // initialize system; create and run P0
   kfork();   // kfork P1 into readyQueue  
   while(1){
     printf("P0: switch process\n");
     while (readyQueue == 0);
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
 //prework 2 P and V

 int P(struct semaphore *s)
{
  s->value--;
  if (s->value < 0){
    printf("proc %d BLOCK\n", running->pid);
    show_buffer();
    running->status = BLOCK;
    enqueue(&s->queue, running);
    tswitch();
  }
}

int V(struct semaphore *s)
{
  PROC *p;
  s->value++;
  if (s->value <= 0){
    p = dequeue(&s->queue);
    p->status = READY;
    enqueue(&readyQueue, p);
    printf("V-up %d\n", p->pid);
    show_buffer();
  }
}


//producer and consumer for prework2
int produce(char c)
{
  BUFFER *p = &buffer;
  P(&p->room);
  P(&p->mutex);
  p->buf[p->head++] = c;
  p->head %= BSIZE;
  V(&p->mutex);
  V(&p->data);
}

int consume()
{
  int c;
  BUFFER *p = &buffer;
  P(&p->data);
  P(&p->mutex);
  c = p->buf[p->tail++];
  p->tail %= BSIZE;
  V(&p->mutex);
  V(&p->room);
  return c;
}
 
int consumer()
{
  char line[128];
  int nbytes, n, i;

  printf("proc %d as consumer\n", running->pid);
 
  while(1){
    printf("input nbytes to read : " );
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;
    sscanf(line, "%d", &nbytes);
    show_buffer();
    for (i=0; i<nbytes; i++){
       line[i] = consume();
       printf("%c", line[i]);
    }
    printf("\n");
    show_buffer();
    printf("consumer %d got n=%d bytes : line=%s\n", running->pid, n, line);
  }
}

int producer()
{
  char line[128];
  int nbytes, n, i;

  printf("proc %d as producer\n", running->pid);

  while(1){
    printf("input a string to produce : " );
    
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;

    nbytes = strlen(line);
    printf("nbytes=%d line=%s\n", nbytes, line);
    show_buffer();
    for (i=0; i<nbytes; i++){
      produce(line[i]);
    }
    show_buffer();
    printf("producer %d put n=%d bytes\n", running->pid, nbytes);
  }
}
