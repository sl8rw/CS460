int tswitch();

int ksleep(int event)
{
  int sr = int_off();
  printf("proc %d going to sleep on event=%d\n", running->pid, event);

  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printList("sleepList", sleepList);
  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
  int sr = int_off();
  
  printList("sleepList", sleepList);

  while (p = dequeue(&sleepList)){
     if (p->event == event){
	printf("wakeup %d\n", p->pid);
	p->status = READY;
	enqueue(&readyQueue, p);
     }
     else{
	enqueue(&temp, p);
     }
  }
  sleepList = temp;
  printList("sleepList", sleepList);
  int_on(sr);
}

  int kexit(int exitValue) //page 183 in MTX
  {
    int i;
    PROC *p;
    if(running->pid==1) //dont wanna kill process 1
    {
      return -1;
    }
    

    for (i=2;i<NPROC;i++)
    {
      p=&proc[i];
      if((p->status!=FREE) && (p->ppid==running->pid))
      {
        if(p->status==ZOMBIE)
        {
          p->status=FREE;
          // putproc(&freeList,p); //releases zombie children
          enqueue(&freeList,&proc[i]);
        }
        else
        {
          printf("sent child %d to P1\n",p->pid);
          p->ppid=1;
          p->parent=&proc[1];
        }
      }
      
      running->exitCode=exitValue;
      running->status=ZOMBIE;
      kwakeup(&proc[1]);
      kwakeup(running->parent);

      tswitch();
    }
  }


  
