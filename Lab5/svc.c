/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

int kgetpid()
{
  return running->pid;
}

int kgetppid()
{
  return running->ppid;
}

char *pstatus[]={"FREE   ","READY  ","SLEEP  ","BLOCK  ","ZOMBIE", " RUN  "};
int kps()
{
  int i; PROC *p; 
  for (i=0; i<NPROC; i++){
     p = &proc[i];
     kprintf("proc[%d]: pid=%d ppid=%d", i, p->pid, p->ppid);
     if (p==running)
       printf("%s ", pstatus[5]);
     else
       printf("%s", pstatus[p->status]);
     printf("name=%s\n", p->name);
  }
}

int kchname(char *s)
{ 
  kprintf("kchname: name=%s\n", s);
  strcpy(running->name, s);
  return 123;
}

int ktswitch()
{
  tswitch();
}

int kgetPA()
{
  return running->pgdir[2048] & 0xFFFF0000;
}


int do_wait()
{
    kwait(running->pid);
}

int do_wakeup()
{
  // int k_int;
  int k_int=geti();
  return kwakeup(k_int);
}

int do_sleep()
{
  ksleep(0);
}


int do_exit()
{
  kexit(running->pid);
}

int do_fork()
{
  fork();
}

int do_kfork(char *name)
{
  kfork(name);
}

int do_exec(int selection)
{
  exec(selection);
}

// called from svc_entry in ts.s
int svc_handler(int a, int b, int c, int d)
{
  int r; 

  switch(a){
     case 0: r = kgetpid();          break;
     case 1: r = kgetppid();         break;
     case 2: r = kps();              break;
     case 3: r = kchname((char *)b); break;
     case 4: r = ktswitch();         break;
     //bind kernel function to a case, then break;
     case 5: r = do_wait(); break;
     case 6: r=do_wakeup();break;
     case 7: r=do_sleep(); break;
     case 8: r=do_exit();break;
     case 9: r=do_fork(); break;
     case 10: r=do_kfork(b); break;
     case 90: r = kgetc() & 0x7F;    break;
     case 91: r = kputc(b);          break;
     case 92: r = kgetPA();          break;
     case 93: r=do_exec(b); break;
     default: printf("invalid syscall %d\n", a);
  }
  
  return r;  // return to goUmode in ts.s; replace r0 in Kstack with r
}

